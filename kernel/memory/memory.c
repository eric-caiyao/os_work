#include "../bitmap.c"
#include "memory.h"
#include "../thread/thread.h"

extern put_hex(int address);
extern put_char(char c);
extern put_str(char* str);
void memory_pool_init(long long int total_mem){
	// kernel physical pool init
	kernel_phy_pool.start_phy_address 
		= 0x100000 
		+ (
			1/*page entry table*/ 
			+ 1/*0 and 768 point to one page*/ 
			+ 254/*768 ~ 1022*/
		) * 4096;
	kernel_phy_pool.bitmap.bitmap_bit_length 
		= ((total_mem - (long long int)kernel_phy_pool.start_phy_address) / 2) * 8
		  / 4096;
	kernel_phy_pool.bitmap.bits = 0xc009a000; // 1 page bitmap => 128M memory

	// user physical pool init
	user_phy_pool.start_phy_address 
		= kernel_phy_pool.start_phy_address 
		+ (total_mem - (long long int)kernel_phy_pool.start_phy_address) / 2;
	user_phy_pool.bitmap.bitmap_bit_length = kernel_phy_pool.bitmap.bitmap_bit_length;
	user_phy_pool.bitmap.bits = 0xc009b000;

	// kernel virtual pool init
	kernel_virtual_pool.start_virtual_address = 0xc0100000;
	kernel_virtual_pool.bitmap.bitmap_bit_length = kernel_phy_pool.bitmap.bitmap_bit_length;
	kernel_virtual_pool.bitmap.bits = 0xc009c000;
}

void* get_virtual(virtual_pool pool, int page_count){
	int result = check_and_set(pool.bitmap,page_count);
	return (void*)(pool.start_virtual_address + result * 4096);
}


void* get_one_phypage(int pool_type){
	void* result_addr = -1;
	if(pool_type == 0){
		int result = check_and_set(kernel_phy_pool.bitmap,1);
		result_addr = (void*)(kernel_phy_pool.start_phy_address + (result * 4096));
	}else{
		int result = check_and_set(user_phy_pool.bitmap,1);
		result_addr = (void*)(user_phy_pool.start_phy_address + (result * 4096));
	}
	return result_addr;
}
/**
 * get virtual_address page directory entry
 */
void* pde(void* virtual_address){
	return 0xFFFFF000 | (((((int)virtual_address) & 0xFFC00000) >> 22) * 4/*notice: must multi 4*/);
}
/**
 * get page table entry
 */
void* pte(void* virtual_address){
	return 0xffc00000 | (((int)virtual_address) & 0xffc00000) >> 10 | (((int)virtual_address) & 0x003ff000) >> 10;
}
/**
 * build virtual to physical address map in page table
 */
void build_map(void* virtual_address, void* physical_address){
	void* pde_address = pde(virtual_address);
	if(
		((*(int*)pde_address) & 0x1) ^ 0x00000001
	){
		int* _pde_address = pde_address;
		int pagetable_addr = (int)get_one_phypage(0);
		*_pde_address =  pagetable_addr | 0x7;
	}
	void* pte_address = pte(virtual_address);
	//if(pte_address & 0x00000001 == 0){
		int* _pte_address = (int*)pte_address;
		*_pte_address = (int)physical_address | 0x7;
	//}
}
/**
* note: user process pcb memory is assign from kernel virtual pool
*/
void* malloc_page(int page_count){
	PCB* pcb = current_pcb();
	int virtual_start_address = (int)get_virtual(pcb->virtual_manage_pool,page_count);
	if(virtual_start_address == -1){
		return -1;
	}
	int i = 0;
	for(; i < page_count; i ++){
		if(pcb->tag == 'K'){
			void* tmp_phy_address = get_one_phypage(0); 
			build_map(virtual_start_address + i * 4 * 1024,tmp_phy_address);
		}else{
			void* tmp_phy_address = get_one_phypage(1);
			build_map(virtual_start_address + i * 4 * 1024,tmp_phy_address);
		}
	}
	return (void*)virtual_start_address;
}


void* virtual_to_physic(void* virtual_address){

	int physical_address = 0x0;
	physical_address = physical_address | (int)0xFFC00000;

	int origin_pde_num = (int)virtual_address & (int)0xFFC00000;
	int shift_to_mid10 = origin_pde_num >> 10;
	physical_address = physical_address | shift_to_mid10;

	int origin_pte_num = (int)virtual_address & (int)0x3FF000;
	int shift_to_low10 = origin_pte_num >> 10;
	physical_address = physical_address | shift_to_low10;

	int raw_pte_data = *((int*)physical_address);
	int result_high20 = raw_pte_data & (int)0xFFFFF000;

	int page_offset = (int)virtual_address & (int)0xFFF;

	int result = result_high20 | page_offset;

	return (void*)result;

}
