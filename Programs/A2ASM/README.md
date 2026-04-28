Beobachtung:
1. AW01 swtzt R0 = 0x2000000c
2. AW02 setzt R2 = 0xef
3. AW03 setzt R3 = 0xbe
4. AW04 setzt R2 = 0xef00
5. AW05 R2 = 0xefbe

Interpretation:
Zunächst wird in AW01 mit ldr die Adresse in Register R0 geladen. Der Wert in der Adresse ist 2 byte (Halbwort oder auch Intel Wort(DCW) groß).
In AW02 wird ein Byte von den 2 Byte in R2 geladen (DCB), hier handelt es sich um den Wert 0xef, weil die Byte Ordner bei ARM Little Endian ist.
In AW03 wird wieder ein Byte geladen in R3, allerdings diesmal mit dem Offset 1, der Wert ist also diesmal 0xbe
In AW04 wird der Wert den R2 halten kann um 1 Byte (8bits) vergrößert und mit 0 befüllt
In AW05 wird das 2te Byte mit R1 gefüllt, der Wert ist jetzt 0xefbe, die Reihenfolge der Bits hat sich also im Vergleich zum Anfangswert verändert.
In AW06 wird der geänderte Wert zurück in den Speicher geschrieben.
