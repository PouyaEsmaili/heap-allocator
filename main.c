#include <stdio.h>
#include <stdlib.h>
#include "mem.h"

int random_allocate(void *ptrs[], int *sizes, int N, int test_size) {
    int flag = 0;
    for (int i = 0; i < N; i++) {
        test_size = rand() % 3;
        if (test_size == 0) {
            sizes[i] = rand() % 249 + 8;
        } else if (test_size == 1) {
            if (rand() % 2 == 0) {
                sizes[i] = 64;
            } else {
                sizes[i] = 64 * 1024;
            }
        } else {
            sizes[i] = 1 << (rand() % 13 + 3);
        }
        ptrs[i] = Mem_Alloc(sizes[i]);
    }
    return flag;
}

int random_test(void *ptrs[], int *sizes, int N, int test_pointer) {
    int flag = 0;
    for (int i = 0; i < N; i++) {
        ((int *) ptrs[i])[0] = i;

        if (ptrs[i] == NULL) {
            flag = 1;
        } else if (Mem_IsValid(ptrs[i] + (test_pointer * rand() % sizes[i])) == 0) {
            flag = 2;
        } else if (Mem_GetSize(ptrs[i] + (test_pointer * rand() % sizes[i])) != sizes[i]) {
            printf("Size: %d, Expexted: %d, Index: %d\n", Mem_GetSize(ptrs[i]), sizes[i], i);
            flag = 3;
        }
    }
    for (int i = 0; i < N; i++) {
        if (((int *) ptrs[i])[0] != i) {
            printf("ERROR: %d != %d\n", ((int *) ptrs[0])[0], i);
            flag = 4;
        }
    }
    return flag;
}

int random_free(void *ptrs[], int *sizes, int N, int test_pointer) {
    int flag = 0;
    for (int i = 0; i < N; i++) {
        if (Mem_Free(ptrs[i] + (test_pointer * rand() % sizes[i])) != 0) {
            flag = 5;
        }
    }
    return flag;
}

int random_post_free_test(void *ptrs[], int *sizes, int N, int test_pointer) {
    int flag = 0;
    for (int i = 0; i < N; i++) {
        if (Mem_IsValid(ptrs[i] + (test_pointer * rand() % sizes[i]))) {
            flag = 6;
        }
    }
    return flag;
}

int main() {
    int test_size = 0; // 0 is '8 to 256 bytes', 1 is '64 to 64k bytes', 2 is 'power of 2 bytes'
    int test_order = 0; // 0 is 'create N blocks free N blocks create N blocks free N blocks',
    // 1 is 'create N/2 blocks free N/2 blocks use N/2 blocks free N/2 blocks'
    int test_pointer = 1; // 0 is 'only use the main ptr', 1 is 'use the main ptr or ptr inside the allocated memory'
    int N = 1000;
    int total_size = N * 64 * 1024;

    Mem_Init(total_size);

    void *ptrs[N];
    int sizes[N];
    int allocate_flag, test_flag, free_flag, post_free_flag, total_allocate_flag;

    if (test_order == 0) {
        allocate_flag = random_allocate(ptrs, sizes, N, test_size);
        test_flag = random_test(ptrs, sizes, N, test_pointer);
        free_flag = random_free(ptrs, sizes, N, test_pointer);
        post_free_flag = random_post_free_test(ptrs, sizes, N, test_pointer);
    } else if (test_order == 1) {
        allocate_flag = random_allocate(ptrs, sizes, N, test_size);
        free_flag = random_free(ptrs, sizes, N / 2, test_pointer);
        post_free_flag = random_post_free_test(ptrs, sizes, N / 2, test_pointer);
        test_flag = random_test(ptrs + N / 2, sizes + N / 2, N - N / 2, test_pointer);
        free_flag += random_free(ptrs + N / 2, sizes + N / 2, N - N / 2, test_pointer);
        post_free_flag += random_post_free_test(ptrs + N / 2, sizes + N / 2, N - N / 2, test_pointer);
    }
    total_allocate_flag = Mem_Alloc(total_size - 50) == NULL;
    if (allocate_flag + test_flag + free_flag + post_free_flag + total_allocate_flag != 0) {
        printf("Failed\n");
        printf("allocate_flag: %d, test_flag: %d, free_flag: %d, post_free_flag: %d, total_allocate_flag: %d\n",
               allocate_flag, test_flag, free_flag, post_free_flag, total_allocate_flag);
        exit(1);
    } else {
        printf("Passed\n");
    }
}