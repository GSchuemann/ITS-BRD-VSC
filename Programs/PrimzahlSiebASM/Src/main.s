;Reserverieren eines Speicherbereiches Namens primArr mit 126Bytes
;Reserverieren eines halbwortes, Namens primes
;in R0 Add primArr laden
;R1 mit der Zahl 2 initialisieren
;Label Sieb Funktion und R2 initialisieren mit R1 AND 0x07, entspricht R1 Mod 8
;R3 mit R1 logischer Shift Right 3 belegen, entspricht R1 div 8
;R3 mit R3 ANDS Logischer Shift Right mit R2, wichtig ist das s bei AND, damit die Zero Flag gesetzt wird!

;Wenn Zero Flag gesetzt, weiter machen, sonst branch to SiebAfterLoop
;R4 mit R1 * R1 belegen

;Label loop_01 und R2 mit R4 AND 0x07 belegen
;R3 mit R4 logischer Shift Right 3 belegen
;R2 mit 0x80 logischer Shift R2 belegen
;In R0, Offset R4 den Wert aus R2 reinschreiben
;R4 mit R1 addieren
;Gucken ob R4 kleiner gleich 1000 ist
;Wenn R4 kleinergleich 1000, branch

;Label SiebAfterLoop und R1 um 1 erhöhen
;Gucken ob R1 kleiner gleich 1000 ist, wenn ja branch loop_01 sonst weiter
;Wenn R1 kleiner 1000 branch to Sieb sonst weiter (abspeichern)

;Label abspeichern R1 auf 2 setzen
;R0 auf die Addresse von Primes setzen

;Label SpeicherLoop R2 belegen mit R1 AND 0x07, entspricht R1 Mod 8
;R3 belegen mit R1 LSR 3, entspricht R1 div 8
;R3 mit 0x80 ANDS R3 belegen, wichtig wieder s, damit Zero Flag gesetzt wird
;Wenn Zero Flag gesetzt brach to Increment, sonst weiter 
;R1 speichern in R0 Offset 2 Pre-Increment

;Label Increment und R1 um 1 erhöhen
;Prüfen ob R1 kleiner gleich 1000 ist
;Wenn kleiner gleich 1000 branch SpeicherLoop, sonst weiter 
;R1 auf die Addresse von Primes setzen
;R0 auf R0 - R1 setzen
;R0 um 2 nach Rechts shiften (logisch)
;In R1 den Wert von R0 speichern. Dadurch haben wir die Anzahl der Primzahlen als erstes Halbwort. 
;Und wir kennen die länge, welche die Primezahlen im Speicher belegen (Anzahl * 2)
