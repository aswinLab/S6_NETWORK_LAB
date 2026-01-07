#ifndef PALIANDROME_H
#define PALIANDROME_H

#include <stdio.h>
#include <stdlib.h>


int pal_fun(int num){
    int rev = 0;
    int rem = 0;
    int temp = num;

    while( temp != 0){
        rem = temp % 10;
        rev = rev + rem * 10;
        temp = temp / 10;
    }

    if( rev == num ){
        return 1;
    }
    else{
        return -1;
    }
}

#endif