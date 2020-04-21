/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   TlistProcessorFrame.cpp
 * Author: petrstepanov
 *
 * Created on July 28, 2017, 7:49 PM
 */

#include <TApplication.h>
#include <TGFileDialog.h>
#include <TG3DLine.h>
#include <TColor.h>
#include <TPolyLine.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TColor.h>
#include <TLine.h>
#include <TImage.h>
#include <TLatex.h>
#include <TGWindow.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <TFitResult.h>
#include "AppSettings.h"
#include "Constants.h"
#include "Utils.h"
#include "HistProcessor.h"
#include "TlistProcessorFrame.h"

// ClassImp(TlistProcessorFrame)

extern Double_t bgfunc(Double_t *x, Double_t *par);
extern Double_t bgfuncrotate(Double_t *x, Double_t *par);
extern Double_t peakfunc(Double_t *x, Double_t *par);
extern Double_t peakfuncrotate(Double_t *x, Double_t *par);

TlistProcessorFrame::TlistProcessorFrame(const TGWindow* p) :
		TGMainFrame(p, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT) {
	SetCleanup(kDeepCleanup);

	Connect("CloseWindow()", "TlistProcessorFrame", this, "doCloseWindow()");
	DontCallClose(); // avoid double deletions

	defaultDisplayOffset = 15;
	int dx = 10;
//	UInt_t canvasWidth = 1000;
//	UInt_t canvasHeight = 500;

	// Toolbars
	TGHorizontalFrame *frameTopBar = new TGHorizontalFrame(this);
	TGHorizontalFrame *frameMiddleBar = new TGHorizontalFrame(this);
	TGHorizontalFrame *frameBottomBar = new TGHorizontalFrame(this);

	// TOP TOOLBAR
	// Open File Button
	TGTextButton *fOpenFileButton = new TGTextButton(frameTopBar, "Open File");
	fOpenFileButton->Connect("Clicked()", "TlistProcessorFrame", this, "openFileButtonClicked()");
	frameTopBar->AddFrame(fOpenFileButton, new TGLayoutHints(kLHintsLeft | kLHintsTop));

	// Rotation checkbox
	checkboxRotate = new TGCheckButton(frameTopBar, "45 degree rotation");
	frameTopBar->AddFrame(checkboxRotate, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2 * dx, 0, 3, 0));

	// Rotation checkbox
	checkboxRelative = new TGCheckButton(frameTopBar, "Relative peak");
	frameTopBar->AddFrame(checkboxRelative, new TGLayoutHints(kLHintsLeft | kLHintsTop, 2 * dx, 2 * dx, 3, 0));

	// Filename
	labelFilename = new TGLabel(frameTopBar, "");
	labelFilename->SetTextJustify(kTextRight | kTextTop);
	frameTopBar->AddFrame(labelFilename, new TGLayoutHints(kLHintsLeft | kLHintsExpandX, dx, 0, 3, 0));

	this->AddFrame(frameTopBar, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, dx, dx, dx, dx));

	// Separator
	this->AddFrame(new TGHorizontal3DLine(this), new TGLayoutHints(kLHintsExpandX, dx, dx));

	// MIDDLE TOOLBAR
	// Project button
	buttonProcessSpectrum = new TGTextButton(frameMiddleBar, "Process Spectrum");
	buttonProcessSpectrum->Connect("Clicked()", "TlistProcessorFrame", this, "processSpectrum()");
	frameMiddleBar->AddFrame(buttonProcessSpectrum, new TGLayoutHints(kLHintsRight | kLHintsTop, 0, 0, 0, 0));

	// Projection Range
	checkboxEnergyRegion = new TGCheckButton(frameMiddleBar, "Cut energy region, keV");
	checkboxEnergyRegion->Connect("Clicked()", "TlistProcessorFrame", this, "onCheckboxEnergyRegionClicked()");
	checkboxEnergyRegion->SetOn();
	// TGLabel* labelEnergyRegion = new TGLabel(frameMiddleBar, "Energy region, keV:   -");
	numberMinusEnergy = new TGNumberEntry(frameMiddleBar, -3.3, 4, -1, TGNumberFormat::kNESRealOne,
			TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMax, 0);

	numberPlusEnergy = new TGNumberEntry(frameMiddleBar, 1.7, 3, -1, TGNumberFormat::kNESRealOne,
			TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMin, 0);
	frameMiddleBar->AddFrame(numberPlusEnergy, new TGLayoutHints(kLHintsRight | kLHintsTop, 0, 2 * dx, 1, 0));

	{
		Int_t colorIndex = 9999;
		TColor *c = new TColor(colorIndex, 0.906, 0.906, 0.906); // grey canvas background color
		TRootEmbeddedCanvas* trec = new TRootEmbeddedCanvas("trec", frameMiddleBar, 113, 20, 0);
		trec->GetCanvas()->cd();
		trec->GetCanvas()->SetFillColor(colorIndex);
		TLatex* latex = new TLatex();
		Double_t fontnumber = 4; // helvetica-medium-r-normal  "Arial"
		Int_t precision = 3;     // Text size is given in pixels
		latex->SetTextFont(10 * fontnumber + precision); // precision 2, font number 4 ()
		latex->SetTextSize(14);
		latex->SetTextAlign(13);  //align at top
		latex->DrawLatex(0, 0.9, "< E_{1} + E_{2} #minus 2mc^{2} <");
		frameMiddleBar->AddFrame(trec, new TGLayoutHints(kLHintsRight | kLHintsTop, 0, 0, 1));
		trec->GetCanvas()->SetEditable(kFALSE);
	}

	frameMiddleBar->AddFrame(numberMinusEnergy, new TGLayoutHints(kLHintsRight, 0, dx, 1, 0));
	frameMiddleBar->AddFrame(checkboxEnergyRegion, new TGLayoutHints(kLHintsRight, 0, 0, 3, 0));

	// Checkbox for sqrt(2.) normalization
