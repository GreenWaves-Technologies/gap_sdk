
from math import sqrt, floor
from functools import reduce

def largest_factor(sz):
    limit = int(floor(sqrt(sz)))
    for i in range(2, limit + 1):
        c = 0
        for j in range(1, i + 1):
            if (i % j) == 0:
                c += 1
        if c == 2 and sz % i == 0:
            return sz // i
    return 1

def prime_factors_generator(num):
    while num % 2 == 0:
        yield 2
        num /= 2
    for i in range(3, int(sqrt(num))+1, 2):
        if num % i == 0:
            yield i
            num /= i
    if num > 2:
        yield int(num)

def split_list(l):
    return l[0::2], l[1::2]

def product(l):
    return reduce(lambda x, y: x * y, l)

def balanced_divisors(num):
    if num == 1:
        return (1, 1)
    factors = sorted(list(prime_factors_generator(num)))
    f1, f2 = split_list(factors)
    return (product(f1), product(f2))
