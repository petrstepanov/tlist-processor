/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AppSettings.cpp
 * Author: petrstepanov
 * 
 * Created on July 28, 2017, 7:45 PM
 */

#include "AppSettings.h"

TString* AppSettings::getFilename(){
	return strFilename;
}

void AppSettings::setFilename(TString* str){
	strFilename = str;
}

AppSettings* AppSettings::instance(){
	if (!s_instance){
		s_instance = new AppSettings();
	}
	return s_instance;
}

AppSettings *AppSettings::s_instance = 0;

