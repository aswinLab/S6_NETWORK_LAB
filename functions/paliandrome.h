#ifndef PALIANDROME_H
#define PALIANDROME_H

#include <stdio.h>
#include <stdlib.h>


char* pal_fun(int num){
    int rev = 0;
    int rem = 0;
    int temp = num;

    while( temp != 0){
        rem = temp % 10;
        rev = rev * 10 + rem; //rev = rev * 10 + rem;
        temp = temp / 10;
    }

    if( rev == num ){
        return "pal";
    }
    else{
        return "not pal";
    }
}

#endif