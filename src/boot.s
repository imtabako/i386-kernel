.set	KERNEL_CODE,  0x8
.set	KERNEL_DATA,  0x10
.set	USER_CODE,    0x18
.set	USER_DATA,    0x20

.set	PAGE_ALGIN ,  1<<0
.set	MEMORY_INFO,  1<<1
.set	HEADER_MAGIC, 0x1BADB002
.set	HEADER_FLAGS, (PAGE_ALGIN | MEMORY_INFO)
.set	CHECKSUM,    -(HEADER_MAGIC+HEADER_FLAGS)

.set	KERNEL_BASE,	0xC0000000
.set	KERNEL_PAGE,	KERNEL_BASE>>22
.set	KERNEL_STACK,	0x4000

.section .bss
stack_down:
.align	16
.space	KERNEL_STACK
stack_top:

.align	0x1000
.global	pg_directory
pg_directory:
.skip	0x1000
_first_table:
.skip	0x1000


.section .multiboot
.align	4
.long	HEADER_MAGIC
.long	HEADER_FLAGS
.long	CHECKSUM

.section .text
.global	_start
.type	_start,	@function
.global	loader
.set	loader, _start  - KERNEL_BASE
_start:
	# Because we want kernel to think that it is located above 3Gb
	# (to get access to virtual memory from 0 to 1Mb and above)
	# we will be redusing 3Gb from all adresses until enabling paging
	# mechanism.

	# Identity mapping
	# 0 -- (END OF KERNEL PAGE)
	mov	$(_first_table - KERNEL_BASE), %ecx
	xor	%edi, %edi
1:
	mov	%edi, %edx	# index
	shl	$12, %edx	# multiply by 4096
	or	$3, %edx	# flags
	mov	%edx, (%ecx, %edi, 4)

	mov	$(_ekern), %edx
	shr	$12, %edx
	and	$0x3FF, %edx

	inc	%edi
	cmp	%edx, %edi
	jb	1b

	# Enable paging mechanism
	mov	$(pg_directory - KERNEL_BASE), %ecx
	movl	$(_first_table - KERNEL_BASE), (%ecx)
	orl	$3, (%ecx)

	mov	$(KERNEL_PAGE), %edx
	movl	$(_first_table - KERNEL_BASE), (%ecx, %edx, 4)
	orl	$3, (%ecx, %edx, 4)

	mov	%ecx, %cr3
	mov	%cr0, %ecx
	or	$0x80000000, %ecx
	mov	%ecx, %cr0

	lea	(higher_half), %ecx
	jmp	%ecx

higher_half:
	# Unmap first page, we do not need it anymore
	mov	$(pg_directory), %ecx
	movl	$0, (%ecx)

	mov	%cr3, %ecx
	mov	%ecx, %cr3
# END

	# Set up the stack
	mov	$stack_top, %esp

	add	$(KERNEL_BASE), %ebx
	push	%ebx
	push	%eax
	call	cmain
	add	$8, %esp

.halt:
	cli
	hlt
2:
	# Jump to an endless loop if cpu somehow continues execution
	jmp	2b

.size	_start, . - _start

.global	get_cr3
.type	get_cr3, @function
get_cr3:
	mov	%cr3, %eax

	ret
.size get_cr3, . - get_cr3

.global	load_gdt
.type	load_gdt, @function
load_gdt:
	mov	0x4(%esp), %eax
	lgdt	(%eax)
	ljmp	$(KERNEL_CODE), $(.Lsegm)

.Lsegm:
	mov	$(KERNEL_DATA), %eax
	mov	%eax, %ss
	mov	%eax, %ds
	mov	%eax, %es
	mov	%eax, %fs
	mov	%eax, %gs

	ret
.size load_gdt, . - load_gdt

