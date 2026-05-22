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

       ldr R0,=PrimArr ;in R0 Add primArr laden
       mov R6,#0x80
       mov R1,#2 ;R1 mit der Zahl 2 initialisieren
sieb   and R2,R1,#0x07 ;Label Sieb Funktion und R2 initialisieren mit R1 AND 0x07, entspricht R1 Mod 8
       lsr R3,R1,#3 ;R3 mit R1 logischer Shift Right 3 belegen, entspricht R1 div 8
       lsr R2,R3,R2
        ands R3,R3,R2 ;R3 mit R3 ANDS Logischer Shift Right mit R2, wichtig ist das s bei AND, damit die Zero Flag gesetzt wird!
        bne SiebAfterLoop ;Wenn Zero Flag gesetzt, weiter machen, sonst branch to SiebAfterLoop


loop_01 mul R4,R1,R1 ;R4 mit R1 * R1 belegen

        AND R2,R4,#0x07 ;Label loop_01 und R2 mit R4 AND 0x07 belegen
        lsr R3,R4,#3 ;R3mit R4 logischer Shift Right 3 belegen
        lsr R2,R6,R2 ;R2 mit 0x80 logischer Shift R2 belegen
        ldr R5,[R0,R3] ;R5 mit R0 Offset R3 belegen 
        ldr R5,[R5]
        orr R2,R5,R2 ;R5 mit R5 OR R2 belegen
        strb R2,[R0,R3] ;In R0, Offset R3 den Wert aus R2 reinschreiben
        add R4,R4,R1 ;R4 mit R1 addieren
        cmp R4,#31;Gucken ob R4 kleiner gleich 31 ist
        bls loop_01 ;Wenn R4 kleinergleich 31, branch

SiebAfterLoop add R1,R1,#1 ;Label SiebAfterLoop und R1 um  erhöhen
        cmp R1,#1 ;Gucken ob R1 gleich 1 ist, wenn ja branch loop_01 sonst weiter
        bls sieb ;Wenn R1 kleiner gleich 31 branch to Sieb sonst weiter (abspeichern)

abspeichern mov R1,#2;Label abspeichern R1 auf 2 setzen
        ldr R0,=Primes;R0 auf die Addresse von Primes setzen

SpeicherLoop and r2,r1,#0x07 ;Label SpeicherLoop R2 belegen mit R1 AND 0x07, entspricht R1 Mod 8
        lsr R3,R1,#3 ;R3 belegen mit R1 LSR 3, entspricht R1 div 8
        ands R3,R6,R3 ;R3 mit 0x80 ANDS R3 belegen, wichtig wieder s, damit Zero Flag gesetzt wird
        bne Increment ;Wenn Zero Flag gesetzt brach to Increment, sonst weiter 
        strh R1,[R0,#2]! ;R1 speichern in R0 Offset 2 Pre-Increment

Increment add R1,R1,#1 ;R1 um 1 erhöhen
        cmp R1,#1000;Prüfen ob R1 kleiner gleich 1000 ist
        bls SpeicherLoop;Wenn kleiner gleich 1000 branch SpeicherLoop, sonst weiter 
        ldr R1,=Primes ;R1 auf die Addresse von Primes setzen
        SUB R0,R0,R1 ;R0 auf R0 - R1 setzen
        LSR R1,R1,#2 ;R0 um 2 nach Rechts shiften (logisch)          
        strd R0,[R1]
;In R1 den Wert von R0 speichern. Dadurch haben wir die Anzahl der Primzahlen als erstes Halbwort. 
;Und wir kennen die länge, welche die Primezahlen im Speicher belegen (Anzahl * 2)
    b .                     ; Anw0E
    
    ALIGN
    END