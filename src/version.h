#include <vector>

class version{
public:
	std::string build;
	version(std::string build);
	version();
	bool compare(version comp);
	std::vector<int> split();
};
