#
# Copyright (C) 2019 GreenWaves Technologies
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import errors
import gen_flash_image
import gen_readfs
import gen_lfs
import traces
import os
import shutil
import importlib
import tools.runner.runner


class Runner(object):

    def __init__(self, args, config, system):
        self.config = config
        self.args = args
        self.system = system
        self.verbose = config.get_bool('gapy/verbose')

    def run(self):
        self.conf()

        if self.args.all or self.args.image:
            if self.image() != 0:
                return -1

        if self.args.all or self.args.flash:
            if self.flash() != 0:
                return -1

        if self.args.all or self.args.exec_prepare:
            if self.exec_prepare() != 0:
                return -1

        if self.args.all or self.args.exec:
            if self.exec() != 0:
                return -1

        if self.args.gtkw:
            if self.gtkw() != 0:
                return -1

        return 0


    def conf(self):

        if  self.config.get('runner/targets') is None:
            self.__handle_target('runner')
        else:
            for target in self.config.get('runner/targets').get_dict():
                self.__handle_target(target + '/runner', flash_device_prefix=target + '/')


    def __handle_target(self, target, flash_device_prefix=None):

        boot_mode = self.config.get_str(target + "/boot/mode")
        if boot_mode is None: 
            raise errors.InputError('The boot mode has to be specified')

        binary = self.config.get_str('runner/boot-loader')

        # If we boot from flash, store the boot binary information in the specified flash
        if boot_mode == 'flash':
            flash_config = self.get_boot_flash(target, flash_device_prefix)
            flash_config.set('content/boot-loader', binary)

        work_dir = self.config.get_str('gapy/work_dir')


        # Go through all the specified flashes to generate image and stimuli if needed
        for flash_path in self.config.get_py(target + '/flash_devices'):

            if flash_device_prefix is not None:
                flash_path = flash_device_prefix + flash_path

            traces.info('Building stimuli for flash ' + flash_path)

            flash_config = self.config.get(flash_path)
            if flash_config is None:
                raise errors.InputError('Invalid flash device: ' + flash_path)

            if flash_config.get('content/image') is None:

                gen_image = False
                flash_image = self.args.force

                # The flash can contain the boot binary and several partitions for FS
                if flash_config.get('content/boot-loader') is not None:
                    flash_image = True
                    gen_image = True

                if flash_config.get('content/partitions') is not None:

                    for name, partition in flash_config.get('content/partitions').get_items().items():

                        gen_image = True

                        type_name = partition.get_str('type')

                        if type_name not in ['hostfs'] and ((partition.get('files') is not None and len(partition.get('files').get_dict()) != 0) or (partition.get_str('root_dir'))):
                            flash_image = True

                        if type_name is not None:
                            img_path = os.path.join(work_dir, flash_path.replace('/', '.') + '.' + name + '.img')
                            partition.set('image', img_path)

                if gen_image:
                    img_path = os.path.join(work_dir, flash_path.replace('/', '.') + '.img')
                    flash_config.set('content/gen_image', True)
                    flash_config.set('content/image', img_path)
                    flash_config.set('content/flash', flash_image)

            stim_format = flash_config.get_str("models/%s/stimuli/format" % self.args.platform)

            if stim_format is not None:
                file_name = flash_config.get_str("models/%s/stimuli/file" % self.args.platform)
                if file_name is None:
                    file_name = flash_path.replace('/', '.') + '.' + stim_format

                file_path = os.path.join(work_dir, file_name)
                    
                flash_config.set("content/stimuli/format", stim_format)
                flash_config.set("content/stimuli/file", file_path)

            if flash_config.get('fs/encrypt') is not None:
                flash_config.set('fs/encrypt', self.args.encrypt)
                flash_config.set('fs/aes_key', self.args.aes_key)
                flash_config.set('fs/aes_iv', self.args.aes_iv)


    def image(self):

        # TODO
        # At long term, all the image build, stimuli and so on should be handled by the
        # system scripts. For now we just do it for efuse and keep the reset with the old
        # mechanism
        if self.args.py_target is not None:
            if False:
                spec = importlib.util.spec_from_file_location("target", self.args.py_target)
                module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(module)
                module.get_config(options=self.args.config_items).gen_stimuli(self.config.get_str('gapy/work_dir'))
            else:
                class_name, module_name = self.args.py_target.split('@')
                module = importlib.import_module(module_name)

                if not callable(getattr(module, class_name, None)):
                    raise RuntimeError('Unable to find target class (method: %s, filepath: %s)' % (class_name, module_name))

                tools.runner.runner.Runner(None, 'top', options=self.args.config_items, target_class=getattr(module, class_name, None)).gen_stimuli(self.config.get_str('gapy/work_dir'))

        self.gen_stimuli()
        return 0

    def flash(self):
        return 0

    def gtkw(self):
        return 0

    def exec(self):
        return 0

    def exec_prepare(self):
        return 0

    def get_boot_mode(self, target='runner'):
        return self.config.get_str('%s/boot/mode' % target)

    def get_flashs(self, target='runner', flash_device_prefix=None):
        result = []
        for flash_path in self.config.get_py(target + '/flash_devices'):
            if flash_device_prefix is not None:
                flash_path = flash_device_prefix + flash_path

            result.append(self.config.get(flash_path))

        return result


    def get_boot_flash(self, target='runner', flash_device_prefix=None):
        flash_path = self.config.get_str("%s/boot/device" % target)
        if flash_path is None:
            raise errors.InputError('The path to the flash device must be specified when booting from flash')

        if flash_device_prefix is not None:
            flash_path = flash_device_prefix + flash_path

        flash_config = self.config.get(flash_path)
        if flash_config is None:
            raise errors.InputError('Invalid flash device specified for boot: ' + flash_path)

        return flash_config

        
    def get_boot_flash_type(self):
        flash_config = self.get_boot_flash()
        return flash_config.get_str('datasheet/type')


    def gen_stimuli(self):
        traces.info('Building stimuli')

        # Generate the flash image for every flash
        self.gen_flash_images()


    def gen_flash_images(self):

        if  self.config.get('runner/targets') is None:
            self.__gen_flash_images_for_target('runner')
        else:
            for target in self.config.get('runner/targets').get_dict():
                self.__gen_flash_images_for_target(target + '/runner', flash_device_prefix=target + '/')

        
    def __gen_flash_images_for_target(self, target, flash_device_prefix=None):

        work_dir = self.config.get_str('gapy/work_dir')

        # Go through all the specified flashes to generate image and stimuli if needed
        for flash_path in self.config.get_py(target+ '/flash_devices'):

            if flash_device_prefix is not None:
                flash_path = flash_device_prefix + flash_path

            traces.info('Building stimuli for flash ' + flash_path)

            flash_config = self.config.get(flash_path)
            if flash_config is None:
                raise errors.InputError('Invalid flash device: ' + flash_path)

            if flash_config.get_bool('content/gen_image'):
                if flash_config.get('content/partitions') is not None:

                    for name, partition in flash_config.get('content/partitions').get_items().items():

                        type_name = partition.get_str('type')

                        if type_name is not None:
                            if type_name == 'readfs':
                                gen_readfs.main(config=self.config, partition_config=partition)
                            elif type_name == 'lfs':
                                gen_lfs.main(config=self.config, partition_config=partition)
                            elif type_name == 'hostfs':
                                work_dir = self.config.get_str('gapy/work_dir')
                                for file in partition.get('files').get_dict():
                                    shutil.copy(file, work_dir)
                            else:
                                raise errors.InputError('Invalid partition type: ' + type_name)

                if flash_config.get('content/image') is not None:
                    gen_flash_image.main(config=self.config, flash_config=flash_config)
