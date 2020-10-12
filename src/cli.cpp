#include <iostream>
#include <stdio.h>
#include <cstring>


int main(int argc, char **argv){
	switch(sizeof(argv)){
		case 1 :
			if (strcmp(argv[1], "test") == 0){
			   std::cout << "Testing prog" << std::endl;
			}
		}
	return 0;
}
