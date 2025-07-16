#include "os_type.h"

template<typename T>
void swap(T &x, T &y) {
    T z = x;
    x = y;
    y = z;
}


void itos(char *numStr, uint32 num, uint32 mod) {
    // 只能转换2~26进制的整数
    if (mod < 2 || mod > 26 || num < 0) {
        return;
    }

    uint32 length, temp;

    // 进制转换
    length = 0;
    while(num) {
        temp = num % mod;
        num /= mod;
        numStr[length] = temp > 9 ? temp - 10 + 'A' : temp + '0';
        ++length;
    }

    // 特别处理num=0的情况
    if(!length) {
        numStr[0] = '0';
        ++length;
    }

    // 将字符串倒转，使得numStr[0]保存的是num的高位数字
    for(int i = 0, j = length - 1; i < j; ++i, --j) {
        swap(numStr[i], numStr[j]);
    }
    
    numStr[length] = '\0';
}

void ftos(char *numStr, double num){
    if (num == 0.0) {
        numStr[0] = '0';
        numStr[1] = '.';
        numStr[2] = '0';
        numStr[3] = '\0';
        return;
    }
    int intPart = (int)num;
    char intStr[32];
    itos(intStr, intPart, 10);
    int i;
    for (i = 0; intStr[i]; i++) {
        numStr[i] = intStr[i];
    }
    numStr[i++] = '.';
   
    double fracPart = num - intPart;
    int precision = 6;
    for (int j = 0; j < precision; j++) {
        fracPart *= 10;
        int digit = (int)fracPart;
        numStr[i++] = digit + '0';
        fracPart -= digit;
    }
    numStr[i] = '\0';
}

void ftos_only_intpart(char *numStr, double num){
    if (num == 0.0) {
        numStr[0] = '0';
        numStr[1] = '\0';
        return;
    }
    
    int intPart=(int)num;
    double fracPart = num - intPart;
    if(fracPart>=0.5){
        intPart++;
    }
    char intStr[32];
    itos(intStr,intPart,10);
    int i;
    for (i = 0; intStr[i]; i++) {
        numStr[i] = intStr[i];
    }
    numStr[i] = '\0';
}


void ftoe(char *numStr, double num){
     if (num == 0.0) {
        numStr[0] = '0';
        numStr[1] = '.';
        numStr[2] = '0';
        numStr[3] = 'e';
        numStr[4] = '+';
        numStr[5] = '0';
        numStr[6] = '\0';
        return;
    }
    if(num<1){
    	int count=0;
    	while(num<1){
    	    num*=10;
    	    count++;
    	}
    	int intPart=(int)num;
    	int length=0;
    	numStr[length]=intPart+'0';
    	length++;
    	numStr[length]='.';
    	length++;
    	double fracPart = num - intPart;
    	int percision=6;
    	while(percision>0){
    	    fracPart*=10;
    	    int digit=(int)fracPart;
    	    numStr[length]=digit+'0';
    	    length++;
    	    fracPart-=digit;
    	    percision--;
    	}
    	numStr[length]='e';
    	length++;
    	numStr[length]='-';
    	length++;
    	char countStr[32];
    	itos(countStr,count,10);
    	for(int j=0;countStr[j];j++){
    	    numStr[length]=countStr[j];
    	    length++;
    	}
    	
    	numStr[length]= '\0';
    }
    else{
    	int intPart=(int)num;
    	char intStr[32];
    	itos(intStr,intPart,10);
    	int i;
    	int length=0;
    	for(i=0;intStr[i];i++){
    	    numStr[length]=intStr[i];
    	    length++;
    	    if(i==0){
    	    	numStr[length++]='.';
    	    }
    	}
    	double fracPart = num - intPart; 
    	int percision=3;
    	while(percision>0){
    	    fracPart*=10;
    	    int digit=(int)fracPart;
    	    numStr[length]=digit+'0';
    	    length++;
    	    fracPart-=digit;
    	    percision--;
    	}
    	numStr[length]='e';
    	length++;
    	numStr[length]='+';
    	length++;
    	numStr[length]=i-1+'0';   	
    	length++;
    	numStr[length]='\0';
    }
}
