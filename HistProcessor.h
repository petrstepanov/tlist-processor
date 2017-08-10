/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   HistProcessor.h
 * Author: petrstepanov
 *
 * Created on August 9, 2017, 10:11 PM
 */

#ifndef HISTPROCESSOR_H
#define HISTPROCESSOR_H

#include <TH1.h>
#include <TH2.h>
#include "FittingFunctions.h"
#include <TMath.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Geometry.h"
#include "AppSettings.h"

enum ConversionGain {
	GAIN_LOW,     // 8192  Channels
	GAIN_MEDIUM,  // 16384 Channels
	GAIN_HIGH     // 32768 Channels
};

typedef std::pair<TH1*, TH1*> TH1Pair;

class HistProcessor {
public:
    static HistProcessor* getInstance();
    ConversionGain getConversionGain(TH2* hist);    
    Double_t getHiSleeveMaxCount(TH2* hist, Bool_t isRotated);
    Double_t getLowSleeveMaxCount(TH2* hist, Bool_t isRotated);
    TH2* subtractFitFromHist(TH2* h, Double_t *par, Bool_t doRotate);
    TH1Pair projectHist(TH2* hist, Double_t e1Mean, Double_t e2Mean, Double_t dEplus, Double_t dEminus, Bool_t doRegion);
    TH1Pair projectHistRotated(TH2* hist, Double_t e1Mean, Double_t e2Mean, Double_t dEplus, Double_t dEminus, Bool_t doRegion);
    TH2* readSpectrumFromFile(TString* filename);
    void saveHistToFile(TH1*, const char*);
    
private:
    HistProcessor();                                      // Private so that it can  not be called
    HistProcessor(HistProcessor const&);                  // Copy constructor is private
    HistProcessor& operator=(HistProcessor const&);       // Assignment operator is private
    static HistProcessor* instance;
};

#endif /* HISTPROCESSOR_H */

