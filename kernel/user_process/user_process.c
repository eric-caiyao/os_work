#include "user_process.h" 
#include "tss.h"
#include "memory.h"
#include "../thread/thread.h"
extern put_hex(long long int address);
extern put_char(char c);
extern put_int32(int a);

void* create_page_dir(){
	int* new_page_dir_entry_list = malloc_page(1);
	// copy high 1G from kernel
	int* kernel_dir_entry_list = (int)0xFFFFF000;
	int index = 768;
	for(; index < 1023; index ++){
		*(new_page_dir_entry_list + index) = *(kernel_dir_entry_list + index);
	}

	*(new_page_dir_entry_list + 0) = *(new_page_dir_entry_list + 768); // firsit 

	int new_page_dir_phy_address = (int)virtual_to_physic(new_page_dir_entry_list);
	*(new_page_dir_entry_list + 1023) = 0x1 | 0x2 | 0x3 | new_page_dir_phy_address; // last

	return (void*)new_page_dir_entry_list;
}

void update_tss(PCB* pcb){
	tss_data.ss0 = 0x10;
	tss_data.esp0 = (int)pcb | 0xFFF;
}

// init gdt
void init_gdt(){
	
	// 0: unuse
	segment_desc* null_segment = &gdt[0];
	null_segment->limit_low16 = 0;
	null_segment->base_low16 = 0;
	null_segment->base_mid8 = 0;
	null_segment->attr = 0;
	null_segment->attr2 = 0;
	null_segment->base_high8 = 0;
	// 1: kernel code segment
	segment_desc* kernel_code = &gdt[1];
	kernel_code->limit_low16 = 0xFFFF;
	kernel_code->base_low16 = 0x0;
	kernel_code->base_mid8 = 0x0;
	kernel_code->attr = (0x1 << 7)/*P*/ | (0x00 << 5)/*DPL*/ | (0x1 << 4)/*S*/ | 0x8/*TYPE*/;
	kernel_code->attr2 
		= (0x1 << 7)/*G*/ | (0x1 << 6)/*D/B*/ | (0x0 << 5)/*L*/ | (0x0 << 4)/*AVL*/ | 0xF /*limit mid 4*/;
	kernel_code->base_high8 = 0x0;
	// 2: kernel data segment
	segment_desc* kernel_data = &gdt[2];
	kernel_data->limit_low16 = 0xFFFF;
	kernel_data->base_low16 = 0x0;
	kernel_data->base_mid8 = 0x0;
	kernel_data->attr = (0x1 << 7)/*P*/ | (0x00 << 5)/*DPL*/ | (0x1 << 4)/*S*/ | 0x2/*TYPE*/;
	kernel_data->attr2 
		= (0x1 << 7)/*G*/ | (0x1 << 6)/*D/B*/ | (0x0 << 5)/*L*/ | (0x0 << 4)/*AVL*/ | 0xF /*limit mid 4*/;
	kernel_data->base_high8 = 0x0;
	// 3: kernel video segment
	segment_desc* kernel_video = &gdt[3];
	kernel_video->limit_low16 = 0x7;
	kernel_video->base_low16 = 0x8000;
	kernel_video->base_mid8 = 0xb;
	kernel_video->attr = (0x1 << 7)/*P*/ | (0x00 << 5)/*DPL*/ | (0x1 << 4)/*S*/ | 0x2/*TYPE*/;
	kernel_video->attr2 
		= (0x1 << 7)/*G*/ | (0x1 << 6)/*D/B*/ | (0x0 << 5)/*L*/ | (0x0 << 4)/*AVL*/ | 0x0 /*limit mid 4*/;
	kernel_video->base_high8 = 0x0;
	// 4: user code segment
	segment_desc* user_code = &gdt[4];
	user_code->limit_low16 = 0xFFFF;
	user_code->base_low16 = 0x0;
	user_code->base_mid8 = 0x0;
	user_code->attr = (0x1 << 7)/*P*/ | (0x3 << 5)/*DPL*/ | (0x1 << 4)/*S*/ | 0x8/*TYPE*/;
	user_code->attr2 
		= (0x1 << 7)/*G*/ | (0x1 << 6)/*D/B*/ | (0x0 << 5)/*L*/ | (0x0 << 4)/*AVL*/ | 0xF /*limit mid 4*/;
	user_code->base_high8 = 0x0;

	// 5: user data segment
	segment_desc* user_data = &gdt[5];
	user_data->limit_low16 = 0xFFFF;
	user_data->base_low16 = 0x0;
	user_data->base_mid8 = 0x0;
	user_data->attr = (0x1 << 7)/*P*/ | (0x3 << 5)/*DPL*/ | (0x1 << 4)/*S*/ | 0x2/*TYPE*/;
	user_data->attr2 
		= (0x1 << 7)/*G*/ | (0x1 << 6)/*D/B*/ | (0x0 << 5)/*L*/ | (0x0 << 4)/*AVL*/ | 0xF /*limit mid 4*/;
	user_data->base_high8 = 0x0;

	// 6: tss segment
	segment_desc* tss_segment = &gdt[6];
	tss_segment->limit_low16 = 0xFFFF;
	tss_segment->base_low16 = (short)(((int)(&tss_data)) & 0x0000FFFF);
	tss_segment->base_mid8 = (char)((((int)(&tss_data)) & 0xFF0000) >> 16);
	tss_segment->attr = (0x1 << 7)/*P*/ | (0x00 << 5)/*DPL*/ | (0x0 << 4)/*S*/ | 0x9/*TYPE*/;
	tss_segment->attr2 
		= (0x1 << 7)/*G*/ | (0x1 << 6)/*D/B*/ | (0x0 << 5)/*L*/ | (0x0 << 4)/*AVL*/ | 0xF /*limit mid 4*/;
	tss_segment->base_high8 = (char)((((int)(&tss_data)) & 0xFF000000) >> 24);
	tss_data.ss0 = 0x10;

	segment_desc* user_video = &gdt[7];
	user_video->limit_low16 = 0x7;
	user_video->base_low16 = 0x8000;
	user_video->base_mid8 = 0xb;
	user_video->attr = (0x1 << 7)/*P*/ | (0x3 << 5)/*DPL*/ | (0x1 << 4)/*S*/ | 0x2/*TYPE*/;
	user_video->attr2 
		= (0x1 << 7)/*G*/ | (0x1 << 6)/*D/B*/ | (0x0 << 5)/*L*/ | (0x0 << 4)/*AVL*/ | 0x0 /*limit mid 4*/;
	user_video->base_high8 = 0xc0;

	long long int gdtr_content = 0;
	gdtr_content = gdtr_content | (8 * 8 - 1);
	gdtr_content = gdtr_content | ((long long int)(&gdt)) << 16;

	asm volatile ("lgdt %0"::"m"(gdtr_content));
	asm volatile ("ltr %w0"::"r"(0x30));

}

void user_process_start(thread_func function){
	intr_stack init_intr_stack;
	init_intr_stack.es = 0x2b;
	init_intr_stack.gs = 0x3b;
	init_intr_stack.ds = 0x2b;
	init_intr_stack.eip = function;
	init_intr_stack.cs = 0x23;
	init_intr_stack.eflags = ((3 << 12)/*IOPL*/ | (1 << 1)/*MBS*/ | (1 << 9)/*IF*/);
	malloc_page(1);
	init_intr_stack.esp = malloc_page(1) + 4096;/*user stack*/
	init_intr_stack.ss = 0x2b;
	asm volatile("movl %0, %%esp; pop %%es; pop %%gs; pop %%ds; iret"::"g"(&init_intr_stack):"memory");
	// function();
}