//	checkboxSqrt2 = new TGCheckButton(frameMiddleBar, "Spectrum is sqrt(2) scaled");
//	frameMiddleBar->AddFrame(checkboxSqrt2, new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 3 * dx, 3, 0));

	// Background checkbox
	checkboxSubtractBackground = new TGCheckButton(frameMiddleBar, "Fit and subtract background");
	frameMiddleBar->AddFrame(checkboxSubtractBackground, new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, dx, 3, 0));
	// checkboxSubtractBackground->SetOn();
	checkboxSubtractBackground->Connect("Clicked()", "TlistProcessorFrame", this,
			"checkboxSubtractBackgroundClicked()");

	// Resolution frame
	resolutionFrame = new TGHorizontalFrame(frameMiddleBar);
	resolutionFrame->AddFrame(new TGLabel(resolutionFrame, "Detector FWHM, keV"), new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, dx, 4));
	numberResolutionFWHM = new TGNumberEntry(resolutionFrame, 1.0, 3, -1, TGNumberFormat::kNESRealOne, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMin, 0);
	resolutionFrame->AddFrame(numberResolutionFWHM, new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, dx));

	frameMiddleBar->AddFrame(resolutionFrame, new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, dx));

	// Add Bars
	this->AddFrame(frameMiddleBar, new TGLayoutHints(kLHintsExpandX | kLHintsTop, dx, dx, dx, dx));

	// tabsWithCanvases
	tabsWithCanvases = new TGTab(this);

	// Full Spectrum Tab
	TGCompositeFrame *tab2DFull = tabsWithCanvases->AddTab("Original 2D Spectrum");
	canvasFullSpectrum = new TRootEmbeddedCanvas("canvasFullSpectrum", tab2DFull);
	tab2DFull->AddFrame(canvasFullSpectrum,
			new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY, dx, dx, dx, dx));

	// Cut Spectrum Tab
	TGCompositeFrame *tab2DCut = tabsWithCanvases->AddTab("Subtracted Background");
	canvasSubtractedSpectrum = new TRootEmbeddedCanvas("canvasSubtractedSpectrum", tab2DCut);
	tab2DCut->AddFrame(canvasSubtractedSpectrum,
			new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY, dx, dx, dx, dx));

	// Bin Counts Tab
	TGCompositeFrame *tabBinCounts = tabsWithCanvases->AddTab("Bin Counts in Doppler Spectrum");
	canvasBinCounts = new TRootEmbeddedCanvas("canvasBinCounts", tabBinCounts);
	tabBinCounts->AddFrame(canvasBinCounts,
			new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY, dx, dx, dx, dx));

	// Doppler Spectrum Tab
	TGCompositeFrame *tabDopplerSpectrum = tabsWithCanvases->AddTab("Dopler Spectrum");
	canvasDopplerSpectrum = new TRootEmbeddedCanvas("canvasDopplerSpectrum", tabDopplerSpectrum);
	tabDopplerSpectrum->AddFrame(canvasDopplerSpectrum,
			new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY, dx, dx, dx, dx));

	// Display Range Container
	//TGHorizontalFrame *frameBottomBar = new TGHorizontalFrame(mainFrame);

	// BOTTOM TOOLBAR
	// Draw Option
	TGLabel* labelDrawOption = new TGLabel(frameBottomBar, "Histogram draw option");
	frameBottomBar->AddFrame(labelDrawOption, new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, dx, 3, 0));
	comboDrawOption = new TGComboBox(frameBottomBar, 0);
	frameBottomBar->AddFrame(comboDrawOption, new TGLayoutHints(kLHintsLeft | kLHintsTop, 0, 3 * dx));
	Int_t i = 0;
	std::for_each(std::begin(Constants::drawOptions), std::end(Constants::drawOptions), [&](std::string const &str) {
		comboDrawOption->AddEntry(str.c_str(),++i);
	});
	comboDrawOption->Select(1);
	comboDrawOption->Resize(75, 20);

	TGLabel* fDisplayLabel = new TGLabel(frameBottomBar, "Display Range");
	fDisplayLabel->SetTextJustify(kTextLeft);
	numberDisplayMin = new TGNumberEntry(frameBottomBar, 511 - defaultDisplayOffset, 3, -1, TGNumberFormat::kNESInteger,
			TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMinMax, 0, 510);
	numberDisplayMin->GetNumberEntry()->Connect("TextChanged(char*)", "TlistProcessorFrame", this,
			"displayMinNumberChanged()");
	TGLabel *lblFitDash = new TGLabel(frameBottomBar, "-");
	numberDisplayMax = new TGNumberEntry(frameBottomBar, 511 + defaultDisplayOffset, 3, -1, TGNumberFormat::kNESInteger,
			TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMinMax, 512, 1024);
	numberDisplayMin->GetNumberEntry()->Connect("TextChanged(char*)", "TlistProcessorFrame", this,
			"displayMaxNumberChanged()");
	fRedrawButton = new TGTextButton(frameBottomBar, "Update Display Range");
	fRedrawButton->Connect("Clicked()", "TlistProcessorFrame", this, "updateAxisRange()");

	fExportImagesButton = new TGTextButton(frameBottomBar, "Export Images");
	fExportImagesButton->Connect("Clicked()", "TlistProcessorFrame", this, "exportImagesClicked()");

	fSaveSpectrumButton = new TGTextButton(frameBottomBar, "Save Doppler Spectrum");
	fSaveSpectrumButton->Connect("Clicked()", "TlistProcessorFrame", this, "saveSpectrumClicked()");

	setButtonsEnabled(kFALSE);

	frameBottomBar->AddFrame(fDisplayLabel, new TGLayoutHints(kLHintsNormal, 0, dx, 3, 0));
	frameBottomBar->AddFrame(numberDisplayMin, new TGLayoutHints(kLHintsLeft));
	frameBottomBar->AddFrame(lblFitDash, new TGLayoutHints(kLHintsLeft, dx, dx, 3, 0));
	frameBottomBar->AddFrame(numberDisplayMax, new TGLayoutHints(kLHintsLeft));
	frameBottomBar->AddFrame(fRedrawButton, new TGLayoutHints(kLHintsLeft, dx));
	frameBottomBar->AddFrame(fSaveSpectrumButton, new TGLayoutHints(kLHintsRight, dx));
	frameBottomBar->AddFrame(fExportImagesButton, new TGLayoutHints(kLHintsRight, dx));

	this->AddFrame(tabsWithCanvases,
			new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY, dx, dx, 0, dx));
	this->AddFrame(frameBottomBar, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, dx, dx, 0, dx));

	// End UI
	SetWindowName(Constants::APPLICATION_NAME);
	SetIconPixmap(Constants::APPLICATION_ICON);
	MapSubwindows();
	Layout();
	Resize(GetDefaultSize());
	MapWindow();

	// TODO: think if this functionality is needed
	frameTopBar->HideFrame(checkboxRelative);
	frameTopBar->HideFrame(checkboxRotate);

	frameMiddleBar->HideFrame(resolutionFrame);
}

