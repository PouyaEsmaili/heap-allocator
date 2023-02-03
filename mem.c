#include "mem.h"
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>


typedef struct block_header_t {
    int size;
    int actual_size;
    struct block_header_t *next;
    struct block_header_t *prev;
} block_header_t;

block_header_t *free_list = NULL;
block_header_t *used_list = NULL;

int round_up_size_of_region(int sizeOfRegion) {
    return sizeOfRegion;
}

int Mem_Init(int sizeOfRegion) {
    if (sizeOfRegion <= 0) {
        return -1;
    }
    sizeOfRegion = round_up_size_of_region(sizeOfRegion);

    if (free_list != NULL) {
        return -1;
    }

    free_list = mmap(NULL, sizeOfRegion, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (free_list == MAP_FAILED) {
        return -1;
    }

    free_list->size = sizeOfRegion - sizeof(block_header_t);
    free_list->actual_size = sizeOfRegion;
    free_list->next = NULL;
    free_list->prev = NULL;

    return 0;
}

block_header_t *find_free_block(int size) {
    block_header_t *best_fit = NULL;
    block_header_t *current = free_list;
    while (current != NULL) {
        if (current->size >= size) {
            if (best_fit == NULL || current->size < best_fit->size) {
                best_fit = current;
            }
        }
        current = current->next;
    }
    return best_fit;
}

void split_block(block_header_t *block, int size) {
    if (block->size - size - sizeof(block_header_t) < 8) {
        block->size = block->size;
        return;
    }
    block_header_t *new_block = (block_header_t *) ((void *) block + sizeof(block_header_t) + size);
    new_block->size = block->size - size - sizeof(block_header_t);
    new_block->actual_size = block->actual_size - size - sizeof(block_header_t);
    new_block->next = block->next;
    new_block->prev = block;
    block->size = size;
    if (size == 1)
        printf("block size: %d\n", block->size);
    block->actual_size = size + sizeof(block_header_t);
    block->next = new_block;
}

void assign_block(block_header_t *block) {
    if (block->prev != NULL) {
        block->prev->next = block->next;
    } else {
        free_list = block->next;
    }
    if (block->next != NULL) {
        block->next->prev = block->prev;
    }
    block->next = used_list;
    block->prev = NULL;
    if (used_list != NULL) {
        used_list->prev = block;
    }
    used_list = block;
}

void *Mem_Alloc(int size) {
    if (size <= 0) {
        return NULL;
    }

    if (free_list == NULL) {
        return NULL;
    }

    block_header_t *block = find_free_block(size);
    if (block == NULL) {
        return NULL;
    }
    split_block(block, size);
    assign_block(block);
    return (void *) block + sizeof(block_header_t);
}

block_header_t *find_block(void *ptr) {
    block_header_t *current = used_list;
    while (current != NULL) {
        if (ptr >= (void *) current + sizeof(block_header_t) &&
            ptr < (void *) current + sizeof(block_header_t) + current->size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void put_in_free_list(block_header_t *block) {
    block_header_t *current = free_list;
    if (current == NULL) {
        free_list = block;
        block->next = NULL;
        block->prev = NULL;
        return;
    }
    while (current != NULL) {
        if (block < current && (current->prev == NULL || block > current->prev)) {
            if (block->prev == NULL && block->next == NULL) {
                used_list = NULL;
            } else {
                if (block->prev != NULL) {
                    block->prev->next = block->next;
                } else {
                    used_list = block->next;
                }
                if (block->next != NULL) {
                    block->next->prev = block->prev;
                }
            }

            block->next = current;
            block->prev = current->prev;
            if (current->prev != NULL) {
                current->prev->next = block;
            } else {
                free_list = block;
            }
            current->prev = block;
            return;
        }
        current = current->next;
    }
}

void merge_blocks() {
    block_header_t *current = free_list;
    while (current != NULL) {
        if (current->next != NULL &&
            (void *) current + current->actual_size == (void *) current->next) {
            current->size = current->actual_size + current->next->actual_size - sizeof(block_header_t);
            current->actual_size = current->size + sizeof(block_header_t);
            current->next = current->next->next;
            if (current->next != NULL) {
                current->next->prev = current;
            }
        } else {
            current = current->next;
        }
    }
}

int Mem_Free(void *ptr) {
    if (ptr == NULL) {
        return -1;
    }

    block_header_t *block = find_block(ptr);
    if (block == NULL) {
        return -1;
    }
    put_in_free_list(block);
    merge_blocks();
    return 0;
}

int Mem_IsValid(void *ptr) {
    if (ptr == NULL) {
        return 0;
    }

    block_header_t *block = find_block(ptr);
    if (block == NULL) {
        return 0;
    }
    return 1;
}

int Mem_GetSize(void *ptr) {
    if (ptr == NULL) {
        return -1;
    }

    block_header_t *block = find_block(ptr);
    if (block == NULL) {
        return -1;
    }
    return block->size;
}
