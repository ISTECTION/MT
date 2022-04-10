int main () {
    int a = 4;                              
    int b = 2;                              
    int f = (a + b) << ( (a + 1) & 1 );     
    char h = 2;                             
    h <<= b + ( (f - 11) | 2 );             
    char i = (h > f) << 3;                  
    int z = (3 > 2);                        
    z |= 6;                                 
}