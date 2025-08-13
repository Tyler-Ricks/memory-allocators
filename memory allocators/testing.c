#include "Pool.h"
#include "Slab.h"
#include "Slab_s.h"
#include "mat4.h"


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

	//Frame frame = frame_create(sizeof(float), 2);
	Frame frame;
	if (frame_create(sizeof(float), 2, &frame) != SLAB_SUCCESS) {
		printf("failed to create a frame! check inputs\n") ;
		exit(1);
	}

	printf("test: %ld \n", frame.slab_count);

	printf("testing count_available_slabs: \n");
	printf("available slabs: %d\n", count_available_slabs(&frame));

	printf("\ntesting raw allocation: \n");
	float* s_a = slab_alloc_raw(&frame);
	*s_a = 1.0f;
	printf("data at a: %f\n", *s_a);
	printf("available slabs: %d\n", count_available_slabs(&frame));


	printf("\ntesting normal allocation: \n");
	float b = 2.0f;
	float* s_b = slab_alloc(&b, &frame);
	printf("data at slab b: %f\n", *s_b);
	printf("available slabs: %d\n", count_available_slabs(&frame));


	printf("\ntesting allocating beyond frame capacity: \n");
	slab_alloc(&b, &frame);

	printf("\ntesting slab_free:\n");
	slab_free(s_a, &frame);
	s_a = NULL;
	printf("available slabs: %d\n", count_available_slabs(&frame));

	printf("allocating a new slab...\n");
	float* s_c = slab_alloc_raw(&frame);
	*s_c = 3.0f;
	printf("data at slab c (formerly slab a): %f\n", *s_c);
	printf("available slabs: %d\n", count_available_slabs(&frame));

	frame_free(&frame);
}

void test_weird_frame() {
	Frame frame;
	if (frame_create(sizeof(double), 5, &frame) != SLAB_SUCCESS) {
		printf("failed to allocate a frame, check you inputs \n");
		exit(1);
	}

	Frame* f1 = slab_alloc_raw(&frame);
	slab_free(&frame, &frame);


	frame_free(&frame);
}


void test_Slab_s() {
	Frame_s frame;
	size_t slab_size = sizeof(double);
	if (frame_s_create(slab_size, 10, &frame) != SLAB_S_SUCCESS) {
		printf("failed to allocate a Frame_s\n");
		
	}

	Slab_s a;
	a.memory_size = slab_size;
	if (slab_s_alloc_raw(&a, &frame) != SLAB_S_SUCCESS) {
		printf("failed to allocate a slab on a Frame!");
		exit(1);
	}
	double* A = (double*)a.memory;
	*A = 5.0;
	printf("allocated a: %lf\n", *A);

	print_void_ptr(frame.start);

	frame_s_free(&frame);
}

#define NUM_THREADS 8
#define SLABS_PER_THREAD 100
#define TOTAL_SLABS (NUM_THREADS * SLABS_PER_THREAD)

Frame_s shared_frame;

int thread_func(void* arg) {
    int thread_id = *(int*)arg;

    for (int i = 0; i < SLABS_PER_THREAD; ++i) {
        Slab_s slab;
        slab.memory_size = sizeof(double);

        if (slab_s_alloc_raw(&slab, &shared_frame) != SLAB_S_SUCCESS) {
            printf("Thread %d: Failed to allocate slab\n", thread_id);
            continue;
        }

        double* ptr = (double*)slab.memory;
        *ptr = (double)(thread_id * 1000 + i);  // Write something unique

        if (slab_s_free(&slab, &shared_frame) != SLAB_S_SUCCESS) {
            printf("Thread %d: Failed to free slab\n", thread_id);
        }
    }

    return 0;
}

void test_slab_multithreaded() {
    if (frame_s_create(sizeof(double), TOTAL_SLABS, &shared_frame) != SLAB_S_SUCCESS) {
        printf("Failed to create frame\n");
        return;
    }

    thrd_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        if (thrd_create(&threads[i], thread_func, &thread_ids[i]) != thrd_success) {
            printf("Failed to create thread %d\n", i);
            return;
        }
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        thrd_join(threads[i], NULL);
    }

    // After all threads finish, all slabs should be available again
    uint32_t remaining = count_s_available_slabs(&shared_frame);
    printf("Available slabs after test: %u (expected: %u)\n", remaining, TOTAL_SLABS);

    if (remaining != TOTAL_SLABS) {
        printf("Memory leak or corruption detected.\n");
    } else {
        printf("All slabs successfully reused.\n");
    }

    frame_s_free(&shared_frame);
}

#define MAT4_COUNT 5
void test_mat4() {
	mat4_set set;
	if (mat4_set_init(MAT4_COUNT, &set) != MAT4_SUCCESS) {
		printf("failed to initialize a mat4 structure\n");
		return;
	}
	for (int i = 0; i < COL_COUNT; i++) { // COME BACK
		printf("number of matrices allocated: %d\n", set.cols[i].slab_count);
	}

	mat4 A;
	if (mat4_init_zeroes(&A, &set) != MAT4_SUCCESS) {
		printf("failed to initialize a matrix!\n");
		return;
	}

	// print the elements of A
	printf("\ntest elements of A:\n");
	mat4_print_value(A);

	printf("\ntest mat4_init:\n");
	float dataB[] = {1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f};
	
	mat4 B;
	if (mat4_init(dataB, &B, &set) != MAT4_SUCCESS) {
		printf("failed to initialize matrix B!\n");
		return;
	}
	printf("\ntesting values of B:\n");
	mat4_print_value(B);

	mat4_set_free(&set);
}


void run_tests() {
	switch (6) {
	case 1:
		test_pool_create();
		break;
	case 2:
		test_slab_create();
		break;
	case 3:
		test_weird_frame();
		break;
	case 4:
		test_Slab_s();
		break;
	case 5:
		test_slab_multithreaded();
		break;
	case 6:
		test_mat4();
		break;
	default:
		printf("no tests\n");
	}
}

int main() {
	run_tests();
}