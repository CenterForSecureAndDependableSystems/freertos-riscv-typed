	.file	"main.c"
	.option nopic
	.attribute arch, "rv32i2p1_m2p0_a2p1_zicsr2p0_zifencei2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.section	.rodata.testFun.str1.4,"aMS",@progbits,1
	.align	2
.LC0:
	.string	"Hello FeeRTOS! Number %d\n"
	.section	.text.testFun,"ax",@progbits
	.align	2
	.globl	testFun
	.type	testFun, @function
testFun:
	# allocate stack space
	addi	sp,sp,-64
	# store constant 4 in argument register a2
	# CHANGE THIS TO SEE RESULTS CHANGE
	li	a2,5
	
	## HERE IS WHERE TESTING STARTS ##


	## Make sure debug value (what we are checking) is in a2 so we can print it
	## HERE IS WHERE TESTING ENDS

	# store address of String in argument register a1
	lla	a1,.LC0
	# put address of buf[] into argument register a0 
	addi	a0,sp,8
	# push old return address
	sw	ra,60(sp)
	# print results
	call	sprintf
	addi	a0,sp,8
	call	vSendString
	lw	ra,60(sp)
	addi	sp,sp,64
	jr	ra
	.size	testFun, .-testFun
	.section	.text.startup.main,"ax",@progbits
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-16
	sw	ra,12(sp)
	lla	a5,freertos_risc_v_trap_handler
 #APP
# 71 "main.c" 1
	csrw mtvec, a5
# 0 "" 2
 #NO_APP
	call	testFun
	lw	ra,12(sp)
	li	a0,0
	addi	sp,sp,16
	jr	ra
	.size	main, .-main
	.section	.text.vApplicationMallocFailedHook,"ax",@progbits
	.align	2
	.globl	vApplicationMallocFailedHook
	.type	vApplicationMallocFailedHook, @function
vApplicationMallocFailedHook:
 #APP
# 89 "main.c" 1
	csrc mstatus, 8
# 0 "" 2
 #NO_APP
.L6:
	j	.L6
	.size	vApplicationMallocFailedHook, .-vApplicationMallocFailedHook
	.section	.text.vApplicationIdleHook,"ax",@progbits
	.align	2
	.globl	vApplicationIdleHook
	.type	vApplicationIdleHook, @function
vApplicationIdleHook:
	ret
	.size	vApplicationIdleHook, .-vApplicationIdleHook
	.section	.text.vApplicationStackOverflowHook,"ax",@progbits
	.align	2
	.globl	vApplicationStackOverflowHook
	.type	vApplicationStackOverflowHook, @function
vApplicationStackOverflowHook:
 #APP
# 120 "main.c" 1
	csrc mstatus, 8
# 0 "" 2
 #NO_APP
.L9:
	j	.L9
	.size	vApplicationStackOverflowHook, .-vApplicationStackOverflowHook
	.section	.text.vApplicationTickHook,"ax",@progbits
	.align	2
	.globl	vApplicationTickHook
	.type	vApplicationTickHook, @function
vApplicationTickHook:
	ret
	.size	vApplicationTickHook, .-vApplicationTickHook
	.section	.text.vAssertCalled,"ax",@progbits
	.align	2
	.globl	vAssertCalled
	.type	vAssertCalled, @function
vAssertCalled:
	addi	sp,sp,-16
	sw	zero,12(sp)
 #APP
# 137 "main.c" 1
	csrc mstatus, 8
# 0 "" 2
 #NO_APP
	li	a5,1
.L12:
	lw	a4,12(sp)
	bne	a4,a5,.L13
	addi	sp,sp,16
	jr	ra
.L13:
 #APP
# 141 "main.c" 1
	NOP
# 0 "" 2
 #NO_APP
	j	.L12
	.size	vAssertCalled, .-vAssertCalled
	.ident	"GCC: (crosstool-NG 1.26.0.120_4d36f27) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