void TlistProcessorFrame::setButtonsEnabled(Bool_t isEnabled) {
	buttonProcessSpectrum->SetEnabled(isEnabled);
	fRedrawButton->SetEnabled(isEnabled);
	fExportImagesButton->SetEnabled(isEnabled);
	fSaveSpectrumButton->SetEnabled(isEnabled);
}

void TlistProcessorFrame::displayMinNumberChanged() {
	// Int_t fitMin = numberDisplayMin->GetIntNumber();
	// numberDisplayMax->SetIntNumber(511 + (511 - fitMin));
}
void TlistProcessorFrame::displayMaxNumberChanged() {
	// Int_t fitMax = numberDisplayMax->GetIntNumber();
	// numberDisplayMin->SetIntNumber(511 - (fitMax - 511));
}

void TlistProcessorFrame::openFileButtonClicked(void) {
	// show file dialog
	TGFileInfo fi;
	const char *filetypes[] = { "All files", "*", "Data files", "*.dat", "CSV files", "*.csv", "Text files",
			"*.[tT][xX][tT]", 0, 0 };

	fi.fFileTypes = filetypes;
	fi.fIniDir = StrDup("./");
	TGFileDialog* tgfd = new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);

	// when obtained the filename - process the file
	if ((fi.fFilename != NULL) && (strlen(fi.fFilename) > 0)) {
		TString* fileName = new TString(fi.fFilename);
		labelFilename->SetText(fileName->Data());
		buttonProcessSpectrum->SetEnabled(kTRUE);
		AppSettings::instance()->setFilename(fileName);
	}
}

