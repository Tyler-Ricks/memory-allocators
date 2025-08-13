#ifndef MAT4_H
#define MAT4_H

#include <stdio.h>
#include <stdint.h>

#include "Slab_s.h"

// so the idea here is to store a bunch of mat4s in a mat4_set struct, using a Struct of Arrays layout
// for performance. the mat4_set struct contains an array of slab allocators, one for each column in 
// a mat4. This way, the memory is stored contiguously, which reduces cache misses for certain matrix
// operations
// 
// Things to consider:
// it may be better to implement mat4 as an array of Slab_s, so that we can free the memory from a
// mat4_set. Otherwise, why not just use a pool allocator?
//
// working with SoA instead of AoS is obviously more complicated. I think reaching a point where we 
// can work with matrices as we would in regular math would be a good abstraction... It seems possible
// without sacrificing the efficiency that implementing SoA is supposed to.



typedef int MAT4_RESULT;
#define MAT4_FAILURE 0
#define MAT4_SUCCESS 1
#define MAT4_INVALID_INPUT 2
#define COL_COUNT 4
#define ROW_COUNT 4
#define MEMBER_COUNT (COL_COUNT * ROW_COUNT)

#define CMO

#ifdef CMO
typedef struct {
	Frame_s cols[COL_COUNT];
}mat4_set;

// contains an array of pointers to the start of each column in a mat4_set
typedef struct {
	 Slab_s cols[COL_COUNT];
}mat4;

#else

typedef struct {

}mat4_set;

#endif

MAT4_RESULT mat4_set_init(uint32_t mat4_count, mat4_set* structure);

MAT4_RESULT mat4_init_zeroes(mat4* mat, mat4_set* structure);
MAT4_RESULT mat4_init(float* data, mat4* mat, mat4_set* structure);

void mat4_set_free(mat4_set* structure);
//void mat4_set_free(mat4_set* structure);

void mat4_print_value(mat4 mat);
void mat4_print_location(mat4 mat); 

#endif