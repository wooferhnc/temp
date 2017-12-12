#include <stdio.h>

#define KGRN "\x1B[32m"
#define KNRM "\x1B[0m"
int main(){
	printf("%sgreen\n", KGRN);
	printf("%sKGRM\n", KNRM);
	return 0;
}