void TlistProcessorFrame::processSpectrum(void) {
	// Start tracking time
	TStopwatch watch;
	watch.Start();

	// Disable some user interface elements while calculation is in progress
	setButtonsEnabled(kFALSE);

	// Disable background tab if needed
	tabsWithCanvases->SetEnabled(1, checkboxSubtractBackground->IsOn());

	// Draw "Processing spectrum..." text on the main canvas
	canvasFullSpectrum->GetCanvas()->Clear("");
	canvasFullSpectrum->GetCanvas()->cd();
	// No Border text https://root.cern.ch/doc/v608/classTPaveText.html#a3462ca2759db1ece8d858b5a48e4cc89
	TPaveText *paveText = new TPaveText(.2, .4, .8, .6, "NB"); // "NB" = No Border
	paveText->AddText("Processing spectrum...");
	paveText->SetTextSize(0.075);
	paveText->SetTextColor(kGray);
	paveText->SetFillColor(kWhite);
	paveText->Draw();
	canvasFullSpectrum->GetCanvas()->Modified();
	canvasFullSpectrum->GetCanvas()->Update();

	// Read 2D histogram from file
	TString* strFileName = AppSettings::instance()->getFilename();
	// hist = readSpectrumFromFile(strFileName);
	// hist = readSpectrumFromFileBinned(strFileName);  // Alternative file reading procedure
	HistProcessor* histProcessor = HistProcessor::getInstance();
	hist = histProcessor->readSpectrumFromFile(strFileName);

	// Crude calculation of the peak coordinates (based on the bin with maximum count)
	// DoublePair peakCoordinates = getHistMaxBinCoordinate(hist);

//	if (hist->GetXaxis()->GetXmin() < 10 && hist->GetXaxis()->GetXmax() > 10){
//		e1Mean = 0; //peakCoordinates.first;
//		e2Mean = 0; //peakCoordinates.second;
//	}
//	else {
//		e1Mean = 510.72;
//		e2Mean = 510.72;
//	}
	Int_t maxBinX, maxBinY, maxBinZ;
	hist->GetMaximumBin(maxBinX, maxBinY, maxBinZ);
	e1Mean = hist->GetXaxis()->GetBinCenter(maxBinX);
	e2Mean = hist->GetYaxis()->GetBinCenter(maxBinY);

	std::cout << "Histogram maximum bin at (" << e1Mean << ", " << e2Mean << ")" << std::endl;

	TAxis* xAxis = hist->GetXaxis();
	TAxis* yAxis = hist->GetYaxis();

	isRotated = checkboxRotate->IsOn();
	isRelative = checkboxRelative->IsOn();
	subtractBackground = checkboxSubtractBackground->IsOn();
	doRegion = checkboxEnergyRegion->IsOn();

	// TROOT* gROOT = gClient->GetRoot();

	// If subtract background checkbox is on
	if (subtractBackground) {
		// Get maximum counts in arms
		std::cout << "Calculating maximum count in arms." << std::endl;
		const Double_t hiSleeveMaxCount = histProcessor->getHiSleeveMaxCount(hist, isRotated);
		const Double_t lowSleeveMaxCount = histProcessor->getLowSleeveMaxCount(hist, isRotated);
		std::cout << "Hi: " << hiSleeveMaxCount << std::endl;
		std::cout << "Lo: " << lowSleeveMaxCount << std::endl;

		// Two extra parameters will be a histogram width and height.
		// It is important to correct fitting of the bad spectra with artefacts on the sides
		Double_t histXmin = hist->GetXaxis()->GetXmin();
		Double_t histXmax = hist->GetXaxis()->GetXmax();
		Double_t histYmin = hist->GetYaxis()->GetXmin();
		Double_t histYmax = hist->GetYaxis()->GetXmax();

		// Fix Tomsk problems on the sides
		if (isRotated) {
			Double_t d = 1;
			histXmin += d;
			histXmax -= d;
			histYmin += d;
			histYmax -= d;
		}
		Double_t fwhm = numberResolutionFWHM->GetNumber();
		fwhm = Utils::roundToOneDecimal(numberResolutionFWHM->GetNumber());

		// Define background function parameter starting values
		const int npar = 33;
		Double_t f2params[npar] = { hiSleeveMaxCount,					// 0) threeGammaIntE1
				lowSleeveMaxCount,					// 1) comptonIntE1
				hiSleeveMaxCount,					// 2) threeGammaIntE2
				lowSleeveMaxCount,					// 3) comptonIntE2
				e1Mean,						// 4)
				e2Mean,						// 5)
				3,							// 6) armFWHM1
				5,							// 7) armFWHM2
				0.02,						// 8) secondGaussFraction
				0.01,						// 9) bg1
				0.04,						// 10) bg24
				0.21,						// 11) bg3
				10,							// 12) lowExpStretch
				10,							// 13) hiExpStretch
				1,                                                  // 14) lowExpContrib
				1,							// 15) hiExpContrib
				2,							// 16) FWHM1
				5,							// 17) FWHM2
				15,							// 18) FWHM3
				2,							// 19) spectFWHM
				hist->GetMaximum(),					// 20) maxCount
				0.8,						// 21) g1Int
				0.8,						// 22) g2Int
				histXmin,                                           // 23)
				histXmax,                                           // 24)
				histYmin,                                           // 25)
				histYmax,                                           // 26)
				2,							// 27) exclude option - 0) full fit; 1) only ridge sides; 2) all but peak
				fwhm,						// 28) resolution FWHM
				5,                                                  // 29) spectFWHM2
				0.8,                                                // 30) spectG1Int
				isRotated ? 1. : 0.,                                  // 31) isRotated
				isRelative ? 1. : 0. };                               // 32) isRelative
		// Define correct background function (rotated or not)
		bgf2 = (TF2*) gROOT->GetListOfFunctions()->FindObject("bgf2");
		if (bgf2)
			delete bgf2;

		bgf2 = isRotated ?
				new TF2("bgf2", bgfuncrotate, xAxis->GetXmin(), xAxis->GetXmax(), yAxis->GetXmin(), yAxis->GetXmax(),
						npar) :
				new TF2("bgf2", bgfunc, xAxis->GetXmin(), xAxis->GetXmax(), yAxis->GetXmin(), yAxis->GetXmax(), npar); // 501-521

		// Set background parameter names and limits
		bgf2->SetParameters(f2params);

		bgf2->SetParName(0, "3GIntE1");              // Higher atan limit along E1 axis
		bgf2->SetParameter(0, hiSleeveMaxCount);
		bgf2->SetParLimits(0, 0, hiSleeveMaxCount * 5);

		bgf2->SetParName(1, "compIntE1");              // Lower atan limit along E1 axis
		bgf2->SetParameter(1, lowSleeveMaxCount);
		bgf2->SetParLimits(1, 0, lowSleeveMaxCount * 5);

		bgf2->SetParName(2, "3GIntE2");              // Higher atan limit along E2 axis
		bgf2->SetParameter(2, hiSleeveMaxCount);
		bgf2->SetParLimits(2, 0, hiSleeveMaxCount * 5);

		bgf2->SetParName(3, "compIntE2");              // Lower atan limit along E2 axis
		bgf2->SetParameter(3, lowSleeveMaxCount);
		bgf2->SetParLimits(3, 0, lowSleeveMaxCount * 2);

		bgf2->SetParName(4, "e1Mean");                    // Peak position on E1 axis
		bgf2->SetParameter(4, e1Mean);
		bgf2->SetParLimits(4, e1Mean - 2, e1Mean + 2);

		bgf2->SetParName(5, "e2Mean");                    // Peak position on E2 axis
		bgf2->SetParameter(5, e2Mean);
		bgf2->SetParLimits(5, e2Mean - 2, e2Mean + 2);

		bgf2->SetParName(6, "armFWHM1");           // 1st gauss width in thick arm profile
		bgf2->SetParameter(6, 3);
		bgf2->SetParLimits(6, 1, 8);

		bgf2->SetParName(7, "armFWHM2");           // 2nd gauss width in thick arm profile
		bgf2->SetParameter(7, 5);
		bgf2->SetParLimits(7, 2, 20);

		bgf2->SetParName(8, "g2Frac");             // 2nd gauss fraction
		bgf2->SetParameter(8, 0.01);
		bgf2->SetParLimits(8, 0, 1);

		bgf2->SetParName(9, "bg1");                     // Descartes quarter I value
		bgf2->SetParameter(9, 0.01);
		bgf2->SetParLimits(9, 0, 0.1);

		bgf2->SetParName(10, "bg24");                    // Descartes quarters II and IV values
		bgf2->SetParameter(10, 0.04);
		bgf2->SetParLimits(10, 0, 5);

		bgf2->SetParName(11, "bg3");                     // Descartes quarter III value
		bgf2->SetParameter(11, 0.21);
		bgf2->SetParLimits(11, 0, 5);

		bgf2->SetParName(12, "loExpStr");            // Atan horizontal stretch coefficient
		bgf2->SetParameter(12, 10);
		bgf2->SetParLimits(12, 1, 100);											// Lo Exp Stretch

		bgf2->SetParName(13, "hiExpStr");            // Atan horizontal stretch coefficient
		bgf2->FixParameter(13, 10);

		bgf2->SetParName(14, "loExpContr");            // Three Gauss Amplitude
		bgf2->SetParameter(14, 1);
		bgf2->SetParLimits(14, 0.01, 20);											// Lo Exp Contribution

		bgf2->SetParName(15, "hiExpContr");            // Three Gauss Amplitude
		bgf2->SetParameter(15, 1);
		bgf2->SetParLimits(15, 0.01, 20);											// Hi Exp Contribution

		bgf2->SetParName(16, "FWHM1");
		bgf2->FixParameter(16, 2);

		bgf2->SetParName(17, "FWHM2");
		bgf2->FixParameter(17, 5);

		bgf2->SetParName(18, "FWHM3");
		bgf2->FixParameter(18, 15);

		bgf2->SetParName(19, "spectFWHM");
		bgf2->FixParameter(19, 2);

		bgf2->SetParName(20, "maxCount");
		bgf2->SetParameter(20, hist->GetMaximum());
		bgf2->SetParLimits(20, hist->GetMaximum() / 1.5, hist->GetMaximum() * 1.5);

		bgf2->SetParName(21, "g1Int");
		bgf2->FixParameter(21, 0.8);

		bgf2->SetParName(22, "g2Int");
		bgf2->FixParameter(22, 0.8);

		bgf2->SetParName(23, "histXmin");
		bgf2->FixParameter(23, histXmin);

		bgf2->SetParName(24, "histXmax");
		bgf2->FixParameter(24, histXmax);

		bgf2->SetParName(25, "histYmin");
		bgf2->FixParameter(25, histYmin);

		bgf2->SetParName(26, "histYmax");
		bgf2->SetParameter(26, histYmax);
		bgf2->SetParLimits(26, histYmax, histYmax);

		bgf2->SetParName(27, "fitRange");
		bgf2->SetParameter(27, 2);  // Set range only sides
//		bgf2->SetParLimits(27, 2, 2);  // Set range only sides

		bgf2->SetParName(28, "resolutionFWHM");
		bgf2->FixParameter(28, fwhm);

		bgf2->SetParName(29, "spectFWHM2");
		bgf2->SetParameter(29, 5);
		bgf2->SetParLimits(29, 1, 10);

		bgf2->SetParName(30, "spectG1I");
		bgf2->SetParameter(30, 0.8);
		bgf2->SetParLimits(30, 1E-4, 1);

		bgf2->SetParName(31, "isRotated");
		bgf2->FixParameter(31, isRotated ? 1 : 0);

		bgf2->SetParName(32, "isRelative");
		bgf2->FixParameter(32, isRelative ? 1 : 0);

		// Clear convoluted ridge profile
//		convolutionCache.clear();

		// Print function parameters
		bgf2->Print("V");

		// Fit histogram with arms
		// "S" The result of the fit is returned in the TFitResultPtr
		// "N" Do not store the graphics function, do not draw
		// "L" Use Loglikelihood method (default is chisquare method) - deals with zero counts
		TFitResultPtr r = hist->Fit("bgf2", "S0L");
		r.Get()->Print();

		// Now we fix first 9 parameters (ridge heights)
		// for (int a = 0; a < 12; a++){
		// 	Double_t parValue = hist->GetFunction("bgf2")->GetParameter(a);
		// 	bgf2->SetParLimits(a, parValue, parValue);
		// }

		// Free Other Parameters

		// Range - whole area
//		bgf2->SetParameter(25, 0);  // Set full range

		// And fit again
		// r = hist->Fit("bgf2", "S0L");

		// TVirtualFitter* fitter = TVirtualFitter::Fitter(); // TVirtualFitter(hist);
		// fitter->SetMaxIterations (1);
		// TVirtualFitter::SetPrecision(0.001); // default 1e-6
		// fitter->SetPrecision(1e-2); // default 1e-6

		// TFitResultPtr r = *(new TFitResultPtr());
		// Print fit result
		// r->Print("V");

		// Get Fit Parameters
		TF1* myfunc = hist->GetFunction("bgf2");
		Double_t fittedParams[npar];
		for (Int_t k = 0; k < npar; k++) {
			fittedParams[k] = myfunc != NULL ? myfunc->GetParameter(k) : f2params[k];
		}

		// Subtract Fit from histogram
		histProject = histProcessor->subtractFitFromHist(hist, fittedParams, isRotated);

		// Update approximate peak center coordinates (calculated as maximum bin coordinated) with fitted peak center coordinates
		e1Mean = myfunc != NULL ? myfunc->GetParameter(4) : f2params[4];
		e2Mean = myfunc != NULL ? myfunc->GetParameter(5) : f2params[5];
	} else {
		// Fitting peak center
		// TODO: support rotation!
		const int npar_peakfunc = 5;
		// Define peak function parameters:    e1Mean, e2Mean, spectFWHM, FWHMres, Maximum Count
		Double_t peakparams[npar_peakfunc] = { e1Mean, e2Mean, 10, 2, hist->GetMaximum() };

		TF2* peakf2old = (TF2*) gROOT->GetListOfFunctions()->FindObject("peakf2");
		if (peakf2old)
			delete peakf2old;
		peakf2 =
				isRotated ?
						new TF2("peakf2", peakfuncrotate, xAxis->GetXmin(), xAxis->GetXmax(), yAxis->GetXmin(),
								yAxis->GetXmax(), npar_peakfunc) :
						new TF2("peakf2", peakfunc, xAxis->GetXmin(), xAxis->GetXmax(), yAxis->GetXmin(),
								yAxis->GetXmax(), npar_peakfunc);

		peakf2->SetParameters(peakparams);
		peakf2->SetParName(0, "e1_mean");              // Higher atan limit along E1 axis
		peakf2->SetParLimits(0, e1Mean - 2, e1Mean + 2);
		peakf2->SetParName(1, "e2_mean");              // Lower atan limit along E1 axis
		peakf2->SetParLimits(1, e2Mean - 2, e2Mean + 2);
		peakf2->SetParName(2, "spectFWHM");              // Higher atan limit along E2 axis
		peakf2->SetParLimits(2, 2, 10);
		peakf2->SetParName(3, "FWHMres");              // Lower atan limit along E2 axis
		peakf2->SetParLimits(3, 0.5, 10);
		peakf2->SetParName(4, "histMax");              // Lower atan limit along E2 axis
		peakf2->SetParLimits(4, hist->GetMaximum() / 10, hist->GetMaximum() * 10);

		// Fit histogram with arms
		// "S" The result of the fit is returned in the TFitResultPtr
		// "N" Do not store the graphics function, do not draw
		// "L" Use Loglikelihood method (default is chisquare method) - deals with zero counts
		TFitResultPtr peak_res = hist->Fit("peakf2", "S0L");
		// Update approximate peak center coordinates (calculated as maximum bin coordinated) with fitted peak center coordinates
		TF1* mypeakfunc = hist->GetFunction("peakf2");
		e1Mean = mypeakfunc->GetParameter(0);
		e2Mean = mypeakfunc->GetParameter(1);

		std::cout << "method: cut and project" << std::endl;
		histProject = (TH2F*) hist->Clone("histCopy");
	}

	std::cout << "Position of the spectrum center: [" << e1Mean << ", " << e2Mean << "]" << std::endl;

	// Projection energy range: 2mc - dEminus < E1+E2 < 2mc + dEplus
	dEplus = numberPlusEnergy->GetNumber();
	dEminus = (-1) * numberMinusEnergy->GetNumber();

	Double_t e1MeanProject, e2MeanProject;
	if (isRelative) {
		e1MeanProject = e1Mean;
		e2MeanProject = e2Mean;
	} else {
		e1MeanProject = e1Mean > 250 ? 511 : 0;
		e2MeanProject = e2Mean > 250 ? 511 : 0;
	}

	std::cout << "Position of the projection center: [" << e1MeanProject << ", " << e2MeanProject << "]" << std::endl;

	// Project 2D histogram and make Doppler spectrum
	// Here we actually get two 1D spectra - 1) Doppler spectrum 2) How many bins projected in every Doppler histogram bin
	TH1Pair hists =
			isRotated ?
					histProcessor->projectHistRotated(histProject, e1MeanProject, e2MeanProject, dEplus, dEminus,
							doRegion) :
					histProcessor->projectHist(histProject, e1MeanProject, e2MeanProject, dEplus, dEminus, doRegion);
	histDoppler = hists.first;
	histSumNumber = hists.second;

	// Variables to limit the display range textbox values
	Double_t displayMin = std::max(hist->GetXaxis()->GetXmin(), hist->GetYaxis()->GetXmin());
	Double_t displayMax = std::min(hist->GetXaxis()->GetXmax(), hist->GetYaxis()->GetXmax());
	Int_t center = (displayMin + displayMax + 0.5) / 2;

	// Update display region if it is incorrect
	if (numberDisplayMin->GetNumber() > center || numberDisplayMax->GetNumber() < center) {
		numberDisplayMin->SetLimits(TGNumberFormat::kNELLimitMinMax, displayMin, center);
		numberDisplayMax->SetLimits(TGNumberFormat::kNELLimitMinMax, center, displayMax);
		numberDisplayMin->SetNumber(center - defaultDisplayOffset);
		numberDisplayMax->SetNumber(center + defaultDisplayOffset);
	} else {
		numberDisplayMin->SetLimits(TGNumberFormat::kNELLimitMinMax, displayMin, center);
		numberDisplayMax->SetLimits(TGNumberFormat::kNELLimitMinMax, center, displayMax);
	}

	// Define displaTGWindowy limit variables
	Double_t displayE1Min = (Double_t) numberDisplayMin->GetIntNumber();
	Double_t displayE2Min = (Double_t) numberDisplayMin->GetIntNumber();
	Double_t displayE1Max = (Double_t) numberDisplayMax->GetIntNumber();
	Double_t displayE2Max = (Double_t) numberDisplayMax->GetIntNumber();

	// Draw histograms on correspondent canvases
	gStyle->SetCanvasPreferGL(kTRUE);

//        const char* drawOption = comboDrawOption->GetSelected()==1 ? "LEGO2" : "SURF2";
	const char* drawOption = Constants::drawOptions[comboDrawOption->GetSelected() - 1].c_str();

	if (subtractBackground) {
		padsPair1 = GraphicsHelper::drawHist3D(hist, canvasFullSpectrum->GetCanvas(), bgf2, kTRUE, displayE1Min,
				displayE1Max, displayE2Min, displayE2Max, e1MeanProject, e2MeanProject, dEplus, dEminus, doRegion,
				isRotated, drawOption);
		padsPair2 = GraphicsHelper::drawHist3D(histProject, canvasSubtractedSpectrum->GetCanvas(), NULL, kTRUE,
				displayE1Min, displayE1Max, displayE2Min, displayE2Max, e1MeanProject, e2MeanProject, dEplus, dEminus,
				doRegion, isRotated, drawOption);
	} else {
		// padsPair1 = drawHist3D(hist, canvasFullSpectrum->GetCanvas(), peakf2, kTRUE, displayE1Min, displayE1Max, displayE2Min, displayE2Max, e1Mean, e2Mean, dEplus, dEminus, isRotated, drawOption);
		padsPair1 = GraphicsHelper::drawHist3D(hist, canvasFullSpectrum->GetCanvas(), NULL, kTRUE, displayE1Min,
				displayE1Max, displayE2Min, displayE2Max, e1MeanProject, e2MeanProject, dEplus, dEminus, doRegion,
				isRotated, drawOption);
	}

	// Draw sum number histogram
	GraphicsHelper::drawHist(histSumNumber, canvasBinCounts->GetCanvas());

	// Draw Doppler histogram
	Double_t max = histDoppler->GetBinContent(histDoppler->GetMaximumBin());
	GraphicsHelper::drawHist(histDoppler, canvasDopplerSpectrum->GetCanvas(), kTRUE, 0.1, max * 2);
	// drawHist(histDoppler, canvasDopplerSpectrum->GetCanvas(), kFALSE, -150, 100);

	// Enable UIoid
	setButtonsEnabled(kTRUE);

	// Print time
	watch.Stop();
	watch.Print();
}

