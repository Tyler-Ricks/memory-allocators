#include "mat4.h"


MAT4_RESULT mat4_set_init(uint32_t mat4_count, mat4_set* structure) {
	if (mat4_count == 0) { return MAT4_INVALID_INPUT; }

	//mat4_set structure;
	for (int i = 0; i < COL_COUNT; i++) {
		if (frame_s_create(sizeof(float) * ROW_COUNT, mat4_count, &structure->cols[i]) != SLAB_S_SUCCESS) {
			return MAT4_FAILURE;
		}
	}

	return MAT4_SUCCESS;
}


MAT4_RESULT mat4_init_zeroes(mat4* mat, mat4_set* structure) {
	if(mat == NULL || structure == NULL){ return MAT4_INVALID_INPUT; }

	float zeroes[] = {0.0f, 0.0f, 0.0f, 0.0f};

	for (uint32_t i = 0; i < COL_COUNT; i++) {
		Slab_s slab;
		slab.memory_size = COL_COUNT * sizeof(float);
		if (slab_s_alloc(zeroes, &slab, &structure->cols[i]) != SLAB_S_SUCCESS) { return MAT4_FAILURE; }

		mat->cols[i] = slab.memory;
	}

	return MAT4_SUCCESS;
}

MAT4_RESULT mat4_init(float* data, mat4* mat, mat4_set* structure) {
	if(data == NULL || mat == NULL || structure == NULL){ return MAT4_INVALID_INPUT; }

	/*for (uint32_t i = 0; i < MEMBER_COUNT; i += ROW_COUNT) {
		//printf("C[%ld] slab size: %ld, slab count: %ld", i, i, i);
	}*/
	for (uint32_t i = 0; i < ROW_COUNT; i++) {
		Slab_s slab;
		slab.memory_size = COL_COUNT * sizeof(float);
		if (slab_s_alloc((char*)data + (i * ROW_COUNT), &slab, &structure->cols[i]) != SLAB_S_SUCCESS) { return MAT4_FAILURE; }
	}

	return MAT4_SUCCESS;
}

void mat4_set_free(mat4_set* structure) {
	for (int i = 0; i < COL_COUNT; i++) {
		frame_s_free(&structure->cols[i]);
	}
}

void mat4_print_value(mat4 mat) {
	for (int i = 0; i < ROW_COUNT; i++) {
		for (int j = 0; j < COL_COUNT; j++) {
			printf("%f, ", *((float*) ( (char*)mat.cols[j] + (i * sizeof(float)))));
		}
		printf("\n");
		}
	}

void mat4_print_location(mat4 mat) {
	for (int i = 0; i < ROW_COUNT; i++) {
		for (int j = 0; j < COL_COUNT; j++) {
			printf("%p, ", (char*)mat.cols[j] + (i * sizeof(float)));
		}
		printf("\n");
	}
}
