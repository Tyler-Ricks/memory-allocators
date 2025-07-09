#include "Pool.h"

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

	float* no_space = pool_raw_alloc(sizeof(float), &pool);
	if (no_space == NULL) {
		printf("failed to allocate to a pool!\n");
	}
	else {
		printf("test failed\n");
		pool_print(&pool);
	}

	float y = 20.0f;
	no_space = pool_alloc(&y, sizeof(float), &pool);
	if (no_space == NULL) {
		printf("failed to allocate to a pool!\n");
	}
	else {
		printf("test failed\n");
		pool_print(&pool);
	}

	pool_free(&pool);
}


void run_tests() {
	switch (1) {
	case 1:
		test_pool_create();
		break;
	default:
		printf("no tests\n");
	}
}

int main() {
	run_tests();
}