#ifndef SLAB_H
#define SLAB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

// this is (what I think is) a slab allocator. It mallocs a large pool of memory (similar to a pool)
// but divides it into equal-sized slabs. unused slabs make up a linked list, where each unused location
// stores a pointer to another unused location. The last in the list points to NULL.
// because unused slabs store pointers, the default slab size is equal to sizeof(void*) 
// 
// I want to figure out a way to make some functions here more safe. for example, slab_free takes a void*
// as a parameter, and there's not much of a way to chck that the memory you submit to be free is also 
// part of the frame you pass in as a parameter. You could even theoretically pass in a pointer to the
// same frame for both parameters, which could be funny. The same is kind of true for slab_alloc, but 
// I think making that safe is a bit easier, and the memory copied over is based on slab_size
// A big issue is that this not so safe stuff is hard to track down. The allocator doesn't crash 
// immediately, it usually happens when freeing slab_free

// ways to enforce passing in proper memory locations into slab_free:
// 1) | have slab_allocate return a slab struct instead, and have slab_free take a slab pointer as a parameter
//    | instead of a void*. This makes it less user friendly I think, since you would have to go through a slab
//    | struct instead of just the value directly.

typedef struct {
	void* start;					// pointer to the full chunk of memory
	void* available;				// pointer to an available chunk 
	size_t slab_size;				// size of each slab in the frame
	uint32_t slab_count;			// number of slabs in the frame
}Frame;

#define FRAME_ERROR (Frame) { NULL, 0, 0, NULL };

typedef int SLAB_RESULT;
#define SLAB_FAILURE 0
#define SLAB_SUCCESS 1
#define SLAB_INVALID_INPUT 2

//static Slab* slab_create(void* memory);
//static Slab* slab_list_create(void* memory, size_t slab_size, uint32_t slab_count);

SLAB_RESULT frame_create(const size_t slab_size, const uint32_t slab_count, Frame* frame);

void* slab_alloc_raw(Frame* frame);
void* slab_alloc(void* data, Frame* frame);

uint32_t count_available_slabs(Frame* frame);

void slab_free(void* location, Frame* frame);

void frame_free(Frame* frame);

#endif