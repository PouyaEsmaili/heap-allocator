#ifndef __mem_h__
#define __mem_h__

// routines
int Mem_Init(int sizeOfRegion);
void *Mem_Alloc(int size);
int Mem_Free(void *ptr);
int Mem_IsValid(void *ptr);
int Mem_GetSize(void *ptr);

#endif // __mem_h__