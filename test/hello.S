.section	.text

.global _start

_start:		
	mov	$14, %edx
	mov	$msg, %ecx	
	mov	$1, %ebx
	mov	$4, %eax
	int	$0x80       

	mov	$1, %eax	
	xor	%ebx, %ebx
	int	$0x80

.section	.data
msg:
	.ascii "hello, world!\12\0"
