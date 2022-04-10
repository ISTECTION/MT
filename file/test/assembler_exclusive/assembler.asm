.386
.MODEL FLAT, STDCALL

EXTRN ExitProcess@4:PROC

.DATA
tmp_var      dd ?
tmp_bit      dd ?
g            dd ?
l            dd ?
b            dd ?
p            dw ?
u            dd ?

const_2_0        dd 1
const_14_0       dd 5
const_32_0       dd 2
const_40_0       dd 9
const_43_0       dd 4
const_43_1       dd -1

.CODE
MAIN PROC
FINIT

FILD const_14_0
FISTP u

FILD u
FILD const_40_0
FSUB
FILD const_43_0
FADD
FISTP g

FILD u
FILD const_2_0
FISTP tmp_bit
FISTP tmp_var
MOV ECX, tmp_bit
SHR tmp_var, CL
FILD tmp_var
FILD u
FILD g
FILD const_32_0
FADD
FISTP tmp_bit
FISTP tmp_var
MOV ECX, tmp_bit
SHL tmp_var, CL
FILD tmp_var
FADD
FISTP p

FILD const_43_1
FILD const_32_0
FMUL
FISTP b

FILD u
FILD b
FADD
FISTP l

PUSH 0
CALL ExitProcess@4
MAIN ENDP
END MAIN