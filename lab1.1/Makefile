all:
	arm-elf-gcc -c ledloop.c
	arm-elf-gcc -o lab1.1.elf lab1.1.S -e _start -nostdlib -Ttext=0x0 ledloop.o 
	arm-elf-objcopy -O binary lab1.1.elf lab1.1.bin
clean:
	rm *.o
	rm lab1.1.elf
	rm lab1.1.bin
