extern put_str(char* str);
void timer_init(){
	put_str("start init timer\n");
	asm volatile("outb %b0,%w1"::"a"(0 << 6 | 3 << 4 | 2 << 1),"Nd"(0x43));
	asm volatile("outb %b0,%w1"::"a"((char)(1193180 / 100)),"Nd"(0x40));
	asm volatile("outb %b0,%w1"::"a"((char)((1193180 / 100) >> 8)),"Nd"(0x40));
	put_str("init timer end\n");
}
