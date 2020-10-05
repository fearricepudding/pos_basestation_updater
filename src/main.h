
class BSUpdater: public cppcms::application{			
public: 
	BSUpdater(cppcms::service &srv);
private: 
	void status();
	void checkForUpdate();
	Json::Value GET(std::string requestUrl);
};
