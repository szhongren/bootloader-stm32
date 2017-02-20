.text
	.syntax unified
	.thumb
	.global	jumpto
	.type	jumpto, %function
jumpto:
	mov pc, r0
	.global loadStackGo
	.type	loadStackGo, %function
loadStackGo:
	mov sp, r0
	mov pc, r1