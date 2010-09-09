all:
	arm-elf-gcc -c armboot.c
	arm-elf-gcc -o lab4.elf lab4.S -e _start -nostdlib -Ttext=0x30000000 armboot.o 
	arm-elf-objcopy -O binary lab4.elf lab4.bin
clean:
	rm *.o
	rm lab4.elf
	rm lab4.bin
