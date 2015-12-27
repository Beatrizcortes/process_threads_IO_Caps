#include <stdio.h>
#include <string.h>
#include "processes_strings.h"

void split (char all[SIZE], char half1[SIZE_HALF], char half2[SIZE_HALF]){
	int a;

	a = strlen(all) / 2;

	strncpy(half1, all, a);
	half1[a] = '\0';

	strcpy(half2, all+a);
}

void concatenate (char all[SIZE], char half1[SIZE_HALF], char half2[SIZE_HALF]){
	strcpy(all, half1);
	strcat(all, half2);
}
