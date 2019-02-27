/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Constants.h
 * Author: petrstepanov
 *
 * Created on October 29, 2017, 7:34 PM
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <TROOT.h>
#include <TColor.h>
#include <string>

class Constants {
  public:   
    static const char* APPLICATION_NAME;
    static char* APPLICATION_ICON[];
    static const UInt_t WINDOW_WIDTH;
    static const UInt_t WINDOW_HEIGHT;

    static const std::string drawOptions[2];
};

#endif /* CONSTANTS_H */

