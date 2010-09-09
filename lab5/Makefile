all:
	arm-elf-gcc -c armboot.c
	arm-elf-gcc -c printf.c
	arm-elf-gcc -c xmodem.c
	arm-elf-gcc -o lab5.elf lab5.S -e _start -nostdlib -Ttext=0x30000000 armboot.o printf.o xmodem.o
	arm-elf-objcopy -O binary lab5.elf lab5.bin
clean:
	rm *.o
	rm lab5.elf
	rm lab5.bin
