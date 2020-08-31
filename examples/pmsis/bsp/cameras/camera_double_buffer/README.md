# Camera double buffering

## Requirements

- HIMAX Camera

## Description

This test shows how to process an image while capturing the next one.
Doing this instead of sequentially capturing an image and then processing it will be faster.
However, this speedup comes at the cost of memory usage.
