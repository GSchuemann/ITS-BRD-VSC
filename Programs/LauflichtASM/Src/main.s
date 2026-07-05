;******************** (C) COPYRIGHT HAW-Hamburg ********************************
;* File Name          : main.s
;* Author             : Franz Korf  
;* Version            : V1.0
;* Date               : 16.05.2022
;* Modified by        : Thomas Lehmann, 2024-07-12
;* Description        : This is the frame for the last assignment.
;                     : Einfaches Lauflicht.
;
;*******************************************************************************
    EXTERN initITSboard
    EXTERN lcdPrintS            ;Display ausgabe
    EXTERN GUI_init
    EXTERN TP_Init
    EXTERN delay
        
; Define address of selected GPIO and Timer registers
PERIPH_BASE         equ 0x40000000                 ;Peripheral base address
AHB1PERIPH_BASE     equ (PERIPH_BASE + 0x00020000)
APB1PERIPH_BASE     equ PERIPH_BASE

GPIOD_BASE          equ (AHB1PERIPH_BASE + 0x0C00)
GPIOE_BASE          equ (AHB1PERIPH_BASE + 0x1000)
GPIOF_BASE          equ (AHB1PERIPH_BASE + 0x1400)
TIM2_BASE           equ (APB1PERIPH_BASE + 0x0000)

GPIO_F_PIN          equ (GPIOF_BASE + 0x10)

GPIO_D_PIN          equ (GPIOD_BASE + 0x10)
GPIO_D_SET          equ (GPIOD_BASE + 0x18)
GPIO_D_CLR          equ (GPIOD_BASE + 0x1A) 
    
GPIO_E_PIN          equ (GPIOE_BASE + 0x10)
GPIO_E_SET          equ (GPIOE_BASE + 0x18)
GPIO_E_CLR          equ (GPIOE_BASE + 0x1A)     



;********************************************
; Data section, aligned on 4-byte boundery
;********************************************   
    AREA MyData, DATA, align = 2
TestPattern DCW     0x8000, 0x7000, 0x5000

;********************************************
; Code section, aligned on 8-byte boundery
;********************************************
    AREA |.text|, CODE, READONLY, ALIGN = 3

;--------------------------------------------
; main subroutine
;--------------------------------------------

        
; Unterprogramm Lauftlicht
;
; Einfaches Lauflicht, das ein Bitmuster zyklisch ueber die 
; LEDs D23 bis D8 schiebt. Das LED Muster wird nach rechts 
; geschoben. Die Frequenz betraegt 2 Hz.
;
; IN R0  Die unteren 16 Bits von R0 speichern das Muster, mit
;        dem die LEDs initialisiert werden.
; IN R1  Anzahl Schritte, die das Lauflicht laufen soll.
;--------------------------------------------       
;

DelayTime   EQU     4000

Modulo        	PROC ;R0 zähler, R1 Teiler
				UDIV R2,R0,R1
				MUL R2,R1
				SUB R0,R0,R2
				BX LR
				ENDP

rot_rtl     PROC ;Leider Rotation nach Links kein RRX Möglich

            TST R0,#0x1 << 15 ; gucke ob ein Bit rausgeshifted wird
            LSL R0,#0x1 ;macht nichts mit den Flags
            ORRNE R0,#0x1 ; reinshiften, wenn es rausgeshifted wurde
            BX LR
            ENDP


setleds    PROC
            LDR R1, =GPIO_D_SET
            STR R0, [R1]
            LSR R0, #8
            LDR R1, =GPIO_E_SET
            STR R0, [R1]
            BX LR
           ENDP

clrleds     PROC
            SUB SP, SP, #4
            STR R0, [SP]
            LDR R1, =GPIO_D_PIN
            LDR R1, [R1]
            EOR R0, R0, R1
            LDR R1, =GPIO_D_CLR
            STR R0, [R1]
            LDR R0, [SP]
            LDR R1, =GPIO_E_PIN
            LDR R1, [R1]
            LSR R0, #8
            EOR R0, R0, R1
            LDR R1, =GPIO_E_CLR
            STR R0, [R1]
            ADD SP, SP, #4
            BX LR
            ENDP
Lauflicht   PROC

			PUSH {LR}
            SUB SP, SP, #8
            STR R0, [SP,#4]
            STR R1, [SP,#0]
            BL setleds

while_01    LDR R1, [SP,#0]
            CMP R1, #0
            BGT do_01
            BAL endwhile_01

do_01       MOV R0, #500
            BL delay
            LDR R0, [SP,#4]
            BL rot_rtl
            STR R0, [SP, #4]
            BL setleds
            LDR R0, [SP,#4]
            BL clrleds
            LDR R1, [SP,#0]
            SUB R1, #1
            STR R1, [SP, #0]
            BL while_01

endwhile_01

			;LSR R0,#8
			STR R0,[R2]
            ADD SP,SP,#8
            POP{PC}
            ENDP

;--------------------------------------------
; main subroutine
;--------------------------------------------
    EXPORT main [CODE]
        
InterTestDelay  EQU     4000
    
main    PROC
        BL initITSboard
        LDR     R7, =TestPattern
        MOV     R8, #0                  ; Laufindex Testpattern
forever 
        CMP     R8, #3
        MOVGE   R8, #0
        
        ; Test Lauflicht
        LDRH    R0, [R7,R8,LSL #1]
        MOV     R1, #20
        BL      Lauflicht
        
        MOV     R0, #InterTestDelay
        BL      delay

        ADD     R8, #1
        BAL     forever     ; nowhere to retun if main ends     
        ENDP
    
        ALIGN
        END
