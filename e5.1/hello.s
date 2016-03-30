
./hello.ko:     file format elf32-i386-freebsd

Disassembly of section .text:

0000045c <hello>:
 45c:	55                   	push   %ebp
 45d:	89 e5                	mov    %esp,%ebp
 45f:	53                   	push   %ebx
 460:	bb 09 00 00 00       	mov    $0x9,%ebx
 465:	8d 76 00             	lea    0x0(%esi),%esi
 468:	68 cb 04 00 00       	push   $0x4cb
			469: R_386_RELATIVE	*ABS*
 46d:	e8 fc ff ff ff       	call   46e <hello+0x12>
			46e: R_386_PC32	printf
 472:	4b                   	dec    %ebx
 473:	58                   	pop    %eax
 474:	79 f2                	jns    468 <hello+0xc>
 476:	31 c0                	xor    %eax,%eax
 478:	8b 5d fc             	mov    0xfffffffc(%ebp),%ebx
 47b:	c9                   	leave  
 47c:	c3                   	ret    
 47d:	8d 76 00             	lea    0x0(%esi),%esi

00000480 <load>:
 480:	55                   	push   %ebp
 481:	89 e5                	mov    %esp,%ebp
 483:	53                   	push   %ebx
 484:	8b 45 0c             	mov    0xc(%ebp),%eax
 487:	31 db                	xor    %ebx,%ebx
 489:	85 c0                	test   %eax,%eax
 48b:	74 0f                	je     49c <load+0x1c>
 48d:	48                   	dec    %eax
 48e:	74 28                	je     4b8 <load+0x38>
 490:	bb 2d 00 00 00       	mov    $0x2d,%ebx
 495:	89 d8                	mov    %ebx,%eax
 497:	8b 5d fc             	mov    0xfffffffc(%ebp),%ebx
 49a:	c9                   	leave  
 49b:	c3                   	ret    
 49c:	ff 35 74 15 00 00    	pushl  0x1574
			49e: R_386_RELATIVE	*ABS*
 4a2:	68 e2 04 00 00       	push   $0x4e2
			4a3: R_386_RELATIVE	*ABS*
 4a7:	e8 fc ff ff ff       	call   4a8 <load+0x28>
			4a8: R_386_PC32	printf
 4ac:	5a                   	pop    %edx
 4ad:	89 d8                	mov    %ebx,%eax
 4af:	59                   	pop    %ecx
 4b0:	8b 5d fc             	mov    0xfffffffc(%ebp),%ebx
 4b3:	c9                   	leave  
 4b4:	c3                   	ret    
 4b5:	8d 76 00             	lea    0x0(%esi),%esi
 4b8:	ff 35 74 15 00 00    	pushl  0x1574
			4ba: R_386_RELATIVE	*ABS*
 4be:	68 02 05 00 00       	push   $0x502
			4bf: R_386_RELATIVE	*ABS*
 4c3:	eb e2                	jmp    4a7 <load+0x27>
