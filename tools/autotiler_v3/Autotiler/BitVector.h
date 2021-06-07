#ifndef _BIT_VECTOR_H_
#define _BIT_VECTOR_H_

#define BIT_S(Vect, Pos)        ((Vect)[(Pos)>>5] = ((Vect)[(Pos)>>5] |  (1<<((Pos)&31))))
#define BIT_C(Vect, Pos)        ((Vect)[(Pos)>>5] = ((Vect)[(Pos)>>5] & ~(1<<((Pos)&31))))
#define BIT_V(Vect, Pos)        (((Vect)[(Pos)>>5] &  (1<<((Pos)&31)))!=0)

typedef unsigned int BitVector;

BitVector *BitVectorCreate(
        unsigned int N
        );

void BitVectorClear(
        BitVector *V,
        unsigned int N
        );

void BitVectorSet(
        BitVector *V,
        unsigned int N
        );

BitVector *BitVectorCopy(
        BitVector *To,
        BitVector *From,
        unsigned int N
        );

#endif

