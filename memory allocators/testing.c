#include "Pool.h"
#include "Slab.h"

void test_pool_create() {
	Pool pool = pool_create(sizeof(float) + sizeof(int));

	// test pool_create
	pool_print(&pool);


	// test pool_raw_alloc when there is capacity
	float* p_a = pool_raw_alloc(sizeof(float), &pool);
	*p_a = 33.0f;

	printf("p_a = %f\n", *p_a);
	pool_print(&pool);


	// test pool_alloc when there is capacity
	int x = 20;
	int* p_b = pool_alloc(&x, sizeof(int), &pool);

	printf("p_b = %d\n", *p_b);
	pool_print(&pool);


	// test both allocations when there is no capacity

	float* p_no_space = pool_raw_alloc(sizeof(float), &pool);
	*p_no_space = 123.0f;
	printf("*no_space = %f\n", *p_no_space);
	pool_print(&pool);


	float y = 20.0f;
	float* p_z = pool_alloc(&y, sizeof(float), &pool);
	printf("y = %f\n", *p_z);
	pool_print(&pool);


	// test pool_free
	pool_free(&pool);
}

void test_slab_create() {
	Frame* frame = frame_create(sizeof(float), 4);
	printf("test: %ld \n", frame->slab_count);
}


void run_tests() {
	switch (2) {
	case 1:
		test_pool_create();
		break;
	case 2:
		test_slab_create();
		break;
	default:
		printf("no tests\n");
	}
}

int main() {
	run_tests();
}