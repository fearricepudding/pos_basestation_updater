
#include <cppcms/application.h>
#include <cppcms/service.h>
#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <cppcms/applications_pool.h>
#include <iostream>
#include <stdlib.h>
#include "main.h"

BSUpdater::BSUpdater(cppcms::service &srv): cppcms::application(srv){
	dispatcher().assign("", &BSUpdater::status, this);

}

void BSUpdater::status(){
	response().out() << "BaseStationUpdater V2";
}

void BSUpdater::checkForUpdate(){
	
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
