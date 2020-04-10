splicekernel: forksplice.o splice.o
	gcc -o $@ $^


%.o: %.c
	gcc -c $(CFLAGS) $< -o $@

%.o: %.asm
	nasm $< -o $@ -f elf64

clean:
	rm -rf *.o
	rm -rf splicekernel
