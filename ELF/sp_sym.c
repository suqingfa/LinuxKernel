#include <stdio.h>

extern char __executable_start[];
extern char etext[];
extern char edata[];
extern char end[];

void main(){
	printf("__executable_start : %x\n", __executable_start);
	printf("etext : %x\n", etext);
	printf("edata : %x\n", edata);
	printf("end : %x\n", end);
}