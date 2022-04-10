.386                                                                             ; разрешает ассемблирование непривилегированных инструкций процессора 80386
.MODEL FLAT, STDCALL                                                            

EXTRN ExitProcess@4:PROC                                                         ; выход

.DATA                                                                            ; сегмент данных
tmp_var      dd ?                                                                ; переменная, для преобразования типов
tmp_bit      dd ?                                                                ; переменная, для операций побитового сдвига
a            dd ?
b            dd ?

const_32_0       dd 2
const_43_0       dd 4

.CODE                                                                            ; сегмент кода
MAIN PROC                                                                        ; метка точки входа
FINIT                                                                            

FILD a
FILD const_43_0
FISTP a

FILD b
FILD const_32_0
FISTP b

FILD a
FILD a
FILD b
FILD const_32_0
FADD 
FISTP tmp_bit
FISTP tmp_var
MOV ECX, tmp_bit
SHL tmp_var, CL
FILD tmp_var
FISTP a

PUSH 0                                                                           ; параметр: код выхода
CALL ExitProcess@4
MAIN ENDP
END MAIN