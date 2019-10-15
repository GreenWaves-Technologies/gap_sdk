## Using vectors

### Introduction

This section is derived from the GCC documentation.

The first step in using these extensions is to provide the necessary data types. This should be done using an appropriate typedef:

~~~~c
typedef short int           v2s __attribute__ ((vector_size (4))); // A vector of 2 shorts
typedef signed char         v4s __attribute__ ((vector_size (4))); // A vector of 4 chars
typedef unsigned short int  v2u __attribute__ ((vector_size (4))); // A vector of 2 unsigned shorts
typedef unsigned char       v4u __attribute__ ((vector_size (4))); // A vector of 4 unsigned chars
~~~~

The `int` type specifies the base type, while the attribute specifies the vector size for the variable, measured in bytes.  For example, the declaration above causes the compiler to set the mode for the `v4s` type to be 4 bytes wide and divided into `char` sized units.  For a 32-bit `int` this means a vector of 4 units of 1 byte, and the corresponding mode of `foo` is V4QI in GCC internal parlance.

The `vector_size` attribute is only applicable to integral and float scalars, although arrays, pointers, and function return values are allowed in conjunction with this construct. Only sizes that are a power of two are currently allowed.

All the basic integer types can be used as base types, both as signed and as unsigned: `char`, `short`, `int`, `long`, `long long`.  In addition, `float` and `double` can be used to build floating-point vector types.

Specifying a combination that is not valid for the current architecture causes GCC to synthesize the instructions using a narrower mode.  For example, if you specify a variable of type `V4SI` and your architecture does not allow for this specific SIMD type, GCC produces code that uses 4 `SIs`.

