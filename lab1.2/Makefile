all:
	arm-elf-gcc -c ledloop.c
	arm-elf-gcc -o lab1.2.elf lab1.2.S -e _start -nostdlib -Ttext=0x0 ledloop.o 
	arm-elf-objcopy -O binary lab1.2.elf lab1.2.bin
clean:
	rm *.o
	rm lab1.2.elf
	rm lab1.2.bin