void TlistProcessorFrame::updateAxisRange() {
	// Disable user interface while app redraws graphs
	setButtonsEnabled(kFALSE);

	// Get new graph range
	Double_t min = (Double_t) numberDisplayMin->GetIntNumber();
	Double_t max = (Double_t) numberDisplayMax->GetIntNumber();

	// Set graphs range
	hist->GetXaxis()->SetRangeUser(min, max);
	hist->GetYaxis()->SetRangeUser(min, max);
	histProject->GetXaxis()->SetRangeUser(min, max);
	histProject->GetYaxis()->SetRangeUser(min, max);
	if (subtractBackground) {
		bgf2->GetXaxis()->SetRangeUser(min, max);
		bgf2->GetYaxis()->SetRangeUser(min, max);
	}

	// Mark Pads as modified
	TPad* pad11 = padsPair1.first;
	TPad* pad12 = padsPair1.second;

	Double_t dEplus = numberPlusEnergy->GetNumber();
	Double_t dEminus = numberMinusEnergy->GetNumber();
	Double_t e511 = e1Mean > 250 ? 511 : 0;
	pad12->cd();
	if (doRegion) {
		GraphicsHelper::drawHistSumRegion(min, max, min, max, e511, e511, dEplus, dEminus, isRotated);
	}

	// Update Canvases
	// When you call canvas->Update(), ROOT looks for all subpads that marked as modified and redraws them
	pad11->Modified();
	pad12->Modified();
	TCanvas* canvas1 = canvasFullSpectrum->GetCanvas();
	canvas1->Update();

	// Do same for canvas with subtracted background
	if (subtractBackground) {
		TPad* pad21 = padsPair2.first;
		TPad* pad22 = padsPair2.second;
		pad22->cd();
		if (doRegion) {
			GraphicsHelper::drawHistSumRegion(min, max, min, max, e511, e511, dEplus, dEminus, isRotated);
		}
		pad21->Modified();
		pad22->Modified();
		TCanvas* canvas2 = canvasSubtractedSpectrum->GetCanvas();
		canvas2->Update();
	}

	// Calculations done. Enable user interface elements
	setButtonsEnabled(kTRUE);
}

