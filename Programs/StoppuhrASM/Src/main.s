;******************** (C) COPYRIGHT HAW-Hamburg ********************************
;* File Name          : main.s
;* Author             : Franz Korf	
;* Version            : V1.0
;* Date               : 11.05.2022
;* Description        : Rahmen zur Loesung von GTP Woche 7-9 (Stoppuhr).
;
;*******************************************************************************

; Define address of selected GPIO and Timer registers
PERIPH_BASE     	equ	0x40000000                 ;Peripheral base address
AHB1PERIPH_BASE 	equ	(PERIPH_BASE + 0x00020000)
APB1PERIPH_BASE     equ PERIPH_BASE

GPIOD_BASE			equ	(AHB1PERIPH_BASE + 0x0C00)
GPIOF_BASE			equ	(AHB1PERIPH_BASE + 0x1400)
TIM2_BASE           equ (APB1PERIPH_BASE + 0x0000)
	
GPIO_F_PIN        	equ	(GPIOF_BASE + 0x10)

GPIO_D_PIN			equ	(GPIOD_BASE + 0x10)
GPIO_D_SET			equ (GPIOD_BASE + 0x18)
GPIO_D_CLR			equ	(GPIOD_BASE + 0x1A)
	
TIMER				equ (TIM2_BASE + 0x24)   ; CNT : current time stamp (32 bit),  resolution
TIM2_PSC			equ (TIM2_BASE + 0x28)   ; Prescaler  resolution
TIM2_ERG			equ (TIM2_BASE + 0x14)   ; 16 Bit register, Bit 0 : 1 Restart Timer


    EXTERN initITSboard
    EXTERN GUI_init
	EXTERN TP_Init
	EXTERN initTimer
	EXTERN lcdSetFont
	EXTERN lcdGotoXY      		; TFT goto x y function
	EXTERN lcdPrintS			; TFT output function
	EXTERN lcdPrintReplS	
    EXTERN lcdPrintC            ; TFT output one character		
	EXTERN Delay				; Delay (ms) function

ConstInitStart EQU 0
ConstInit EQU 4
ConstInitEnd EQU 8
ConstRunningStart EQU 12
ConstRunning EQU 16
ConstHoldStart EQU 20
ConstHold EQU 24

ConstXStart EQU 10
ConstYStart EQU 6

;********************************************
; Data section, aligned on 4-byte boundery
;********************************************
	AREA MyData, DATA, align = 2

DEFAULT_BRIGHTNESS	DCW     800
MY_TEXT				DCB		"Hold down different buttons from S0 to S7 and watch D8 to D15.", 0
MY_TIME		    	DCB		"00:00.00", 0
MY_TIME_NEW		    DCB		"00:00.00", 0
MY_TIME_OLD		    DCB		"00:00.00", 0

MY_CURRENT_STATE			DCB		0

TIME_DIVIDERS		DCD		60000000,6000000,0,1000000,100000,0,10000,1000
MY_STATES			DCD init_start, init, init_end,running_start,running,hold_start,hold
;********************************************
; Code section, aligned on 8-byte boundery
;********************************************
	AREA |.text|, CODE, READONLY, ALIGN = 3

;--------------------------------------------
; main subroutine
;--------------------------------------------
	EXPORT main [CODE]
	
main	PROC

		; Initialisierung der HW
		BL		initITSboard
		ldr   	r1, =DEFAULT_BRIGHTNESS
		ldrh 	r0, [r1]
		bl   	GUI_init
		bl  	initTimer
		ldr 	R1,=TIM2_PSC   			; Set pre scaler such that 1 timer tick represents 10 us
		mov 	R0,#(90*10-1) 
		strh	R0,[R1]
		ldr 	R1,=TIM2_ERG   			; Restart timer	
		mov		R0,#0x01
		strh	R0,[R1]					; Set UG Bit
		MOV 	R0, #24
		bl  	lcdSetFont

					; Ihre Initialisierung
					MOV R0,#ConstInitStart
					BL change_state

superloop
					LDR R0,=MY_CURRENT_STATE
					LDR R0,[R0]
					LDR	R1,=MY_STATES	
					LDR	R1,[R1,r0]
					LDR	R0,=GPIO_F_PIN
					ldrh	R0,[R0]	; read buttons
					BLX R1
					BAL superloop

change_state 		PROC
					LDR R1,=MY_CURRENT_STATE
					STRB R0,[R1]
					BX LR
					ENDP

init_start 			PROC
					PUSH {LR}
					MOV R0,#ConstXStart
					MOV R1,#ConstYStart
					MOV R8,#0
					BL lcdGotoXY
					LDR R0,=MY_TIME
					BL  	lcdPrintS
					BL clrAllLEDS
					MOV R0, #ConstInit
					BL change_state
					POP {PC}
					ENDP

