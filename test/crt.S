.section .text 

.global _start

_start:
	xor    %ebp,%ebp
	mov    %esp,%eax
	and    $0xfffffff0,%esp
 	mov    (%eax),%ebx      /* argc */
 	mov    %eax, %ecx
       	add    $0x4,%ecx        /* argv[] */
	mov    %ebx,%edx
	add    $0x2,%edx
	shl    $0x2,%edx
  	add    %eax,%edx        /*  envp[] */
	push   %edx
	push   %ecx
	push   %ebx
	call   main 
	mov %eax, %ebx
	mov $1, %eax
	int $0x80
