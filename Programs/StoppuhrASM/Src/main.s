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

Const10ms EQU 1000
Const100ms EQU 10000
ConstS EQU 100000
Const10S EQU 1000000
Constmin EQU 10000000
Const10min EQU 100000000

;********************************************
; Data section, aligned on 4-byte boundery
;********************************************
	AREA MyData, DATA, align = 2

DEFAULT_BRIGHTNESS	DCW     800
MY_TEXT				DCB		"Hold down different buttons from S0 to S7 and watch D8 to D15.", 0
MY_TIME		    	DCB		"00:00.00", 0
MY_TIME_NEW		    DCB		"00:00.00", 0
MY_TIME_OLD		    DCB		"00:00.00", 0
TEN_MS		        DCB     "0"
HUNDRED_MS		   	DCB		"0"
SECOND       		DCB		"0"
TEN_SECONDS			DCB		"0"	
MINUTE       		DCB		"0"
TEN_MINUTES			DCB		"0"

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
		MOV R9, #0     ;State Index
		MOV R11,#0x01 ;LED PrüfMaske

superloop
		; read buttons
		BL checktimer
		LDR		R2,=GPIO_F_PIN
		ldrh	R2,[R2]
		LDR	R1,=MY_STATES	
		LDR	R1,[R1,R9]	
		BLX R1
		BAL superloop

init_start 	PROC
			PUSH {LR}
			MOV R0,#10
			MOV R1,#6
			MOV R8,#0
			BL lcdGotoXY
			LDR R0,=MY_TIME
			BL  	lcdPrintS
			BL clrAllLEDS
			MOV R9, #4
			POP {PC}
			ENDP
init    PROC
		PUSH {LR}
if_01   TST    R2, R11 ,LSL #7
		BNE endif_01
		
then_01 MOV R0,#7
		MOV R9, #8
		BL waitForRelease

endif_01		
        POP {PC}
		ENDP
init_end 	PROC
			ldr 	R1,=TIM2_ERG   			; Restart timer	
			mov		R0,#0x01
			strh	R0,[R1]					; Set UG Bit
			MOV R9, #12
			BX LR 
			ENDP
running_start 	PROC
				PUSH {LR}
				MOV R1,#0x1
				BL setLED
				BL clrHoldLED
				MOV R9, #16
				POP {PC}
				ENDP
running PROC
		PUSH {LR,R2}
		BL displayTime
		POP{R2}	
if_02   TST R2, R11 ,LSL #6
		BNE endif_02

then_02	MOV R0,#6
		MOV R9, #20
		BL waitForRelease
endif_02		

if_03   TST  R2, R11 ,LSL #5
		BNE endif_03

then_03 MOV R9, #0
		MOV R0, #5
		BL waitForRelease

endif_03
		POP {PC}
		ENDP
hold_start	PROC 
			PUSH {LR}
			MOV R1,#2
			BL setLED
			MOV R9, #24
			POP {LR}
			BX LR
			ENDP
hold    PROC
		PUSH{LR,R2}
if_04	TST    R2, R11 ,LSL #5
		BNE endif_04

then_04	MOV R0,#5
		MOV R9, #0
		BL waitForRelease
endif_04		

		POP{R2}	
if_05	TST  R2, R11 ,LSL #7
		BNE endif_05

then_05	MOV R0,#7
		MOV R9, #12
		BL waitForRelease
endif_05
        POP {PC}
		ENDP

clrAllLEDS		PROC
				LDR R0,=GPIO_D_CLR
				MOV R2,#0xFF
				STR R2,[R0]
				BX LR
				ENDP
clrHoldLED		PROC
				LDR R0,=GPIO_D_CLR
				MOV R2,#0x02
				STR R2,[R0]
				ENDP
setLED			PROC
				LDR R0,=GPIO_D_SET
				STR R1,[R0]
				BX LR
				ENDP

waitForRelease 	PROC
				LDR		R2,=GPIO_F_PIN
				ldrh	R2,[R2]
				LSL R10,R11,R0
				TST  R2, R10
				BEQ waitForRelease
				BX LR
				ENDP
checktimer		PROC
				PUSH {LR}
				LDR     R2,=TIMER
				LDR     R2,[R2]
				MOV R3,#0
				BL updateTimeString
				MOV R2,R0
				MOV R3,#1
				BL updateTimeString
				MOV R2,R0
				MOV R3,#3
				BL updateTimeString
				MOV R2,R0
				MOV R3,#4
				BL updateTimeString
				MOV R2,R0
				MOV R3,#6
				BL updateTimeString
				MOV R2,R0
				MOV R3,#7
				BL updateTimeString
				POP {PC}
				ENDP
updateTimeString 	PROC
					PUSH{R4}
					LDR R0,=MY_TIME_NEW
					LDR R4,=TIME_DIVIDERS
					LDR R4,[R4,R3,LSL #2]
					UDIV R1,R2,R4

if_06				CMP R1,#0
					BNE then_06
					BAL else_06

then_06				MUL R4,R1
					SUB R4,R2,R4
					BAL endif_06

else_06				MOV R4,R2

endif_06			ADD R1,#0x30
					STRB R1,[R0,R3]
					MOV R0,R4
					POP {R4}
					BX LR
					ENDP

displayTime		PROC
				PUSH {LR,R4}
				LDR R0, =MY_TIME_NEW
				LDR R2, =MY_TIME_OLD
				MOV R4,#0
repeat_01		
				LDRB R0, [R0,R4] 
				LDRB R1, [R2,R4]
				STRB R0, [R2,R4]
				CMP R0,R1
				MOV R2,R4
				ADD R2,#10
				MOV R3,R0
				BLNE writeToScreen

				LDR R0, =MY_TIME_NEW
				LDR R2, =MY_TIME_OLD
				ADD R4,#1
until_01
				CMP R4,#8
				BEQ enduntil_01
				BAL repeat_01

enduntil_01
				POP {PC,R4}
				ENDP
writeToScreen	PROC ;R2 is the pos of the digit to Write, R3 is the digit
				PUSH {LR}
				MOV R0,R2
				MOV R1,#6
				BL lcdGotoXY
				MOV R0,R3
				BL lcdPrintC
				POP  {PC}
				ENDP 



		; switch LEDs off (button s<i> not pressed : LED D<î+8> switched off (for 0 <= i <= 7)
		;LDR		R1,=GPIO_D_CLR
		;str		R0,[R1]
		
		; switch LEDs on (button s<i>      pressed : LED D<î+8> switched on  (for 0 <= i <= 7)
		;eor		R1,R1,#0xFF       ; toogle bit 0 to 7 of R1

		ALIGN
		END
