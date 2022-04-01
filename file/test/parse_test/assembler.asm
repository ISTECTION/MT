.386                                                                             ; разрешает ассемблирование непривилегированных инструкций процессора 80386
.MODEL FLAT, STDCALL                                                            

EXTRN ExitProcess@4:PROC                                                         ; выход

.DATA                                                                            ; сегмент данных
test_2   dd ?
c        db ?
t        dd ?
g        dd ?
a        dd ?
v        dd ?
d        dd ?
test_    dd ?
b        dd ?
e        db ?
w        db ?
test_3   dd ?

const_0_0        dd 5
const_0_0        dd 3
const_0_0        dd 33
const_0_0        dd 222
const_0_0        dd 2
const_0_0        dd 85
const_0_0        dd 8
const_0_0        dd 22

.CODE                                                                            ; сегмент кода
MAIN PROC                                                                        ; метка точки входа
