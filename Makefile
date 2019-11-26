intel_brightness: asm_syscall.S intel_brightness.c
	gcc -s -O2 -nostdlib -o $@ $^
