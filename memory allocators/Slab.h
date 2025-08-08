#ifndef SLAB_H
#define SLAB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

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
//
// Instead of using a slab struct (incorrectly btw, since I keep it stack allocated which is a huge mistake) I
// could just store a pointer to the next available location in each available slab. This only fails if you 
// want slabs that are smaller than the size of a pointer, but I could work around that later
// you may be able to work around small slab sizes by having each slab store an int offset that fits in each 
// slab, but of course this limits how many slabs you can have.
//


//typedef unsigned int uint32_t;


// maybe include a pointer to the frame that owns the slab?
/*
typedef struct {
	void* mem;
	struct Slab* next;
}Slab;
*/

typedef struct {
	void* start;					// pointer to the full chunk of memory
	void* available;				// pointer to an available chunk 
	size_t slab_size;			// size of each slab in the frame
	uint32_t slab_count;		// number of slabs in the frame
	// Slab* available;
}Frame;

#define FRAME_ERROR (Frame) { NULL, 0, 0, NULL };

typedef int SLAB_RESULT;
#define SLAB_FAILURE 0
#define SLAB_SUCCESS 1
#define SLAB_INVALID_INPUT 2

//static Slab* slab_create(void* memory);
//static Slab* slab_list_create(void* memory, size_t slab_size, uint32_t slab_count);

Frame frame_create2(const size_t slab_size, const uint32_t slab_count);
SLAB_RESULT frame_create(const size_t slab_size, const uint32_t slab_count, Frame* frame);

void* slab_alloc_raw(Frame* frame);
void* slab_alloc(void* data, Frame* frame);

uint32_t count_available_slabs(Frame* frame);

void slab_free(void* location, Frame* frame);

void frame_free(Frame* frame);

#endif