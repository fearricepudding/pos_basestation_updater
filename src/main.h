#include "version.h"

#ifndef MAIN_H
#define MAIN_H

class BSUpdater: public cppcms::application{			
public: 
	BSUpdater(cppcms::service &srv);
private: 
	version _localVersion;
	version _latestVersion;
	bool setLocalVersion();
	bool setLatestVersion();
	std::string _baseStationAddress;
	std::string _updateServerAddress;
	void checkForUpdate(); 
	Json::Value GET(std::string requestUrl);
	// Routes
	void status();
};

#endif
