#ifndef H_USER
#define H_USER
#include "tss.h"
#include "../thread/thread.h"
void* create_page_dir();


typedef struct{
	short limit_low16;
	short base_low16;
	char base_mid8;
	char attr;
	char attr2;
	char base_high8;
}segment_desc;

void init_gdt();

void user_process_start(thread_func function);

segment_desc gdt[8];

static struct tss tss_data;

void update_tss(PCB* pcb);
#endif