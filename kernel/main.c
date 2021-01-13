#include "thread/thread.h"
#include "user_process/user_process.h"
#include "interrupt/interrupt.h"
#include "device/time.h"
#include "memory/memory.h"
#include "../lib/sys_call.h"

extern void put_char(char char_ascii);
extern void put_str(char* char_array);
extern void put_int32(long long int x);
extern void put_hex(long long int address);
extern void put_hex_new(int value);
extern void put_32(int value);

void test_thread();
void test_user_process();

int main(void){
	// put_32(0x12345678);
	// int i = 0;
	// for(; i < 16; i ++){
	// 	put_hex_new(i);
	// 	put_char('\n');
	// }
	// put_char('\n');
	// put_str("OS start...\n");

	// put_str("idt init start \n");
	idt_init();
	put_str("idt init end \n");

	asm volatile("sti");
	put_str("interrupt enable \n");

	put_str("timer init start \n");
	timer_init();
	put_str("timer init end \n");

	put_str("memory init start \n");
	long long int total_mem = (long long int)*((int*)0xa1f);
	memory_pool_init(total_mem);
	put_str("memory init end \n");

	put_str("main thread init start \n");
	init_main();
	put_str("main thread init end \n");

	put_str("gdt init start \n");
	init_gdt();
	put_str("gdt init end \n");

	put_str("start one user process start \n");
	thread_start(user_process_start,(void*)test_user_process);
	put_str("start one user process end \n");
	while(1){
		put_str("main thread \n");
	}
}

void test_user_process(){
	while(1){
		write("hello world! \n");
	}
	return;
}

void test_thread(void* args){
	while(1){
		put_char('H');
	}
	return;
}

