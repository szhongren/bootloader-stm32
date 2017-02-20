.text
	.syntax unified
	.thumb
; these functions are the most unsafe things ever.
	.global	jumpto
	.type	jumpto, %function
jumpto:
	; basically goto address given
	mov pc, r0
	.global loadStackGo
	.type	loadStackGo, %function
loadStackGo:
	; put first arg in stack pointer and second arg in program counter
	; never do this in any other case
	mov sp, r0
	mov pc, r1