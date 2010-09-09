
all:
	arm-elf-gcc -o lab1.elf lab1.S -e _start -nostdlib -Ttext=0x0  
	arm-elf-objcopy -O binary lab1.elf lab1.bin
clean:
	rm *.o
	rm lab1.elf
	rm lab1.bin
