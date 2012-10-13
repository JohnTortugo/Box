.section .text

stub:
	call   jmp_test
	xorl   %eax, %eax
test:
	mov    (%ebx), %esi
	test   %esi, %esi
	je     jmp_start
	push   %ebx
	push   $0x50564e45
	push   $0x56475241
	push   $0x43475241
	xorl   %ebx, %ebx
	call   *%esi
	pop    %ebx
	add    $0x4, %ebx
	jmp    test
jmp_start:
	add    $0x4, %ebx
	mov    (%ebx), %esi
	xorl   %ebx, %ebx
	jmp    *%esi
jmp_test:
	mov    (%esp), %ebx
	mov    -4(%ebx), %eax
	add    %eax, %ebx
	add    $12, %ebx
	ret
