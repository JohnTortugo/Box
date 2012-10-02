section	.text
    global _start

_start:		

	mov	edx,len	
	mov	ecx,msg	
	mov	ebx,1	
	mov	eax,4	
	int	0x80       ; sys_write

	mov	eax,1	
	int	0x80       ; sys_exit	

section	.data

msg	db	'Hello, world!',0xa	
len	equ	$ - msg			

