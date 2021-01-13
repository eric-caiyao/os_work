#include "../memory/memory.h"

#ifndef H_THREAD
#define H_THREAD
typedef void thread_func();

typedef struct {
	void* esi;
	void* edi;
	void* edx;
	void* ebp;
	void (*entry)(thread_func* func);
	void* returnAddress;
	void (*function)(void* args);
	void* args;
}thread_stack;

typedef struct{
	void* es;
	void* gs;
	void* ds;
	void* eip;
	void* cs;
	void* eflags;
	void* esp;
	void* ss;
}intr_stack;

typedef struct {
	int pid;
	char name;
	int priority;
	int last;
	void* stack;
	virtual_pool virtual_manage_pool;
	void* pagedir_addr;
	char tag; // U: user K: kernel
}PCB;

void* virtual_to_physic(void* virtual_address);

PCB* current_pcb();

void thread_start(thread_func function,void* args);

void update_page_table();

PCB* current_run_pcb;

#endif