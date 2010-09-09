all:
	arm-elf-gcc -c armboot.c
	arm-elf-gcc -o lab2.elf lab2.S -e _start -nostdlib -Ttext=0x0 armboot.o 
	arm-elf-objcopy -O binary lab2.elf lab2.bin
clean:
	rm *.o
	rm lab2.elf
	rm lab2.bin
