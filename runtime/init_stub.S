.section .text

stub:
	call   jmp_test
	xorl   %eax, %eax
test:
	mov    (%ebx), %esi	# Le o endereco do init localizado em (%ebx)
	test   %esi, %esi		# Verifica se o valor eh zero (fim dos enderecos)
	je     jmp_start			# Se zero salta para jmp_start
	push   %ebx
	push   $0x50564e45       # Os valores que serao colocados na pilha na verdade sao TAGS (ENVP, ARGV, ARGC) e NAO DEVEM SER MUDADOS
	push   $0x56475241       # A funcao setInitProgArgs procura esses tags e os substitui pelos enderecos de env, argv e argc
	push   $0x43475241
	xorl   %ebx, %ebx
	call   *%esi			# Chama a init da lib
	pop    %ebx
	add    $0x4, %ebx		# Incrementa o contador para o proximo endereco a ser chamado (se existir)
	jmp    test
jmp_start:
	add    $0x4, %ebx		# Incrementa o contador para apontar para o endereco de entrada do binario
	mov    (%ebx), %esi
	xorl   %ebx, %ebx
	jmp    *%esi			# Inicia aplicacao
jmp_test:
	mov    (%esp), %ebx	# Recupera o EIP da segunda instrucao
	mov    -4(%ebx), %eax 	# Recupera o numero de bytes usados no salto da instrucao call
	add    %eax, %ebx		# Calcula a posicao onde esta localizado o primeiro endereco dos inits
	add    $12, %ebx             # Calculo: pos = EIP da seg instrucao + numero de bytes ate o jmp_test + numero de bytes da jmp_test
	ret
