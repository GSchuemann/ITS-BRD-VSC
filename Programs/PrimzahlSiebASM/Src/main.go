package main

import (
	"fmt"
)

var primArr = make([]byte, 126)
var curPos int64 = 2
var primes = make([]int16, 0) //Initialiersrung eines Dynamischen Arrays im Speicher

func main() {
	sieb()
	abspeichern()
	for i := 0; i < len(primes); i++ {
		fmt.Printf("%v ist eine Primzahl!\n", primes[i])
	}

}

func sieb() {
	curBit := curPos % 8
	curByte := primArr[int(curPos>>3)]         //lsr 3 ist div 8
	bitToCheck := (curByte & (0x80 >> curBit)) //setzt bitToCheck auf 0, wenn das bit 0 und sonst > 0

	if bitToCheck == 0 {
		// fmt.Printf("%v ist eine Primzahl \n", curPos)
		for i := (curPos * curPos); i < int64(len(primArr)*8); i += curPos {
			curBit = i & 0x07 //&(AND) 0x07 ist % 8
			curByte = byte(i >> 3)
			primArr[curByte] = primArr[curByte] | (0x80 >> curBit)
		}
	}
	curPos += 1
	if curPos < int64(len(primArr)*8) {
		sieb()
	}
}
func abspeichern() {
	for i := 2; i < len(primArr)*8; i++ {
		curBit := i % 8
		curByte := primArr[i>>3]
		bitToCheck := (curByte & (0x80 >> curBit))

		if bitToCheck == 0 {
			primes = append(primes, int16(i))
		}
	}

}
