		PRESERVE8
		AREA OS_Area, CODE, READONLY, ALIGN=2
		THUMB
		EXPORT SVC_Handler
			
SVC_Table
		IMPORT Yield_Context_Switch
	DCD Yield_Context_Switch		; SVC 0
		
		IMPORT OS_Transfer_SVC
	DCD OS_Transfer_SVC				; SVC 1
		
		IMPORT OS_Sleep_SVC
	DCD OS_Sleep_SVC				; SVC 2
			
SVC_Handler						; This should NOT be called from another ISR
								; There's no point to that, anyway...
								; ...since the whole reason we use SVC is to get around privilege violations
	MRS  r3, PSP				; Get the calling program's stack pointer
	LDR  r0, [r3, #0]			; Get the calling program's value for r0
	LDR  r3, [r3, #24]			; Get topmost value on full descending stack (lr)
	LDRH r3, [r3,#-2]			; Extract last instruction before interrupt (SVC)
    BIC  r3, r3, #0xFF00		; Extract comment field
	LSL  r3, r3, #2				; 4-byte address length
	LDR  r12, =SVC_Table		; Get starting address
	ADD  r3, r3, r12			; Add offset
	LDR  r3, [r3]				; Get function pointer
	PUSH {r1, lr}
	BLX	 r3						; Perform indicated SVC routine
	POP  {r1, lr}
	BX   LR
	
		ALIGN
		END