TString* TlistProcessorFrame::getBaseOutputFilename() {
	TString* s = new TString(AppSettings::instance()->getFilename()->Data());
	s = StringUtils::stripFileExtension(s);
	if (checkboxEnergyRegion->IsOn()) {
		*s += TString::Format("%.1f+%.1f", numberMinusEnergy->GetNumber(), numberPlusEnergy->GetNumber()).Data();
	}
	if (checkboxSubtractBackground->IsOn()) {
		*s += "-no-bkg";
	}
	return s;
}

void TlistProcessorFrame::exportImagesClicked() {
	Int_t currentTab = tabsWithCanvases->GetCurrent();
	std::cout << "Current tab: " << currentTab << std::endl;
	tabsWithCanvases->SetTab(0);
	TString* fileName = getBaseOutputFilename();
	TString fileNameHist = *fileName + "-hist.png";
	GraphicsHelper::saveCanvasToImage(canvasFullSpectrum->GetCanvas(), &fileNameHist);
	if (subtractBackground) {
		tabsWithCanvases->SetTab(1);
		TString fileNameSubtract = *fileName + "-subtract.png";
		GraphicsHelper::saveCanvasToImage(canvasSubtractedSpectrum->GetCanvas(), &fileNameSubtract);
	}
	std::cout << tabsWithCanvases->SetTab(3) << std::endl;
	TString fileNameDoppler = *fileName + "-doppler.png";
	GraphicsHelper::saveCanvasToImage(canvasDopplerSpectrum->GetCanvas(), &fileNameDoppler);
	tabsWithCanvases->SetTab(currentTab);
	ShowOkDialog("TLIST Processor", "Images saved");
}

