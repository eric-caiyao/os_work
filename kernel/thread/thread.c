#include "thread.h"
#include "../user_process/user_process.h"
#include "../memory/memory.h"

#include "../interrupt/interrupt.h"
extern void put_char(char char_ascii);

PCB* all_thread_list[10];
int next_index = 0;

PCB* current_pcb(){
	int esp_value = 0;
	asm volatile ("movl %%esp,%0"::"g"(esp_value):"memory");
	PCB* pcb = (PCB*)(esp_value & 0xFFFFF000);
	return pcb;
}

void thread_entry(thread_func* func,void* args){
	PCB* pcb = current_pcb();
	update_page_table(pcb);
	update_tss(pcb);
	intr_enable();
	(*func)(args);
}

void update_page_table(PCB* pcb){
	void* physic_address = virtual_to_physic(pcb->pagedir_addr);
	asm volatile ("movl %0,%%cr3"::"r"(physic_address):"memory");
	// asm volatile ("movl %0,%%cr3"::"r"(0x100000):"memory");
}

void thread_start(thread_func function,void* args){
	PCB* pcb = malloc_page(1);
	pcb->pid = 1;
	pcb->name = '2';
	pcb->stack = (void*)((int)pcb + 4096);
	pcb->stack = pcb->stack - sizeof(thread_stack);
	pcb->priority = 2; 
	pcb->last = 1;
	if(function == user_process_start){
		// 0000 0000 0100 0000 0000 00400000
		pcb->virtual_manage_pool.start_virtual_address = 0x400000;
		pcb->virtual_manage_pool.bitmap.bits = malloc_page(1);
		pcb->virtual_manage_pool.bitmap.bitmap_bit_length = 1024; // limit 1024 page can be assign per process
		pcb->pagedir_addr = create_page_dir();
		pcb->tag = 'U';
	}else{
		pcb->virtual_manage_pool = kernel_virtual_pool;
		pcb->pagedir_addr = 0x100000;
		pcb->tag = 'K';
	}
	thread_stack* stack = (thread_stack*)pcb->stack;
	stack->entry = &thread_entry;
	stack->function = function;
	stack->returnAddress = (void*)1111;
	stack->args = args;
	stack->esi = 0;
	stack->edi = 0;
	stack->edx = 0;
	all_thread_list[next_index ++] = pcb;
	return;
}

void init_main(){
	PCB* main_pcb = current_pcb();
	main_pcb->pid = 0;
	main_pcb->name = '1';
	main_pcb->stack = (thread_stack*)((int)main_pcb + 4096);
	main_pcb->priority = 2; 
	main_pcb->last = 1;
	main_pcb->virtual_manage_pool = kernel_virtual_pool;
	main_pcb->pagedir_addr = 0xFFFFF000;
	main_pcb->tag = 'K';
	all_thread_list[next_index ++] = main_pcb;
	current_run_pcb = main_pcb;
	return;
}
