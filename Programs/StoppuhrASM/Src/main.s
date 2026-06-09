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


;********************************************
; Data section, aligned on 4-byte boundery
;********************************************
	AREA MyData, DATA, align = 2

DEFAULT_BRIGHTNESS	DCW     800
MY_TEXT				DCB		"Hold down different buttons from S0 to S7 and watch D8 to D15.", 0
MY_INIT			    DCB		"Current Step: init.", 0
MY_RUNNING			DCB		"Current Step: Runn.", 0
MY_STOP				DCB		"Current Step: Stop.", 0

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
		MOV R0, #2
		BL changeState
		MOV R11,#0x01 ;LED PrüfMaske

superloop
		; read buttons
		LDR		R2,=GPIO_F_PIN
		ldrh	R2,[R2]
if_01   CMP R0,#2

then_01 BEQ init

else_if_02 CMP R0,#4

then_02 BEQ running

else_if_03 CMP R0,#8

then_03 BEQ stop
		;and		R0,#0xFF   ; set bit 31 to 8 of R0 to 0 ; bit 7 to 0 do not change
		; bit i for R0 is 1 <=> button S<i> not pressed (for 0 <= i <= 7)
		; bit i for R0 is 0 <=> button S<i>     pressed (for 0 <= i <= 7)
		
init    PROC
		TST    R2, R11 ,LSL #7
		BLEQ s7Pressed	
        BAL endif_01
		ENDP
running    PROC
		PUSH{R2,R3}
		TST    R2, R11 ,LSL #6
		BLEQ s6Pressed
		POP{R2,R3}
		TST  R2, R11 ,LSL #5
		BLEQ s5Pressed
        BAL endif_01
		ENDP
stop    PROC
		PUSH{R2,R3}
		TST    R2, R11 ,LSL #5
		BLEQ s5Pressed
		POP{R2,R3}	
		TST  R2, R11 ,LSL #7
		BLEQ s7Pressed	
        BAL endif_01
		ENDP

endif_01
        BAL superloop

s5Pressed       PROC
				PUSH {LR}
				MOV R0,#5
				BL waitForRelease
				LDR R0,=GPIO_D_CLR
			    MOV R2,#0x03
				STR R2,[R0]
				MOV R0,#2
				bl changeState
				POP {LR}
				BX LR
				ENDP
s6Pressed       PROC 
				PUSH {LR}
				MOV R0,#6
				BL waitForRelease
				LDR R0,=GPIO_D_SET
			    MOV R2,#2
				STR R2,[R0]
				MOV R0,#8
				bl changeState
				POP {LR}
				BX LR
				ENDP

s7Pressed       PROC 
				PUSH {LR}
				MOV R0,#7
				BL waitForRelease
				LDR R0,=GPIO_D_SET
			    MOV R2,#1
				STR R2,[R0]
				LDR R0,=GPIO_D_CLR
			    MOV R2,#0x02
				STR R2,[R0]
				MOV R0,#4
				BL changeState
				POP {LR}
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
changeState PROC ; TAKES R0 and switches State depending on that, R0 = 2 Init, R0 = 4, Running, R0 8 Stop
            PUSH {LR,R0}
            CMP R0,#2
			LDREQ R0,=MY_INIT
            CMP R0,#4
			LDREQ R0,=MY_RUNNING
			CMP R0,#8
			LDREQ R0,=MY_STOP
			BL setText
			POP {LR,R0}
            BX LR
            ENDP

setText     PROC
            PUSH {LR,R0}
			MOV R0,#5
			MOV R1,#6
			BL lcdGotoXY
			POP {R0}
			BL  	lcdPrintS
			POP {LR}
			BX LR
            ENDP

		; switch LEDs off (button s<i> not pressed : LED D<î+8> switched off (for 0 <= i <= 7)
		;LDR		R1,=GPIO_D_CLR
		;str		R0,[R1]
		
		; switch LEDs on (button s<i>      pressed : LED D<î+8> switched on  (for 0 <= i <= 7)
		;eor		R1,R1,#0xFF       ; toogle bit 0 to 7 of R1

		ALIGN
		END
