.386
.MODEL FLAT, STDCALL

EXTRN ExitProcess@4:PROC

.DATA
tmp_var      dd ?
tmp_bit      dd ?
a            dd ?
f            dd ?
i            dw ?
b            dd ?
z            dd ?
h            dw ?

const_2_0        dd 1
const_19_0       dd 11
const_23_0       dd 3
const_32_0       dd 2
const_43_0       dd 4
const_46_0       dd 6

.CODE
MAIN PROC
FINIT

FILD const_43_0
FISTP a

FILD const_32_0
FISTP b

FILD a
FILD b
FADD
FILD a
FILD const_2_0
FADD
FILD const_2_0
FISTP tmp_var
MOV ECX, tmp_var
FISTP tmp_var
AND tmp_var, ECX
FILD tmp_var
FISTP tmp_bit
FISTP tmp_var
MOV ECX, tmp_bit
SHL tmp_var, CL
FILD tmp_var
FISTP f

FILD const_32_0
FISTP h

FILD b
FILD f
FILD const_19_0
FSUB
FILD const_32_0
FISTP tmp_var
MOV ECX, tmp_var
FISTP tmp_var
OR tmp_var, ECX
FILD tmp_var
FADD
FISTP tmp_bit
MOV ECX, tmp_bit
SHL h, CL

FILD h
FILD f
FCOM
FSTSW AX
SAHF
JL JMP_ZERO_0
	FLD1
JMP JMP_END_0
JMP_ZERO_0:
	FLDZ
JMP_END_0:
FILD const_23_0
FISTP tmp_bit
FISTP tmp_var
MOV ECX, tmp_bit
SHL tmp_var, CL
FILD tmp_var
FISTP i

FILD const_23_0
FILD const_32_0
FCOM
FSTSW AX
SAHF
JL JMP_ZERO_1
	FLD1
JMP JMP_END_1
JMP_ZERO_1:
	FLDZ
JMP_END_1:
FISTP z

FILD const_46_0
FISTP tmp_var
MOV ECX, tmp_var
OR z, ECX

PUSH 0
CALL ExitProcess@4
MAIN ENDP
END MAIN