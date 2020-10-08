#include <vector>

#ifndef VERSION_H
#define VERSION_H

class version{
public:
	std::string build;
	bool set = false;
	version(std::string build);
	version();
	bool compare(version comp);
	std::vector<int> split();
};

#endif