void TlistProcessorFrame::saveSpectrumClicked() {
	TString* fileName = getBaseOutputFilename();
	std::cout << *fileName << std::endl;
	TString fileNameDoppler = *fileName + "-cdb.txt";
	HistProcessor* histProcessor = HistProcessor::getInstance();
	histProcessor->saveHistToFile(histDoppler, fileNameDoppler.Data());
	ShowOkDialog("TLIST Processor", "Doppler spectrum saved");
}

void TlistProcessorFrame::onCheckboxEnergyRegionClicked() {
	Bool_t doRegion = checkboxEnergyRegion->IsOn();
	numberMinusEnergy->SetState(doRegion);
	numberPlusEnergy->SetState(doRegion);
}

void TlistProcessorFrame::ShowOkDialog(const char* heading, const char* message) {
	int retval;
	this->Disconnect("CloseWindow()");
	this->Connect("CloseWindow()", "TlistProcessorFrame", this, "dontCloseWindow()");
	new TGMsgBox(gClient->GetRoot(), this, heading, message, kMBIconAsterisk, kMBOk, &retval);
	this->Disconnect("CloseWindow()");
	this->Connect("CloseWindow()", "TlistProcessorFrame", this, "doCloseWindow()");
}

TGCompositeFrame* getParentFrame(TGFrame* frame){
	const TGWindow* parentWindow = frame->GetParent();
	TGCompositeFrame* parentFrame = dynamic_cast<TGCompositeFrame*>(const_cast<TGWindow*>(parentWindow));
	return parentFrame;
}

void TlistProcessorFrame::checkboxSubtractBackgroundClicked() {
	TGCompositeFrame* parentFrame = getParentFrame(resolutionFrame);
	if (parentFrame == NULL) return;

	if (checkboxSubtractBackground->IsOn()) {
		parentFrame->ShowFrame(resolutionFrame);
	} else {
		parentFrame->HideFrame(resolutionFrame);
	}
}

TlistProcessorFrame::~TlistProcessorFrame() {
	Cleanup();
}

void TlistProcessorFrame::dontCloseWindow() {
	std::cout << "Can't close the window. Dialog is still open." << std::endl;
}

void TlistProcessorFrame::doCloseWindow() {
	// DeleteWindow();              // to stay in the ROOT session
	gApplication->Terminate();   // to exit and close the ROOT session
}
