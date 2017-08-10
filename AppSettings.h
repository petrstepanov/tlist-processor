/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AppSettings.h
 * Author: petrstepanov
 *
 * Created on July 28, 2017, 7:45 PM
 */

#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <TString.h>

class AppSettings {
	//FilteringMethod filteringMethod;
	TString* strFilename;
	static AppSettings *s_instance;
	AppSettings(){}

public:
	//FilteringMethod getFilteringMethod(){
	//	return filteringMethod;
	//}

	//void setFilteringMethod(FilteringMethod method){
	//	filteringMethod = method;
	//}

	TString* getFilename();

	void setFilename(TString*);

	static AppSettings *instance();
};

#endif /* APPSETTINGS_H */

