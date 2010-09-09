all:
	arm-elf-gcc -c armboot.c
	arm-elf-gcc -c xmodem.c
	arm-elf-gcc -o lab6.elf lab6.S -e _start -nostdlib -Ttext=0x30000000 armboot.o xmodem.o
	arm-elf-objcopy -O binary lab6.elf lab6.bin
clean:
	rm *.o
	rm lab6.elf
	rm lab6.bin
