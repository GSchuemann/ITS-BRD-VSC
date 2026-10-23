#include <stdio.h>

typedef struct{
    int test;
    char c;
    char d;
        char e;
    char f;
        char x;

    int test2;
} Test;
int main(){

    int result = 0;
    int result1 = 0;
    printf("Struct: %d\n ",sizeof(Test));
    printf("%d Memory 1: %d\n",&result,&result1);
    // printf("%s\n","");

    // return 0;
}