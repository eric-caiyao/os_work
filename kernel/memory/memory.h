
#include "../bitmap.h" 

#ifndef H_MEMORY
#define H_MEMORY
typedef struct{
	bitmap bitmap;
	void* start_phy_address;
}phy_pool;
typedef struct{
	bitmap bitmap;
	void* start_virtual_address;
}virtual_pool;

// physical pool for kernel and user
phy_pool kernel_phy_pool, user_phy_pool;
// virtual pool for kernel
virtual_pool kernel_virtual_pool;


void* get_one_phypage(int pool_type);

void* malloc_page(int page_count);

void memory_pool_init(long long int total_mem);

void* virtual_to_physic(void* virtual_address);

#endif