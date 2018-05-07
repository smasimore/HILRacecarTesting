NVIC_ST_CTRL_R EQU 0xE000E010
        
        PRESERVE8

        AREA OS_Data, DATA, ALIGN=2
        EXPORT CS_Ignore
        EXPORT Systick_Calls
        EXPORT Current_Thread
            
CS_Ignore         DCD 0            ; ignore the next interrupt-driven context switch
Systick_Calls     DCD 0            ; total # of times the systick handler has run
Current_Thread    DCD 0            ; pointer to currently executing thread


        AREA OS_Code, CODE, READONLY, ALIGN=2
        THUMB
        IMPORT ReadyThreads
        IMPORT priorityOccupied
        IMPORT SleepThreads
        IMPORT InactiveThreads
        IMPORT OSAux_Wake
        IMPORT HardFault_Handler
        IMPORT StartCritical
        IMPORT EndCritical
        IMPORT getHighestPriority

;**************Pack_Context***************
; Description: Saves state to TCB of currently active thread, in anticipation of changing the active thread.
; Inputs : None
; Outputs: None

Pack_Context
    PUSH  {r0, lr}
    
    LDR   r0, =Current_Thread
    LDR   r0, [r0]                ; get pointer to current task's TCB
    
    MRS   r1, PSP                ; get process stack pointer
    STMFD r1!, {r4-r11}            ; push r4-r11 to the process stack
    STR   r1, [r0]                ; store decremented SP to task SP
    
    POP   {r0, lr}
    BX    lr
    

;**************Unpack_Context***************
; Description: Restores the state of a new active thread.
; Inputs : None
; Outputs: None

Unpack_Context
    PUSH  {r0, lr}
    BL    getHighestPriority    ; returns in r0
    LSL   r0, r0, #2            ; align to 4-word address length
    
    LDR   r1, =ReadyThreads        ; pointer to array of linked lists
    ADD   r0, r1, r0            ; index into array of ll pointers
    LDR   r0, [r0]                ; get pointer to (new) current task's TCB
    
    LDR   r1, [r0]                ; get saved process stack pointer
    LDMFD r1!, {r4-r11}            ; restore r4-r11 from the TCB stack
    MSR   PSP, r1                ; change the process stack pointer to the new task's
    
    LDR   r1, =Current_Thread
    STR   r0, [r1]
    
    POP   {r0, lr}
    BX    lr


;**************Context_Switch***************
; Description: Changes active process to next entry in active linked list.
; Inputs : None
; Outputs: None

        EXPORT Yield_Context_Switch

Yield_Context_Switch            ; called by OS_Suspend
    CPSID I
    LDR   r0, =CS_Ignore        
    MOV   r1, #1
    STR   r1, [r0]                ; set CS_Ignore
    B     Context_Switch        ; take context switch always
    
Int_Context_Switch                ; called by SysTick_Handler
    CPSID I
    LDR r0, =CS_Ignore
    LDR r1, [r0]
    CMP r1, #0                    ; test CS_Ignore
    BEQ Context_Switch            ; take context switch only if zero
    MOV r1, #0
    STR r1, [r0]                ; else clear CS_Ignore
    CPSIE I                        ; safe to do naively - will never be called from within a critical section
    BX lr                        ; don't take context switch
    
