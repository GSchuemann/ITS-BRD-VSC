;************************************************
;* Beginn der globalen Daten *
;************************************************
                   AREA MyData, DATA, align = 2
Base
PrimArr            FILL 126 
Primes             DCW 0

;***********************************************
;* Beginn des Programms *
;************************************************
    AREA |.text|, CODE, READONLY, ALIGN = 3
; ----- S t a r t des Hauptprogramms -----
    EXPORT main
    EXTERN initITSboard
main            PROC
              bl    initITSboard                 ; HW Initialisieren

              ldr R0,=PrimArr ;R0 = Start Adresse des Primzahlen Sieb Arrays
              mov R6,#0x80 ;R6 Register mit der Maske die geshiftet werden soll
              mov R1,#2 ;R1 ist der Index 2-1000, für die Primzahlen
fn_sieb       and R2,R1,#0x07 ;Label Sieb Funktion und R2 (currentBit) initialisieren mit Index Mod 8
              lsr R3,R1,#3 ;R3 (currentByte) mit Index div 8 belegen
              ldrb R3,[R0,R3] ;Laden des Bytes aus dem  in R3
              lsr R2,R6,R2 ;Bit maske für das zu Prüfende Bit erstellen
if_01         ands R3,R3,R2 ;Geladenes Byte mit der Bit Maske abgleichen
              bne endif_01 ;Wenn das gesuchte Bit 0 ist, den For Loop überpspringen
                     
then_01
for_01        mul R4,R1,R1 ;R4 ist der Index im Loop (nachfolgend l_Index), initialisieren mit Index * Index

until_01      cmp R4,#1000 ;Prüfung ob l_Index kleiner gleich 1000 ist
              bhi enddo_01 ;Wenn l_Index nicht kleiner gleich Tausend ist, Loop beenden, sonst weiter

do_01         and R2,R4,#0x07 ;l_currentBit mit l_Index mod 8 belegen
              lsr R3,R4,#3 ;l_currentByte mit l_Index div 8 belegen
              lsr R2,R6,R2 ;Die Maske für das currentBit vorbereiten
              ldrb R5,[R0,R3] ;Das Byte aus dem Primzahlsieb in R5 laden
              orr R2,R5,R2 ;Das Byte mit der erstellten Maske mit OR Kombinieren, damit die neue 1 gesetzt wird
              strb R2,[R0,R3] ;Das Byte wieder zurück in das Primzahlsieb schreiben.

step_01       add R4,R4,R1 ;l_Index um Index erhöhen
              b until_01
endif_01
enddo_01      add R1,R1,#1 ;Index um 1 erhöhen
              cmp R1,#31 ;Prüfen ob Index kleiner gleich 31, denn 32*32 ist größer als 1000
              bls fn_sieb ;Wenn Index kleiner gleich 31 Zur Anfangs Sieb Funktion springen (Recursive) sonst weiter (abspeichern)
end_fn_sieb

abspeichern   ldr R5,=Primes;R0 auf die Addresse von Primes setzen
              

for_02        mov R1,#2;Counter i mit Wert 2 initialisieren

until_02      cmp R1,#1000;Prüfen ob i kleiner gleich 1000 ist
              bhi enddo_02;Wenn größer 1000 Loop beenden, sonst weiter 

              and r2,r1,#0x07 ;curBit mit i Mod 8 belegen
              lsr R3,R1,#3 ;curByte mit i Div 8 belegen
              ldrb R3,[R0,R3] ; Byte aus dem Speicher laden
              lsr R2,R6,R2 ; Bit aus dem Byte extrahieren

if_02         ands R3,R3,R2 ;Prüfen ob das Bit 1 ist  
              bne step_02 ;Wenn 1 weiter, sonst weiter in den Loop 

then_02       strh R1,[R5,#2]! ;R1 speichern in R0 Offset 2 Pre-Increment

step_02       add R1,R1,#1 ;R1 um 1 erhöhen
              b until_02
endif_02
enddo_02      ldr R0,=Primes ;R1 auf die Addresse von Primes setzen
              SUB R1,R5,R0 ;R0 auf R0 - R1 setzen
              LSR R1,R1,#1 ;R0 um 2 nach Rechts shiften (logisch)          
              strh R1,[R0]
end_abspeichern
;In R1 den Wert von R0 speichern. Dadurch haben wir die Anzahl der Primzahlen als erstes Halbwort. 
;Und wir kennen die länge, welche die Primezahlen im Speicher belegen (Anzahl * 2)
     b .                     ; Anw0E
    
    ALIGN
    END