#ifndef SLAB_H
#define SLAB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// this is (what I think is) a slab allocator. It mallocs a large pool of memory (similar to a pool)
// but divides it into equal-sized slabs. It maintians a linked list of unused slabs. When a slab is
// used, it is removed from the linked list. When a slab is "freed" it is added back on to the linked list
//
// it may also be worth considering maintaining a linked list of memory that is currently in use, in order to 
// handle dangling pointers upon freeing the whole frame
//
// maybe have slab_alloc return an actual slab instead of a pointer to memory? slab_free takes a void* as a 
// parameter without checking that it is the start of a slab or anything, so it could be unsafe. 
// alternatively, do the linked list of slabs in use, so it can be found and removed from there (this is slow)


typedef unsigned int uint32_t;


// maybe include a pointer to the frame that owns the slab?
typedef struct {
	void* mem;
	void* next;
}Slab;

typedef struct {
	void* start;					// pointer to the full chunk of memory
	const size_t slab_size;			// size of each slab in the frame
	const uint32_t slab_count;		// number of slabs in the frame
	Slab* available;				// pointer to an available chunk 
}Frame;

#define FRAME_ERROR (Frame) { NULL, 0, 0, NULL };

static Slab* slab_create(void* memory);
static Slab* slab_list_create(void* memory, size_t slab_size, uint32_t slab_count);

Frame frame_create(const size_t slab_size, const uint32_t slab_count);

void* slab_alloc_raw(Frame* frame);
void* slab_alloc(void* data, Frame* frame);

void slab_free(void* location, Frame* frame);

void frame_free(Frame* frame);

#endif