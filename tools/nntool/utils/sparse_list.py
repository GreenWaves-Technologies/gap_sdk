from collections.abc import MutableSequence, Iterable

class SparseList(MutableSequence):
    def __init__(self, arg=None, default_value=None):
        self.default = default_value
        self.elements = {}
        self.size = 0
        if isinstance(arg, int):
            self.size = int(arg)
        elif isinstance(arg, dict):
            self.__initialise_from_dict(arg)
        elif isinstance(arg, Iterable):
            self.__initialise_from_iterable(arg)

    def insert(self, index, value):
        to_increment = list([v for v in self.elements if v >= index])
        for k in to_increment:
            v = self.elements[k]
            del self.elements[k]
            self.elements[k + 1] = v
        self.elements[index] = value

    def __len__(self):
        return self.size

    def population(self):
        return len(self.elements)

    def __setitem__(self, index, value):
        try:
            if index.start:
                self.size = max(self.size, index.start + len(value))
            s = slice(index.start, index.stop, index.step).indices(self.size)
            for v, i in enumerate(range(*s)):
                self.__setitem__(i, value[v])
        except AttributeError:
            if value != self.default:
                self.elements[index] = value
            self.size = max(index + 1, self.size)

    def __getitem__(self, index):
        try:
            s = slice(index.start, index.stop, index.step).indices(self.size)
            indices = range(*s)
            sl = SparseList(
                {
                    k: self.elements[i]
                    for k, i in enumerate(indices)
                    if i in self.elements
                }, self.default)
            sl.size = len(indices)
            return sl
        except AttributeError:
            i = slice(index).indices(self.size)[1]
            return self.elements.get(i, self.default)

    def __delitem__(self, item):
        if isinstance(item, slice):
            keys_to_remove = range(*item.indices(self.size))
        elif item < 0:
            keys_to_remove = (self.size + item, )
        else:
            keys_to_remove = (item, )

        if not keys_to_remove:
            return

        keys_removed = 0
        removing_tail = keys_to_remove[-1] == self.size - 1

        for current_key in sorted(self.elements.keys()):
            if current_key < keys_to_remove[0]:
                continue

            elif keys_removed < len(keys_to_remove) and current_key > keys_to_remove[keys_removed]:
                keys_removed += 1

            if keys_removed and not removing_tail:
                self.elements[current_key - keys_removed] = self.elements[current_key]

            del self.elements[current_key]

        self.size -= len(keys_to_remove)

    def __iter__(self):
        for index in range(self.size):
            yield self[index]

    def __contains__(self, index):
        return index in self.elements.values()

    def __repr__(self):
        return '[{}]'.format(', '.join([str(e) for e in self]))

    def __add__(self, other):
        result = self[:]
        return result.__iadd__(other)

    def __iadd__(self, other):
        for element in other:
            self.append(element)
        return self

    def append(self, element):
        '''
        append element, increasing size by exactly one
        '''
        if element != self.default:
            self.elements[self.size] = element
        self.size += 1

    push = append

    def __initialise_from_dict(self, arg):
        def __convert_and_size(key):
            try:
                key = int(key)
            except ValueError:
                raise ValueError('Invalid key: {}'.format(key))
            self.size = max(key + 1, self.size)
            return key
        self.elements = {__convert_and_size(k): v for k, v in arg.items() if v != self.default}

    def __initialise_from_iterable(self, arg):
        for v in arg:
            self.append(v)

    def __eq__(self, other):
        return len(self) == len(other) and all(a == b for a, b in zip(self, other))

    def __ne__(self, other):
        return not self.__eq__(other)

    def __lt__(self, other):
        for a, b in zip(self, other):
            if a < b:
                return True
            if a > b:
                return False
        return len(self) < len(other)

    def __ge__(self, other):
        return not self.__lt__(other)

    def __mul__(self, multiplier):
        result = self[:]
        for _ in range(multiplier - 1):
            result += self[:]
        return result

    def count(self, value):
        '''
        return number of occurrences of value
        '''
        return sum(v == value for v in self.elements.values()) + (
            self.size - len(self.elements) if value == self.default else 0
        )

    def extend(self, iterable):
        '''
        extend sparse_list by appending elements from the iterable
        '''
        self.__iadd__(iterable)

    def index(self, value):
        '''
        return first index of value.
        Raises ValueError if the value is not present.
        '''

        if value == self.default:
            for k, v in enumerate(self):
                if v == value:
                    return k
            raise ValueError('{} not in SparseList'.format(value))
        for k, v in self.elements.items():
            if v == value:
                return k
        raise ValueError('{} not in SparseList'.format(value))

    def pop(self):
        '''
        remove and return item at end of SparseList
        Raises IndexError if list is empty.
        '''
        if self.size < 1:
            raise IndexError('pop from empty SparseList')
        value = self[-1]
        del self[-1]
        return value

    def remove(self, value):
        '''
        remove first occurrence of value.
        Raises ValueError if the value is not present.
        '''
        if value == self.default:
            return
        for k, v in self.elements.items():
            if v == value:
                del self.elements[k]
                return
        raise ValueError('{} not in SparseList'.format(value))