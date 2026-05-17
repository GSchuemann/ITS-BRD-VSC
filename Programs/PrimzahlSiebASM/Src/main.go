package main

import (
	"fmt"
)

var primArr = make([]byte, 126)
var curPos int64 = 2
var curBytePos = 0
var primes = make([]int16, 0) //Initialiersrung eines Dynamischen Arrays im Speicher

func main() {
	sieb()
	// for i:=2;i<1000;i++ {
	// TODO Ausgabe vom Array, bzw. dessen Validierung
	// }
	abspeichern()
	for i := 0; i < len(primes); i++ {
		fmt.Printf("%v ist eine Primzahl!\n", primes[i])
	}

}
func sieb() {
	arrPos := curPos % 8
	curByte := primArr[int(curPos>>3)]         //lsr 3 ist div 8
	bitToCheck := (curByte & (0x80 >> arrPos)) //setzt bitToCheck auf 0, wenn das bit 0 und sonst > 0
	//  bitToCheck := fmt.Sprintf("%08b\n", curByte)[arrPos]
	// fmt.Println(fmt.Sprintf("%08b ,%v\n", curByte, arrPos))
	if bitToCheck == 0 {
		// fmt.Printf("%v ist eine Primzahl \n", curPos)
		for i := (curPos * curPos); i < int64(len(primArr)*8); i += curPos {
			b := int(i >> 3)
			bit := (i + 8) & 0x07 //&(AND) 0x07 ist % 8
			mask := getMask(int(bit))
			primArr[b] = primArr[b] | byte(mask)
		}
	}
	curPos += 1
	if curPos < int64(len(primArr)*8) {
		sieb()
	}
}
func abspeichern() {
	for i := 2; i < len(primArr)*8; i++ {
		arrPos := i % 8
		curByte := primArr[i>>3]
		curBit := (curByte & (0x80 >> arrPos))

		if curBit == 0 {
			primes = append(primes, int16(i))
		}
	}

}
func getMask(bit int) int {
	return 0x80 >> bit
	// switch bit {
	// case 0:
	// 	return 0x80
	// case 1:
	// 	return 0x40
	// case 2:
	// 	return 0x20
	// case 3:
	// 	return 0x10
	// case 4:
	// 	return 0x08
	// case 5:
	// 	return 0x04
	// case 6:
	// 	return 0x02
	// case 7:
	// 	return 0x01
	// default:
	// 	return 0x00
	// }
}

// func div(arrPos int64) {
// 	if arrPos == 0 {

// 		return
// 	}
// 	byteToCheck := primArr[int(arrPos/8)]
// 	bitToCheck := fmt.Sprintf("%08b\n", byteToCheck)[arrPos]
// 	if bitToCheck == 49 {
// 		div(arrPos - 1)
// 	}
// 	if ((curPos + 2) % arrPos) == 0 {
// 		fmt.Printf("%v ist keine Primzahl!\n", curPos+2)
// 	}
// }
