
class BSUpdater: public cppcms::application{			
public: 
	BSUpdater(cppcms::service &srv);
private: 
	std::string _baseStationAddress;
	std::string _updateServerAddress;
	void status();
	bool checkForUpdate();
	Json::Value GET(std::string requestUrl);
};
