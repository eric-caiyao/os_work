#ifndef H_SYSCALL
#define H_SYSCALL

void* sys_call_handle_table[10];

int get_pid();

int getpid();

void init_sys_call_handle();

int write(char* str);

int write_str_to_console(char* str);

#endif