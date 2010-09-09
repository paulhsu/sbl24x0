all:
	arm-elf-gcc -c armboot.c
	arm-elf-gcc -o lab3.elf lab3.S -e _start -nostdlib -Ttext=0x30000000 armboot.o 
	arm-elf-objcopy -O binary lab3.elf lab3.bin
clean:
	rm *.o
	rm lab3.elf
	rm lab3.bin
