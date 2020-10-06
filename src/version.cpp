#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include "version.h"

version::version(std::string inp){
	build = inp;
}
version::version(){}

bool version::compare(version comp){
	std::vector<int> local = this->split();
	std::vector<int> remote = comp.split();
	std::size_t size = local.size();
	if(size > remote.size()){
		size = remote.size();
	};
	for(int i = 0; i < size; i++){
		if(remote[i] > local[i]){
			return true;
		}else if(local[i] > remote[i]){
			return false;
		};
	};
	return false;
}

/**
 * Split the version string into its int values
 *
 * @return int array or version values
 *
 * TODO: Fix this ugly mess.
 */
std::vector<int> version::split(){
	std::size_t count = std::count(build.begin(), build.end(), '.');
	std::vector<int> version;
	std::size_t prev = 0;
	std::size_t current = build.find('.');
	std::string current_v;
	for(int i = 0; i < count; i++){
		current_v = build.substr(prev, current);
		current_v.erase(std::remove(current_v.begin(), current_v.end(), '.'), current_v.end());
		version.push_back(std::stoi(current_v));
		prev = current;
		current = build.find('.', current+1);
	}
	// Get last
	current_v = build.substr(prev, build.length());
	current_v.erase(std::remove(current_v.begin(), current_v.end(), '.'), current_v.end());
	version.push_back(std::stoi(current_v));
	return version;
}
