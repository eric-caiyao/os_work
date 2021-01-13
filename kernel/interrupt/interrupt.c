#include "../thread/thread.h" 
#include "../user_process/user_process.h"
#include "../../lib/sys_call.h"

struct intr_desc {
	unsigned short int handler_low16; // 16 bit
	unsigned short int selector; // 16 bit
	unsigned char ncount; // 8 bit
	unsigned char attr; // 8 bit
	unsigned short int handler_high16; // 8 bit
};
extern void put_int();
extern void * interrupt_handle_entry;
extern void* system_call_handle_entry;
extern void put_int32(unsigned int x);
extern void put_str(char* str);
extern void switch_to(void* from , void* to);
extern PCB* all_thread_list[10];

struct intr_desc idt[81]; // interrupt desc table. first 32 interrupt is used by system

void c_version_handle(char vectorNo){
	put_char('A');
	put_char('\n');
	current_run_pcb->last --;
	if(current_run_pcb->last > 0){
		return;
	}
	int i = 0;
	for(; i < 10; i ++){
		if((int)all_thread_list[i] != 0){
			if(all_thread_list[i]->last > 0){
				current_run_pcb->last = current_run_pcb->priority;
				PCB* old_pcb = current_run_pcb;
				current_run_pcb = all_thread_list[i];
				update_page_table(current_run_pcb);
				update_tss(current_run_pcb);
				switch_to(
					&(old_pcb->stack),
					current_run_pcb->stack
				);
				break;
			}
		}
	}
}

void idt_init(){
	// system retain 0 ~ 31 interrupt vector no
	int i = 0;
	for(; i < 81; i ++){
		idt[i].handler_low16 = 0;
		idt[i].selector = 0;
		idt[i].ncount = 0;
		idt[i].attr = 0;
		idt[i].handler_high16 = 0;

		// idt[i].handler_low16 = ((unsigned int)&interrupt_handle_entry_0) & 0x0000FFFF;
		// idt[i].selector = 0x8;
		// idt[i].ncount = 0;
		// idt[i].attr = 0x8E ;
		// idt[i].handler_high16 = ((unsigned int)&interrupt_handle_entry_0) >> 16;
	}
	// time interrupt
	idt[32].handler_low16 = ((unsigned int)&interrupt_handle_entry) & 0x0000FFFF;
	idt[32].selector = 0x8;
	idt[32].ncount = 0;
	idt[32].attr = 0x8E ;
	idt[32].handler_high16 = ((unsigned int)&interrupt_handle_entry) >> 16;
	// system call 
	idt[80].handler_low16 = ((unsigned int)&system_call_handle_entry) & 0x0000FFFF;
	idt[80].selector = 0x8;
	idt[80].ncount = 0;
	idt[80].attr = 0xEE ;
	idt[80].handler_high16 = ((unsigned int)&system_call_handle_entry) >> 16;

	asm volatile("outb %b0,%w1"::"a"(0x11),"Nd"(0x20));
	asm volatile("outb %b0,%w1"::"a"(0x20),"Nd"(0x21));
	asm volatile("outb %b0,%w1"::"a"(0x04),"Nd"(0x21));
	asm volatile("outb %b0,%w1"::"a"(0x01),"Nd"(0x21));
	asm volatile("outb %b0,%w1"::"a"(0x11),"Nd"(0xa0));
	asm volatile("outb %b0,%w1"::"a"(0x28),"Nd"(0xa1));
	asm volatile("outb %b0,%w1"::"a"(0x02),"Nd"(0xa1));
	asm volatile("outb %b0,%w1"::"a"(0x01),"Nd"(0xa1));
	asm volatile("outb %b0,%w1"::"a"(0xfe),"Nd"(0x21));
	asm volatile("outb %b0,%w1"::"a"(0xff),"Nd"(0xa1));
	unsigned long long int idt_operand = ((sizeof(idt) - 1) | ((unsigned long long int)((unsigned int)idt << 16 )));
	asm volatile("lidt %0" :: "m"(idt_operand));
	
	init_sys_call_handle();
}


// int intr_set_status(int status){
// 	if(status == 0){
// 		intr_disable();
// 	}else{
// 		intr_enable();
// 	}
// }
int intr_enable(){

		asm volatile("sti");
// 	if(intr_get_status() == 0){
// 		return 0;
// 	}else{
// 		return 1;
// 	}
}
// int intr_disable(){
// 	if(intr_get_status() == 1){
// 		asm volatile("cli":::"memory");
// 		return 1;
// 	}else{
// 		return 0;
// 	}
// }
// int intr_get_status(){
// 	unsigned long int eflags = 0;
// 	asm volatile("pushfl; popl %0)":"=g"(eflags));
// 	return 0x00000200 & eflags ? 1 : 0;
// }

