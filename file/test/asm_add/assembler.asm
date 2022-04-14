.386
.MODEL FLAT, STDCALL

EXTRN ExitProcess@4:PROC

.DATA
tmp_var      dd ?
tmp_bit      dd ?
a            dd ?
b            dd ?
j            dw ?

const_2_0        dd 1
const_11_0       dd 7
const_21_0       dd 0
const_23_0       dd 3
const_32_0       dd 2
const_40_0       dd 9

.CODE
MAIN PROC
FINIT

FILD const_21_0
FILD const_11_0
FILD const_23_0
FCOMPP
FSTSW AX
SAHF
JNE JMP_ZERO_0
	FLD1
JMP JMP_END_0
JMP_ZERO_0:
	FLDZ
JMP_END_0:
FADD
FISTP a

FILD const_2_0
FILD const_40_0
FILD const_11_0
FCOMPP
FSTSW AX
SAHF
JE JMP_ZERO_1
	FLD1
JMP JMP_END_1
JMP_ZERO_1:
	FLDZ
JMP_END_1:
FADD
FISTP b

FILD const_32_0
FILD b
FILD const_32_0
FCOMPP
FSTSW AX
SAHF
JNE JMP_ZERO_2
	FLD1
JMP JMP_END_2
JMP_ZERO_2:
	FLDZ
JMP_END_2:
FSUB
FISTP j

PUSH 0
CALL ExitProcess@4
MAIN ENDP
END MAIN