init    			PROC
					PUSH {LR}
					MOV R1,#1

if_01   			TST R0, R1 ,LSL #7
					BNE endif_01
		
then_01 			MOV R0, #ConstInitEnd
					BL change_state
					MOV R0,#7
					BL waitForRelease
endif_01		
        			POP {PC}
					ENDP

init_end	 		PROC
					PUSH {LR}
					ldr 	R1,=TIM2_ERG   			; Restart timer	
					mov		R0,#0x01
					strh	R0,[R1]					; Set UG Bit
					MOV R0, #ConstRunningStart
					BL change_state
					POP {PC}
					ENDP

running_start	 	PROC
					PUSH {LR}
					MOV R1,#0x1 ;LED 7
					BL setLED
					BL clrHoldLED
					MOV R0, #ConstRunning
					BL change_state
					POP {PC}
					ENDP

running 			PROC
					PUSH {LR,R0}
					BL setTime
					BL displayTime
					MOV R1,#1
					POP {R0}
if_02   			TST R0, R1,LSL #6
					BEQ then_02
					BAL else_if_02

then_02				MOV R0, #ConstHoldStart
					BL change_state
					MOV R0,#6
					BL waitForRelease
					BAL endif_02

else_if_02			TST R0, R1 ,LSL #5
					BNE endif_02

else_then_02		MOV R0, #ConstInitStart
					BL change_state
					MOV R0, #5
					BL waitForRelease

endif_02
					POP {PC}
					ENDP

hold_start			PROC 
					PUSH {LR}
					MOV R1,#2
					BL setLED
					MOV R0, #ConstHold
					BL change_state
					POP {PC}
					ENDP

hold    			PROC
					PUSH{LR}
					MOV R1,#1
if_03				TST  R0, R1 ,LSL #5
					BEQ then_03
					BAL else_if_03

then_03				MOV R0, #ConstInitStart
					BL change_state
					MOV R0,#5
					BL waitForRelease
					BAL endif_03

else_if_03			TST  R0, R1 ,LSL #7
					BNE endif_03	

else_then_03		MOV R0, #ConstRunningStart
					BL change_state
					MOV R0,#7
					BL waitForRelease

endif_03
        			POP {PC}
					ENDP

clrAllLEDS			PROC
					LDR R0,=GPIO_D_CLR
					MOV R2,#0xFF
					STR R2,[R0]
					BX LR
					ENDP

clrHoldLED			PROC
					LDR R0,=GPIO_D_CLR
					MOV R2,#0x02;LED 8
					STR R2,[R0]
					ENDP
setLED				PROC
					LDR R0,=GPIO_D_SET
					STR R1,[R0]
					BX LR
					ENDP

waitForRelease	 	PROC
					LDR		R1,=GPIO_F_PIN
					ldrh	R1,[r1]
					MOV R2,#1
					LSL R3,R2,R0
					TST  R1, R3
					BEQ waitForRelease
					BX LR
					ENDP
setTime		PROC
					PUSH {LR}
					LDR     R0,=TIMER
					LDR     R0,[R0]
					MOV R1,#0
					BL updateTimeString
					MOV R1,#1
					BL updateTimeString
					MOV R1,#3
					BL updateTimeString
					MOV R1,#4
					BL updateTimeString
					MOV R1,#6
					BL updateTimeString
					MOV R1,#7
					BL updateTimeString
					POP {PC}
					ENDP
updateTimeString 	PROC
					PUSH{R4}
					LDR R2,=MY_TIME_NEW
					LDR R4,=TIME_DIVIDERS
					LDR R4,[R4,R1,LSL #2]

					UDIV R3,R0,R4
					MLS R0,R4,R3,R0 ;R0 = R0 - (R4 * R3)

					ADD R3,#'0'
					STRB R3,[R2,R1]
					POP {R4}
					BX LR
					ENDP

displayTime			PROC
					PUSH {LR,R4,R5,R6}
					LDR R5, =MY_TIME_NEW
					LDR R6, =MY_TIME_OLD
					MOV R4,#0
repeat_01		
					LDRB R0, [R5,R4] 
					LDRB R1, [R6,R4]
					STRB R0, [R6,R4]
					CMP R0,R1
					MOV R1,R4
					ADD R1,#ConstXStart
					BLNE writeToScreen
					ADD R4,#1
until_01
					CMP R4,#8
					BEQ enduntil_01
					BAL repeat_01

enduntil_01
					POP {PC,R4,R5,R6}
					ENDP
writeToScreen		PROC 
					PUSH {LR,R0}
					MOV R0,R1
					MOV R1,#6
					BL lcdGotoXY
					POP {R0}
					BL lcdPrintC
					POP  {PC}
					ENDP 

		ALIGN
		END
