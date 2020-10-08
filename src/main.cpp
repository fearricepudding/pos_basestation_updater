
#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <json/json.h>
#include <vector>

#include "main.h"
#include "version.h"

std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out){
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

BSUpdater::BSUpdater(cppcms::service &srv): cppcms::application(srv){
	dispatcher().assign("", &BSUpdater::status, this);
//	dispatcher().assign("/test", &BSUpdater::test, this);
	// XXX: Maybe check BS config for address and port incase of change.
	_updateServerAddress = "localhost:8081";
	_baseStationAddress = "http://clarkeandcoproperty.co.uk/version.php";
}

void BSUpdater::status(){
	response().out() << "{\"version\":\"2.32.122\"}";
}

bool BSUpdater::checkForUpdate(){
	version localVersion;
	version latestVersion;
	try{
		Json::Value localVersionResponse = GET(_updateServerAddress);
		localVersion.build = localVersionResponse["version"].asString();
		response().out() << "Local version: " << localVersion.build << "<br />";
	} catch (Json::Exception const&) {
		std::cout << "Json error" << std::endl;
		response().out() << "Failed to get local BaseStation version, is it running?";
	}
	try{
		Json::Value latestVersionResponse = GET(_baseStationAddress);
		latestVersion.build = latestVersionResponse["version"].asString();
		response().out() << "Latest version: " << latestVersion.build << "<br />";
	} catch (Json::Exception const&) {
		std::cout << "Json error" << std::endl;
		response().out() << "Failed to get latest version from server, are you online?";
	}
//	bool test = localVersion.compare(latestVersion);
//	if(test){
//		response().out() << "Outdated";
//	}else{
//		response().out() << "Up to date";
//	}
	return localVersion.compare(latestVersion);
}

/**
 * HTTP GET request from url
 *
 * @param std::string URL to get json data from
 * @return Json::Value response json or bool on fail
 */
Json::Value BSUpdater::GET(std::string requestUrl){
	const std::string url(requestUrl);
	CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    long httpCode(0);
    std::unique_ptr<std::string> httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
    // Perform curl
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);
    if(httpCode == 200){
	    Json::Value jsonData;
	    Json::Reader jsonReader;
	    if(jsonReader.parse(*httpData.get(), jsonData)){
	        return jsonData;
	    }else{
	        std::cout << "Could not parse HTTP data as JSON" << std::endl;
//	        std::cout << "HTTP data was:\n" << *httpData.get() << std::endl;
	    }
    }else{
        std::cout << "Couldn't GET from " << url << " - exiting" << std::endl;
    }
    return "";
}

int main(int argc,char ** argv){
    try{
        cppcms::service app(argc,argv);
        app.applications_pool().mount(cppcms::applications_factory<BSUpdater>());
        app.run();
    }catch(std::exception const &e){
        std::cerr<<e.what()<<std::endl;
    };
}
