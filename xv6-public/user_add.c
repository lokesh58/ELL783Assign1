#include "types.h"
#include "user.h"

int main(int argc, char *argv[]){
	if(argc < 3) {
		printf(1, "Provide two integers\n");
	}else{
		int a = atoi(argv[1]), b = atoi(argv[2]);
		printf(1, "Sum of %d and %d is %d\n", a, b, add(a,b));
	}
	exit();
}
