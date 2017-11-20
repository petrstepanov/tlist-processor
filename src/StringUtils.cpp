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

std::istream& StringUtils::safeGetline(std::istream& is, std::string& t){
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}