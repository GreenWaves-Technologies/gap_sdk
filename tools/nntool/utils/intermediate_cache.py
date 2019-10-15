# Copyright (C) 2019 GreenWaves Technologies
# All rights reserved.

# This software may be modified and distributed under the terms
# of the BSD license.  See the LICENSE file for details.

import logging
import os
import pickle
import sys

import xxhash
import numpy as np

from execution.execute_graph import execute_iterator

INPUT_EXTENSION = '.input'
OUTPUT_EXTENSION = '.output'
MB = 1000000

LOG = logging.getLogger('nntool.'+__name__)

def get_file_size(filepath):
    try:
        return os.path.getsize(filepath)
    except FileNotFoundError:
        return 0

def get_size(start_path='.'):
    total_size = 0
    for dirpath, _, filenames in os.walk(start_path):
        for fname in filenames:
            if not (fname.endswith(INPUT_EXTENSION) or fname.endswith(OUTPUT_EXTENSION)):
                continue
            fpath = os.path.join(dirpath, fname)
            # skip if it is symbolic link
            if not os.path.islink(fpath):
                total_size += os.path.getsize(fpath)

    return total_size

def hash_tensor(tensor):
    h = xxhash.xxh64()
    h.update(tensor.data)
    return h.hexdigest()

class IntermediateCacheError(Exception):
    pass

class IntermediateCache:

    def __init__(self, cache_dir, cache_size_mb=1000, memory_size_mb=100):
        self.cache_dir = cache_dir
        self.cache_size = cache_size_mb * MB
        self.memory_size = memory_size_mb * MB
        self.current_cache_size = get_size(cache_dir)
        self.current_memory_size = 0
        self.cache_queue = []
        self.cache = {}

    def preload(self, G, input_tensors):
        for input_tensor in input_tensors:
            self.cache_intermediates(G, input_tensor)

    def cache_path_prefix(self, input_tensor, input_hash, create_bucket=True):
        cache_bucket = os.path.join(self.cache_dir, input_hash)
        if not os.path.exists(cache_bucket):
            if create_bucket:
                os.makedirs(cache_bucket)
                return os.path.join(cache_bucket, "0")
            return None

        cache_entry = 0
        while True:
            cache_path_prefix = os.path.join(cache_bucket, str(cache_entry))
            cache_input_filename = cache_path_prefix + INPUT_EXTENSION
            if os.path.isfile(cache_input_filename):
                with open(cache_input_filename, 'rb') as input_fp:
                    cached_input_tensor = pickle.load(input_fp)
                    if np.array_equal(input_tensor, cached_input_tensor):
                        return cache_path_prefix
            else:
                break
            cache_entry += 1

        if create_bucket:
            os.makedirs(cache_bucket)
            return os.path.join(cache_bucket, cache_entry)

        return None

    def write_entry(self, cache_path_prefix, input_tensor, outputs):
        input_file = cache_path_prefix + INPUT_EXTENSION
        output_file = cache_path_prefix + OUTPUT_EXTENSION
        for filename in [input_file, output_file]:
            self.current_cache_size -= get_file_size(filename)

        for filename, contents in [(input_file, input_tensor), (output_file, outputs)]:
            with open(filename, 'wb') as file_fp:
                pickle.dump(contents, file_fp)
                self.current_cache_size += get_file_size(filename)
                if self.current_cache_size > self.cache_size:
                    raise IntermediateCacheError("disk cache is full")

    def cache_intermediates(self, G, input_tensor, cache=True):
        outputs = []
        fusion_outputs = []

        for _, _, _, output, fusion_op_name, _, _ in\
            execute_iterator(G, [input_tensor], disable_cache=True):

            if fusion_outputs:
                # handle end of fusion
                if fusion_op_name is None:
                    # python upvalues are not as smart as lua
                    outputs.append([out.copy() for out in fusion_outputs])
                    fusion_outputs.clear()
                    continue
            # handle start or continuation of fusion
            if fusion_op_name is not None:
                fusion_outputs.append([out.copy() for out in output])
                continue

            outputs.append([out.copy() for out in output])

        input_hash = IntermediateCache.__get_hash(G, input_tensor)
        cache_path_prefix = self.cache_path_prefix(input_tensor, input_hash)
        self.write_entry(cache_path_prefix, input_tensor, outputs)

        if cache:
            self.add_to_cache(input_hash, (input_tensor, outputs))
            return self.cache[input_hash]
        return None

    def remove_from_queue(self, input_hash):
        try:
            self.cache_queue.remove(input_hash)
        except ValueError:
            pass

    def __remove_from_memory(self, input_hash):
        self.remove_from_queue(input_hash)
        elem = self.cache[input_hash]
        del self.cache[input_hash]
        self.current_cache_size -= (sys.getsizeof(elem))

    def add_to_cache(self, input_hash, cache_entry):
        if input_hash in self.cache:
            self.__remove_from_memory(input_hash)

        size = sys.getsizeof(cache_entry)
        self.drop_until_fit(size)
        self.current_memory_size += size
        self.cache_queue.insert(0, input_hash)
        self.cache[input_hash] = cache_entry

    def pop_last(self):
        input_hash = self.cache_queue[-1]
        self.__remove_from_memory(input_hash)

    def drop_until_fit(self, size):
        while (self.current_memory_size + size) > self.memory_size and\
            self.cache_queue:
            self.pop_last()

    @staticmethod
    def __get_hash(G, input_tensor):
        # The graph identity contains everything that makes a graph unique
        return "{}_{}".format(G.graph_identity.hexdigest, hash_tensor(input_tensor))

    def get_outputs(self, G, input_tensor):
        input_hash = IntermediateCache.__get_hash(G, input_tensor)
        cached_entry = self.cache.get(input_hash)
        if cached_entry is not None and\
           np.array_equal(input_tensor, cached_entry[0]):
            if self.cache_queue[0] != input_hash:
                self.remove_from_queue(input_hash)
                self.cache_queue.insert(0, input_hash)
            return cached_entry[1]

        cache_path_prefix = self.cache_path_prefix(input_tensor,
                                                   input_hash,
                                                   create_bucket=False)
        if cache_path_prefix is None:
            return self.cache_intermediates(G, input_tensor)[1]

        with open(cache_path_prefix + OUTPUT_EXTENSION, "rb")\
            as tensors_fp:
            outputs = pickle.load(tensors_fp)
            self.add_to_cache(input_hash, (input_tensor, outputs))
            return outputs
