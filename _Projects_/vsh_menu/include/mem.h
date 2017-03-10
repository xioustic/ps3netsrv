#ifndef __MEM_H__
#define __MEM_H__


#define MB(x)      ((x)*0x100000UL)


int32_t create_heap(int32_t size);
void destroy_heap(void);
void *mem_alloc(uint32_t size);
int32_t mem_free(uint32_t size);
void reset_heap(void);

#endif // __MEM_H__