Context_Switch
    PUSH  {r0, lr}
    BL    Pack_Context
    
    LDR   r0, =Current_Thread    ; get priority of previous thread
    LDR   r0, [r0]
    LDRB  r0, [r0, #17]
    LSL   r0, r0, #2
    
    LDR   r1, =ReadyThreads        ; pointer to array of linked lists
    ADD   r0, r1, r0            ; index into array of ll pointers
    LDR   r1, [r0]                ; get pointer to old task's TCB
    
    LDR   r1, [r1, #8]            ; get pointer to next TCB in list
    STR   r1, [r0]                ; rotate list    - should we make this rotate the old list instead?
    
    BL    Unpack_Context
    POP   {r0, lr}
    CPSIE I                        ; safe to do naively - will never be called from within a critical section
    BX lr                        


;**************SysTick_Handler***************
; Description: Interrupt delineating time slices.
;     Calls context switches and handles scheduling.
; Inputs : None
; Outputs: None

        EXPORT SysTick_Handler

SysTick_Handler
    LDR  r0, =Systick_Calls        ; increment
    LDR  r1, [r0]
    ADD  r1, r1, #1
    STR  r1, [r0]
    PUSH {r0,lr}                ; save lr
    BL   OSAux_Wake                ; remove threads from the wake list
    BL   Int_Context_Switch        ; switch to next task
    POP  {r0,lr}
    BX   lr
    

;**************OS_Transfer_SVC***************
; Transfers the currently active TCB to the specified linked list
;    (currently a very early partial implementation of blocking, which also handles OS_Kill() calls)
; Inputs : r0 holds a pointer to the linked list
; Outputs: None

; For this and following SVC ASM wrappers:
; The ASM portion is necessary to ensure r4-r11 are preserved
; (Pack_Context and Unpack_Context violate AAPCS calling convention)

; TODO make this and other set CS_Ignore flag(?)

    EXPORT OS_Transfer_SVC
    IMPORT OS_Transfer_SVC_C        ;as much as possible is handled in C
        
OS_Transfer_SVC
    CPSID I
    PUSH  {r0,lr}
    BL    Pack_Context
    POP   {r0, lr}
    PUSH  {r0, lr}
    BL    OS_Transfer_SVC_C
    BL    Unpack_Context
    POP   {r0, lr}
    CPSIE I
    BX    lr
    

;**************OS_Sleep_SVC***************
; Transfers the currently active TCB to the ordered sleep list
; Inputs : r0 holds the number of time slices for which to sleep
; Outputs: None

    EXPORT OS_Sleep_SVC
    IMPORT OS_Sleep_SVC_C        ;as much as possible is handled in C
        
OS_Sleep_SVC
    CPSID I
    PUSH  {r0,lr}
    BL    Pack_Context
    POP   {r0, lr}
    PUSH  {r0, lr}
    BL    OS_Sleep_SVC_C
    BL    Unpack_Context
    POP   {r0, lr}
    CPSIE I
    BX    lr
    

;**************StartOS***************
; Description: Enables SysTick, protection, and begins first task.
;     Causes a hard fault if no tasks have been initialized.
; Inputs : None
; Outputs: None

        EXPORT StartOS

StartOS
    CPSID I
    MRS   r0, CONTROL            ; read CONTROL register
    ORR   r0, r0, #2            ; enable ASP bit to use PSP
    MSR   CONTROL, r0            ; write modified CONTROL
    ISB   SY                    ; flush pipeline
    
    LDR   r0, =NVIC_ST_CTRL_R    ; get address of NVIC_ST_CTRL_R
    MOV   r1, #7
    STR   r1, [r0]                ; enable SysTick with interrupts
    
    LDR   r0, =priorityOccupied
    LDR   r0, [r0]
    CMP   r0, #0                ; ensure that at least one thread exists
    BEQ   HardFault_Handler        ; if not - this is unrecoverable - this should probably go somewhere else, though.
    
    BL    getHighestPriority    ; returns in r0
    LSL   r0, r0, #2            ; align to 4-word address length
    LDR   r1, =ReadyThreads        ; pointer to array of linked lists
    ADD   r0, r1, r0            ; index into array of ll pointers
    LDR   r0, [r0]                ; get pointer to first task's TCB
    
    LDR   r1, =Current_Thread
    STR   r0, [r1]
    
    LDR   sp, [r0]                ; get TCB stack pointer
    POP   {r4-r11}
    POP   {r0-r3}
    POP   {r12}
    ADD   sp, sp, #4            ; discard saved "R14"
    POP   {lr}                    ; get starting address of task
    ADD   sp, sp, #4            ; discard t-bit
    CPSIE I
    
    BX    lr                    ; begin executing first task

        ALIGN
        END