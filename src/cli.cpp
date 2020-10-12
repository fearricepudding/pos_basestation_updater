#include <iostream>
#include <stdio.h>
#include <cstring>

void printHelp(){
	std::cout << "Help page" << std::endl;
}

void tester(){
	std::cout << "Testing page" << std::endl;
}

int main(int argc, char **argv){
	switch(argc){
		case 2 : 
			if(strcmp(argv[1], "test")){
				tester();
				break;
			}else{
				break;
			}
		default: 
			printHelp();
			break;
		}
	return 0;
}
