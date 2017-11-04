/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StringUtils.cpp
 * Author: petrstepanov
 * 
 * Created on August 9, 2017, 9:40 PM
 */

#include "StringUtils.h"

StringUtils::StringUtils() {
}

StringUtils::StringUtils(const StringUtils& orig) {
}

StringUtils::~StringUtils() {
}

TString* StringUtils::stripFileName(TString* path){
	std::string p(path->Data());
	std::size_t found = p.find_last_of("/\\");
	return new TString(p.substr(found + 1).c_str());
}

TString* StringUtils::stripFileExtension(TString* path){
	std::string p(path->Data());
	std::size_t found = p.find_last_of(".");
	return new TString(p.substr(0, found).c_str());
}