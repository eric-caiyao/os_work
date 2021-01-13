#ifndef H_TSS
#define H_TSS

struct tss{
	void* pre_tss_address;	
	void* esp0;
	void* ss0;
	void* esp1;
	void* ss1;
	void* esp2;
	void* ss2;
	void* cr3;
	void* eip;
	void* eflags;
	void* eax;
	void* ecx;
	void* edx;
	void* ebx;
	void* esp;
	void* ebp;
	void* esi;
	void* edi;
	void* es;
	void* cs;
	void* ss;
	void* ds;
	void* fs;
	void* gs;
	void* ldt_selector;
	void* iobitmap_offset;
};

#endif