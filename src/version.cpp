#include <stdio.h>
#include <string>
#include <iostream>

#include "version.h"

version::version(std::string build){
	build = build;
}

bool version::compare(version comp){
	std::cout << comp.build << std::endl;
}
