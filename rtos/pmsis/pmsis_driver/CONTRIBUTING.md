# Contribution to this repository should follow some basic rules:

## The coding style is based on gnu-c and common sense: 
### That is, all variables and function names should not be using any capital letters, and separations must be made using underscores
### Defines must be in capital letters
### functions/if-else blocks/while/for and all blocks requiring curly brackets ('{}') should have their brackets on a new line bellow the declaration
### No keyword must come after a curly bracket, with the exception of the while in do-while blocks
### one liners (if-else on same line for example) are forbidden for readability issues
### As much as possible, code lines must not be more than 80 characters wide (and definitely not more than 90), use line breaks if needed
### All functions must be as documented as possible, even local ones, in particular add comments for any parts in asm, or for complicated algorithms

## Function names must begin with following prefixes: cl_ for functions called mostly from cluster, mc_ for those called from fc, and pmsis_ for builtins and shared functions (event kernel for example)
## typedefs must be used for fully opaque type accesses, whereas "struct struct_name" will be used in files where types can be accessed freely.

## For performance sake, compatibility layers, and all small functions that are often called will be declared static inline

## For the sake of better modularity:
###  constants that should only be used inside a given module will be declared with attributes static const
### functions only used locally should also be declared static (and inline if they are small enough)

## Finally, a particular care will be given to not breaking abstractions (e.g. access a private structure member outside of type managing code)