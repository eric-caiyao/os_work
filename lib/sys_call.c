#include "sys_call.h"
#include "../kernel/thread/thread.h"
extern void put_str(char* str);

int _systemcall0(int num){
	int retvalue;
	asm volatile ("int $80":"=a"(retvalue):"a"(num):"memory");
	return retvalue;
}
int _systemcall1(int num, int arg1){
	int retvalue;
	asm volatile ("int $80":"=a"(retvalue):"a"(num),"b"(arg1):"memory");
}
void init_sys_call_handle(){
	sys_call_handle_table[0] = get_pid;
	sys_call_handle_table[1] = write_str_to_console;
}
/* kernel use start */
int get_pid(){
	PCB* currentpcb = current_pcb();
	return currentpcb->pid;
}
int write_str_to_console(char* str){
	put_str(str);
	return 0;// todo: return str length
}
/* kernel use start end */

/* user lib use start */
int getpid(){
	return _systemcall0(0x0);
}
int write(char* str){
	return _systemcall1(1,str);
}
/* user lib use end */
