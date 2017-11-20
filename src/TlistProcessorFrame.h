/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TlistProcessorFrame.h
 * Author: petrstepanov
 *
 * Created on July 28, 2017, 7:49 PM
 */

#ifndef TLISTPROCESSORFRAME_H
#define TLISTPROCESSORFRAME_H

#include <TGFrame.h>
#include <TGNumberEntry.h>
#include <TGButton.h>
#include <TGTextEdit.h>
#include <TGLabel.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TRootEmbeddedCanvas.h>
#include <TH2.h>
#include <TF2.h>
#include <TString.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TStopwatch.h>
#include <TPaveText.h>
#include <TGMsgBox.h>
#include "GraphicsHelper.h"
#include "StringUtils.h"

typedef std::pair<TPad*, TPad*> TPadPair;
typedef std::pair<TH1*, TH1*> TH1Pair;

class TlistProcessorFrame : public TGMainFrame {
private:
//    TGMainFrame* mainFrame;
    TGNumberEntry* numberMinusEnergy;
    TGNumberEntry* numberPlusEnergy;
    TGNumberEntry* numberResolutionFWHM;
    TGNumberEntry* fLynx1Emin;
    TGNumberEntry* fLynx1Emax;
    TGNumberEntry* iLynx1Bins;
    TGNumberEntry* fLynx2Emin;
    TGNumberEntry* fLynx2Emax;
    TGNumberEntry* iLynx2Bins;
    TGNumberEntry* numberDisplayMin;
    TGNumberEntry* numberDisplayMax;
    TGButton* buttonProcessSpectrum;
    TGButton* fRedrawButton;
    TGButton* fExportImagesButton;
    TGButton* fSaveSpectrumButton;
    TGLabel* labelFilename;
    TGComboBox* comboDrawOption;
    //TGRadioButton* optionSubtractBackground;
    //TGRadioButton* optionCutDiagonal;
    TGCheckButton* checkboxRotate;
    TGCheckButton* checkboxRelative;
    TGCheckButton* checkboxSubtractBackground;
//    TGCheckButton* checkboxSqrt2;
    TGCheckButton* checkboxEnergyRegion;
    TGTab* tabsWithCanvases;
    TRootEmbeddedCanvas* canvasFullSpectrum;
    TRootEmbeddedCanvas* canvasSubtractedSpectrum;
    TRootEmbeddedCanvas* canvasBinCounts;
    TRootEmbeddedCanvas* canvasDopplerSpectrum;
    Int_t myPalette[255];
    Int_t defaultDisplayOffset;
    // Switch to settings
    Double_t e1Mean;
    Double_t e2Mean;
    Double_t dEplus;
    Double_t dEminus;
    Bool_t isRotated;
    Bool_t isRelative;
    Bool_t subtractBackground;
    Bool_t doRegion;

    TH2* hist;
    TH1* histDoppler;
    TH1* histSumNumber;
    TH2* histProject;

    TF2* bgf2;
    TF2* peakf2;
    TPadPair padsPair1;
    TPadPair padsPair2;

public:
    TlistProcessorFrame(const TGWindow*, UInt_t, UInt_t);
    void openFileButtonClicked(void);
    void processSpectrum(void);
    void displayMinNumberChanged();
    void displayMaxNumberChanged();
    void updateAxisRange();
    void exportImagesClicked();
    void saveSpectrumClicked();
    void ShowOkDialog(const char*, const char*);
    void setButtonsEnabled(Bool_t);
    void onCheckboxEnergyRegionClicked();
    TString* getBaseOutputFilename();
    void tryToClose();
    void closeWindow();
    virtual ~TlistProcessorFrame();

    ClassDef(TlistProcessorFrame, 0);
};

#endif /* TLISTPROCESSORFRAME_H */
