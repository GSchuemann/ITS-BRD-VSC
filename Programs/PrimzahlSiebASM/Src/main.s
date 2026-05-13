;Reserverieren eines Speicherbereiches mit 125Bytes
;Wenn in dem Speicherbereich ein Bit 1 ist, dann ist die Zahl an der Bitposition keine Primzahl, es wird von 2 Inklusive Indexiert, das letze Bit wird ignoriert
;Es gibt einen curPos Zähler, der bei 0 beginnt.
;Wenn im Speicherbereich index[curPos] eine 0 steht, wird solange versucht die zahl durch kleinere Werte (die auch 0 sind) im Speicherbereich den Rest zu ermitteln, bis einer gefunden wird.
;Wenn nicht, wird index[curPos] auf 0 gelassen. Danach wird die Zahl solange mit sich selber addiert, bis sie größer 1000 ist. Für jede errechnete Pos wird der Wert im Speicherbereich auf 1 gesetzt.
;Wenn einer gefunden, wird index[curPos] auf 1 gesetzt.
;In beiden Fällen wird curPos um 1 erhöht.