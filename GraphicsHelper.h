/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GraphicsHelper.h
 * Author: petrstepanov
 *
 * Created on August 9, 2017, 10:54 PM
 */

#ifndef GRAPHICSHELPER_H
#define GRAPHICSHELPER_H

#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TF2.h>
#include <utility>
#include <TPad.h>

typedef std::pair<TPad*, TPad*> TPadPair;

class GraphicsHelper {
public:
    GraphicsHelper();
    GraphicsHelper(const GraphicsHelper& orig);
    virtual ~GraphicsHelper();
      
    static Int_t* getBirdPalette(void);
    static void drawHistSumRegion(Double_t e1Min = 0, Double_t e1Max = 0, Double_t e2Min = 0, Double_t e2Max = 0, Double_t e1_0 = 0, Double_t e2_0 = 0, Double_t dEplus = 2.4, Double_t dEminus = 2.4, Bool_t isRotated = kFALSE);
    static TPadPair drawHist3D(TH2* hist, TCanvas* canvas, TF2* tf2 = NULL, Bool_t setLogZ = kFALSE, Double_t e1Min = 0, Double_t e1Max = 0, Double_t e2Min = 0, Double_t e2Max = 0, Double_t e1_0 = 0, Double_t e2_0 = 0, Double_t dEplus = 2.4, Double_t dEminus = 2.4, Bool_t doRegion = kTRUE, Bool_t isRotated = kFALSE, const char* drawOption = "SURF2");
    static void drawHist(TH1*, TCanvas*, Bool_t setLogZ = kFALSE, Double_t min = 0, Double_t max = 0);
    static void saveCanvasToImage(TCanvas* c, TString* filename);
    
private:

    static constexpr Double_t titleOffset = 2.2;
    static constexpr Double_t labelSize = 0.02;
};

#endif /* GRAPHICSHELPER_H */

