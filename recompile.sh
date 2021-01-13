rm -rf hd60M.img
rm -rf kernel.bin

../bochs_gdb/bin/bximage -hd -mode="flat" -size=60 -q hd60M.img
nasm -g -o mbr.bin code/boot/mbr.S
nasm -g -o loader.bin code/boot/loader.S
nasm -g -f elf32 -o print.o code/kernel/print.S
nasm -g -f elf32 -o interrupt_handler.o code/kernel/interrupt/interrupt_handler.S
nasm -g -f elf32 -o switch_to.o /data/work/work/code/kernel/thread/switch_to.S
gcc -g -c -o interrupt.o code/kernel/interrupt/interrupt.c
gcc -g -c -o main.o code/kernel/main.c
gcc -g -c -o timer.o code/kernel/device/timer.c
gcc -g -c -o bitmap.o code/kernel/bitmap.c
gcc -g -c -o memory.o code/kernel/memory/memory.c
gcc -g -c -o thread.o code/kernel/thread/thread.c
gcc -g -c -o user_process.o code/kernel/user_process/user_process.c
gcc -g -c -o sys_call.o code/lib/sys_call.c

ld -g -Ttext 0xc0001500 -e main -o kernel.bin main.o interrupt_handler.o interrupt.o print.o timer.o  memory.o thread.o switch_to.o user_process.o sys_call.o

dd if=mbr.bin of=hd60M.img bs=512 conv=notrunc count=1
dd if=loader.bin of=hd60M.img bs=512 count=2 conv=notrunc seek=2
dd if=kernel.bin of=hd60M.img bs=512 count=200 conv=notrunc seek=9

rm -rf loader.bin 
rm -rf mbr.bin
rm -rf interrupt.o
rm -rf main.o
rm -rf timer.o
rm -rf bitmap.o
rm -rf memory.o
rm -rf thread.o
rm -rf print.o
rm -rf switch_to.o
rm -rf interrupt_handler.o
rm -rf user_process.o
rm -rf sys_call.o