The types defined in this manner can be used with a subset of normal C operations.  Currently, GCC allows using the following operators on these types: `+, -, *, /, unary minus, ^, |, &, ~, %'.

The operations behave like C++ `valarrays`.  Addition is defined as the addition of the corresponding elements of the operands.  For
 example, in the code below, each of the 4 elements in A is added to the corresponding 4 elements in B and the resulting vector is stored in C.

~~~~c
     typedef char v4s __attribute__ ((vector_size (4)));

     v4qi a, b, c;

     c = a + b;
~~~~

Subtraction, multiplication, division, and the logical operations operate in a similar manner.  Likewise, the result of using the unary minus or complement operators on a vector type is a vector whose elements are the negative or complemented values of the corresponding elements in the operand.

It is possible to use shifting operators `<<`, `>>` on integer-type vectors. The operation is defined as following: `{a0, a1, ..., an} >> {b0, b1, ..., bn} == {a0 >> b0, a1 >> b1, ..., an >> bn}`. Vector operands must have the same number of elements.

For convenience, it is allowed to use a binary vector operation where one operand is a scalar. In that case the compiler transforms the scalar operand into a vector where each element is the scalar from the operation. The transformation happens only if the scalar could be safely converted to the vector-element type.  Consider the following code.

~~~~c
     typedef char v4s __attribute__ ((vector_size (4)));

     v4s a, b, c;
     int I;

     a = b + 1;    /* a = b + {1,1,1,1}; */
     a = 2 * b;    /* a = {2,2,2,2} * b; */

     a = I + a;    /* Error, cannot convert int to char. */
~~~~

Vectors can be subscripted as if the vector were an array with the same number of elements and base type.  Out of bound accesses invoke undefined behavior at run time.  Warnings for out of bound accesses for vector subscription can be enabled with `-Warray-bounds`. For example you could sum up all the elements of a given vector as shown in the following example:

~~~~c
     typedef char v4s __attribute__ ((vector_size (4)));

     v4s A, B[10];
     int R1, R2;
     int index;

     R1 = A[0] + A[1] + A[2] + A[3];
     R2 = B[index][0] + B[index][1] + B[index][2] + B[index][3];
~~~~

Vector subscripts are endian independent (GCC code generation generates different code for big and little endian). GAP8 is little endian.

Vector comparison is supported with standard comparison operators: `==, !=, <, <=, >, >=`. Comparison operands can be vector expressions of integer-type or real-type. Comparison between integer-type vectors and real-type vectors are not supported.  The result of the comparison is a vector of the same width and number of elements as the comparison operands with a signed integral element type.

Vectors are compared element-wise producing 0 when comparison is false and -1 (constant of the appropriate type where all bits are set) otherwise. Consider the following example.

~~~~c
     typedef char v4s __attribute__ ((vector_size (4)));

     v4s a = {1,2,3,4};
     v4s b = {3,2,1,4};
     v4s c;

     c = a >  b;     /* The result would be {0, 0,-1, 0}  */
     c = a == b;     /* The result would be {0,-1, 0,-1}  */
~~~~

The following example illustrates how to properly compare vectors, in particular how to infer a test that need to be satisfied on a all vector elements and a test that need to be satisfy on at least one vector element.

~~~~c
     typedef short v2s __attribute__ ((vector_size (4)));
     
     v2s R;
     int a;
     
     void foo(v2s V1, v2s V2)
     
     {
             R = (V1 > V2);          /* The result is a vector */
     
             if (V1 > V2)            /* Error: used vector type where scalar is required */
                     a = 1;
     
             if ((int)(V1 > V2))     /* Correct, equivalent to ((int)(V1>V2) != 0) */
                     a = 2;          /* Test will pass if at least one of the 2 comparisons */
                                     /* is true */
     
             if ((int)(V1 > V2)==-1) /* Correct,  Test will pass if the 2 comparisons are true */
                     a = 3;          
     }
~~~~

In C++, the ternary operator `?:` is available. `a?b:c`, where `b` and `c` are vectors of the same type and `a` is an integer vector with the same number of elements of the same size as `b` and `c`, computes all three arguments and creates a vector `{a[0]?b[0]:c[0], a[1]?b[1]:c[1], ...}`.  Note that unlike in OpenCL, `a` is thus interpreted as `a != 0` and not `a < 0`.  As in the case of binary operations, this syntax is also accepted when one of `b` or `c` is a scalar that is then transformed into a vector. If both `b` and `c` are scalars and the type of `true?b:c` has the same size as the element type of `a`, then `b` and `c` are converted to a vector type whose elements have this type and with the same number of elements as `a`.

Vector shuffling is available using functions `__builtin_shuffle (vec, mask)` and `__builtin_shuffle (vec0, vec1, mask)`.  Both functions construct a permutation of elements from one or two vectors and return a vector of the same type as the input vector(s).  The MASK is an integral vector with the same width (W) and element count (N) as the output vector.

The elements of the input vectors are numbered in memory ordering of VEC0 beginning at 0 and VEC1 beginning at N.  The elements of MASK are considered modulo N in the single-operand case and modulo 2*N in the two-operand case.

Consider the following example,

~~~~c
     typedef char v4s __attribute__ ((vector_size (4)));

     v4qi a = {1,2,3,4};
     v4qi b = {5,6,7,8};
     v4qi mask1 = {0,1,1,3};
     v4qi mask2 = {0,4,2,5};
     v4qi res;

     res = __builtin_shuffle (a, mask1);       /* res is {1,2,2,4}  */
     res = __builtin_shuffle (a, b, mask2);    /* res is {1,5,3,6}  */
~~~~

Note that `__builtin_shuffle` is intentionally semantically compatible with the OpenCL `shuffle` and `shuffle2` functions.

You can declare variables and use them in function calls and returns, as well as in assignments and some casts.  You can specify a vector type as a return type for a function.  Vector types can also be used as function arguments.  It is possible to cast from one vector type to another, provided they are of the same size (in fact, you can also cast vectors to and from other datatypes of the same size).

The previous example is rewritten using a user function to initialize the masks:

~~~~c
     typedef char v4s __attribute__ ((vector_size (4)));

     void   __attribute__ ((noinline)) set_mask(v4s *M, v4s val)

     {
             *M = val;
     }

     void main()

     {
          v4s a = {1,2,3,4};
          v4s b = {5,6,7,8};
          v4s mask1;
          v4s mask2;
          v4s res;

          set_mask(&mask1, (v4s) {0,1,1,3});       /* Note that the immediate aggregate has
							  to be explictly casted to v4s */
          set_mask(&mask2, (v4s) {0,4,2,5});

          res = __builtin_shuffle (a, mask1);       /* res is {1,2,2,4}  */
          printf("Res={%d,%d,%d,%d}\n", res[0], res[1], res[2], res[3]);

          res = __builtin_shuffle (a, b, mask2);    /* res is {1,5,3,6}  */
          printf("Res={%d,%d,%d,%d}\n", res[0], res[1], res[2], res[3]);
     }
~~~~

You cannot operate between vectors of different lengths or different signedness without a cast.

### Handling alignment (auto vectorization)

Even though GAP8 has full support for unaligned accesses GCC will generate better code if you provide an alignment hint:

~~~~c
     void foo(char * __restrict__ a, char * __restrict__ b)
     
     {
             int i;
     
             char *x = __builtin_assume_aligned(a, 4);
             char *y = __builtin_assume_aligned(b, 4);
     
             for (i = 0; i < SIZE; i++) {
                     x[i] = x[i] + y[i + 1];
             }
     }
~~~~

If you don't, this loop will be vectorized but a complex peeling scheme will be applied ... Performance is reasonable but code size is very bad.

Note also the `__restrict__` qualifier, it is essential otherwise you have to assume that a and b can point to the same location making the vector transformation illegal.

### Code example using vector notation

Searching a given char (Pat) in a string (A):
  
Here is another example using plain vectors and bit manipulation function. It returns the index of the first matching instance of Pat in A (compared to a sequential search the execution time is divided by 3). This code will also compile as is on any processor supported by GCC.

~~~~c
     int  __attribute__ ((noinline)) Search_Pattern_Scalar(v4s *A, int Size, char Pat)
     
     {
             v4s R;
             int i;
     
             if (Size <= 0) return -1;
             for (i=0; i<Size; i++) {
                     if (((int) (R=A[i]==(v4qi){Pat,Pat,Pat,Pat})) != 0)
                             return (4*i + (__builtin_ffs((int) (A[i]==Pat))>>3));
             }
     
             return -1;
     }
~~~~

Salient features are:

+ if (((int) (R=A[i]==(v4s){Pat,Pat,Pat,Pat})) != 0)  
  Test if Pat matches on at least one subscript of A[i] and store comparison result into R (a vector)
+ return (4*i + (__builtin_ffs((int) (A[i]==Pat))>>3))  
  Returns the byte position. Since R elements are either 0 or -1 we want to find the first 1 starting from LSB if endian is little
+ if (Size <= 0) return -1  
  A trick to let GCC knows that Size in loop is > 0, code is more compact

###	Auto vectorization code examples

GCC activates auto vectorization when -O3 is passed. You should be aware that at this level of optimization code-bloat might be an issue since loop unrolling, loop peeling and loop realignment lead to a lot of code replication.
As a rule of thumb using vector notation as explained in section 3 combined with -O2 is often a better compromise for an embedded target. Beside full target portability is not impacted since these GCC vector extensions are supported on any existing GCC target.

Performing sum of product:

~~~~c
     void foo(char * __restrict__ c1, char * __restrict__ c2, char * __restrict__ c3)
     
     {
             int i;
     
             char * cc1 = __builtin_assume_aligned(c1, 4);
             char * cc2 = __builtin_assume_aligned(c2, 4);
             char * cc3 = __builtin_assume_aligned(c3, 4);
     
             for (i=0; i<SIZE; i++) // SIZE=128
                     cc1[i] = cc1[i] + (cc2[i] + cc3[i]);
     }
~~~~

GCC for Gap8 with -O3 will produce the following fully vectorized loop

~~~~assembly
foo:
        lp.setupi       x1,32,(.L5)      # loop setup, lc+le set
.L2:
        lw      a5,0(a0)
        p.lw    a4,4(a1!)       	    # load post inc
        p.lw    a6,4(a2!)       	    # load post inc
        pv.add.b        a5,a4,a5         # Vect Op Vect
        pv.add.b        a5,a5,a6         # Vect Op Vect
.L5:
        p.sw    a5,4(a0!)       # store post inc
        ret
~~~~

Performing min/max:

~~~~c
     void foo1(char * __restrict__ a, char * __restrict__ b, char * __restrict__ c)
     {
             int i, j;
     
             char *x = __builtin_assume_aligned(a, 4);
             char *y = __builtin_assume_aligned(b, 4);
             char *z = __builtin_assume_aligned(c, 4);
     
             for (i = 0; i < SIZE; i++) { // SIZE=128
                     z[i] = (y[i] > x[i]) ? x[i] : y[i];
             }
     }
~~~~

GCC for Gap8 with -O3 will produce the following fully vectorized loop

~~~~assembly
foo1:
        lp.setupi       x1,32,(.L9)      # loop setup, lc+le set
.L7:
        p.lw    a3,4(a1!)       # load post inc
        p.lw    a5,4(a0!)       # load post inc
        pv.minu.b       a5,a5,a3         # VectU Op Vect
.L9:
        p.sw    a5,4(a2!)       # store post inc
        ret
~~~~

The last example illustrates the limits of auto vectorization. The following code performs a simple 2D matrix to 2D matrix sum. GCC partially manages to auto vectorize it: the inner loop is fully vectorized but the outer loop is a total mess with lot of code to try to realign the data through partial loop peeling. The exec time will be reasonably acceptable but code-bloat is enormous.

~~~~c
     void foo3(char * __restrict__ a, char * __restrict__ b)
     {
             int i, j;
     
             char *x = __builtin_assume_aligned(a, 4);
             char *y = __builtin_assume_aligned(b, 4);
     
             for (j = 0; j < SIZE; j++) {
                     for (i = 0; i < SIZE; i++) {
                             x[i + j * SIZE] += y[i + j * SIZE];
                     }
             }
     }
~~~~

The following code does the same thing but using direct vector notation. SIZE/4
is to account for the fact that a vector contains 4 elements.

~~~~c
     void foo4(v4qi * __restrict__ a, v4qi * __restrict__ b)
     {
             int i, j;
     
             for (j = 0; j < SIZE; j++) {
                     for (i = 0; i < SIZE/4; i++) {
                             a[i + j * SIZE/4] += b[i + j * SIZE/4];
                     }
             }
     }
~~~~

Performance is slightly better than auto vectorized version but code size is 10x smaller! Here is code generated for the entire function.

~~~~assembly
foo4:
        add     a0,a0,128
        lp.setupi       x0,128,(.L46)    # loop setup, lc+le set
.L39:
        add     a3,a0,-128
        mv      a2,a1
        lp.setupi       x1,32,(.L45)     # loop setup, lc+le set
.L40:
        lw      a5,0(a3)
        p.lw    a6,4(a2!)       	# load post inc
        pv.add.b        a5,a5,a6     # Vect Op Vect
.L45:
        p.sw    a5,4(a3!)       # store post inc
        add     a1,a1,128
.L46:
        add     a0,a0,128
        ret
~~~~
