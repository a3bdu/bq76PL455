/*********************************************************************
* File:        Flash Operations.s
* Author:      Carlo Campanale
* Company:     Podium Engineering S.r.l.
* Processor:   dsPIC33FJ family
* Board:       DEV_1401-BQ76PL536EVMCtrlBrd-r08
*
* Created on 19 Marzo 2014, 10.35
*********************************************************************/

.global _ReadPMHigh
.global _ReadPMLow
.global _UnlockPM
.global _WritePMHigh
.global _WritePMHighB
.global _WritePMLow
.global _WritePMLowB

.section .text
_ReadPMHigh:
	tblrdh [W0],W0
	return

_ReadPMLow:
	tblrdl [W0],W0
	return

_UnlockPM:
	push	W0
	disi	#5
	mov		#0x55,W0
	mov		W0, NVMKEY
	mov		#0xAA, W0
	mov		W0, NVMKEY
	bset	NVMCON, #15
	nop
	nop
	btsc	NVMCON, #15
	bra		$-2
	pop		W0
	return

_WritePMHigh:
	tblwth	W0,[W1]
	return

_WritePMHighB:
	tblwth.b	W0,[W1]
	return

_WritePMLow:
	tblwtl	W0,[W1]
	return

_WritePMLowB:
	tblwtl.b	W0,[W1]
	return

.end



