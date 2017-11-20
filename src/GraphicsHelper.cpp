/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GraphicsHelper.cpp
 * Author: petrstepanov
 * 
 * Created on August 9, 2017, 10:54 PM
 */

#include "GraphicsHelper.h"
#include <TColor.h>
#include <TPolyLine.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TLine.h>
#include <TImage.h>

GraphicsHelper::GraphicsHelper() {
}

GraphicsHelper::GraphicsHelper(const GraphicsHelper& orig) {
}

GraphicsHelper::~GraphicsHelper() {
}
//
//Int_t* GraphicsHelper::getBirdPalette(void){
//    Int_t myPalette[255] = new Int(255);
//    Double_t stops[9] = { 0.0000, 0.1250, 0.2500, 0.3750, 0.5000, 0.6250, 0.7500, 0.8750, 1.0000 };
//
//    Double_t red[9] = { 0.2082, 0.0592, 0.0780, 0.0232, 0.1802, 0.5301, 0.8186, 0.9956, 0.9764 };
//    Double_t green[9] = { 0.1664, 0.3599, 0.5041, 0.6419, 0.7178, 0.7492, 0.7328, 0.7862, 0.9832 };
//    Double_t blue[9] = { 0.5293, 0.8684, 0.8385, 0.7914, 0.6425, 0.4662, 0.3499, 0.1968, 0.0539 };
//    Int_t FI = TColor::CreateGradientColorTable(9, stops, red, green, blue, 255);
//    for (Int_t i = 0; i < 255; i++) myPalette[i] = FI + i;
//    return myPalette;
//}

void GraphicsHelper::drawHistSumRegion(Double_t e1Min, Double_t e1Max, Double_t e2Min, Double_t e2Max, Double_t e1_0, Double_t e2_0, Double_t dEplus, Double_t dEminus, Bool_t isRotated){
    // Draw sum region
//    e1_0 = e1_0 > 250 ? 511: 0;
//    e2_0 = e2_0 > 250 ? 511: 0;

    if (isRotated){
        Double_t x[7] = { e1Min,
            e1Min,
            e1Max,
            e1Max,
            e1Min };
        Double_t y[7] = { e2_0 - dEminus/sqrt(2.),
            e2_0 + dEplus / sqrt(2.),
            e2_0 + dEplus / sqrt(2.),
            e2_0 - dEminus / sqrt(2.),
            e2_0 - dEminus / sqrt(2.) };
        TPolyLine *pline = new TPolyLine(5, x, y);
        // pline->SetFillColorAlpha(kOrange, 0.5); // set "OpenGL.CanvasPreferGL" to 1 in $ROOTSYS/etc/system.rootrc
        pline->SetUniqueID(101);
        pline->SetFillColor(kOrange);
        pline->SetFillStyle(3001);
        pline->Draw("f");
    }
    else {
        Double_t x[7] = { e2_0 + e1_0 - dEminus - e2Min,
                        e1Min,
                        e1Min,
                        -e2Max + e1_0 + e2_0 + dEplus,
                        e1Max,
                        e1Max,
                        e2_0 + e1_0 - dEminus - e2Min};
        Double_t y[7] = { e2Min,
                        -e1Min + e2_0 + e1_0 - dEminus,
                        e2Max,
                        e2Max,
                        -e1Max + e1_0 + e2_0 + dEplus,
                        e2Min,
                        e2Min};
        TPolyLine *pline = new TPolyLine(7, x, y);
        // pline->SetFillColorAlpha(kOrange, 0.5); // set "OpenGL.CanvasPreferGL" to 1 in $ROOTSYS/etc/system.rootrc
        pline->SetUniqueID(101);
        pline->SetFillColor(kOrange);
        pline->SetFillStyle(3001);
        pline->Draw("f");
    }
}

TPadPair GraphicsHelper::drawHist3D(TH2* hist, TCanvas* canvas, TF2* tf2, Bool_t setLogZ, Double_t e1Min, Double_t e1Max, Double_t e2Min, Double_t e2Max, Double_t e1_0, Double_t e2_0, Double_t dEplus, Double_t dEminus, Bool_t doRegion, Bool_t isRotated, const char* drawOption){
	TAxis* xAxis = hist->GetXaxis();
	TAxis* yAxis = hist->GetYaxis();
	TAxis* zAxis = hist->GetZaxis();

	// Set Pallette - in ROOT6 use kBird
	gStyle->SetPalette(kBird); // ROOT 6

	// Draw Histogram
	// canvas->Clear();
	canvas->cd();
	TPad *pad3D = new TPad("pad3D", "Pad 3D", 0.0, 0.0, 0.5, 1.0, kWhite); // Double_t xlow, Double_t ylow, Double_t xup, Double_t yup
	TPad *pad2D = new TPad("pad2D", "Pad 2D", 0.5, 0.0, 1.0, 1.0, kWhite);
	pad2D->Draw();
	// pad2D->Clear();
	pad3D->Draw();
	// pad3D->Clear();

	// Draw 3D Pad
	pad3D->cd();
	pad3D->SetRightMargin(0.05);
	pad3D->SetLeftMargin(0.06);
	pad3D->SetTopMargin(0.04);

	// Set Z axis limits and scale
	if (setLogZ) pad3D->SetLogz();

	// Get histogram maximum in central region
	// Double_t histMaxValue = hist->GetBinContent(hist->GetMaximumBin());
	Double_t histMaxValue = 0;
	for (int i = xAxis->GetNbins() / 3; i < xAxis->GetNbins() * 2 / 3; i++){
		for (int j = yAxis->GetNbins() / 3; j < yAxis->GetNbins() * 2 / 3; j++){
			Int_t count = hist->GetBinContent(i, j);
			if (count > histMaxValue){
				histMaxValue = count;
			}
		}
	}
	Double_t histMinCount = hist->GetBinContent(hist->GetMinimumBin());
	Bool_t hasNegativeCounts = histMinCount < 0 ? kTRUE : kFALSE;

	Double_t zAxisMaximum = histMaxValue;
	Double_t zAxisMinimum = 0;
	if (setLogZ) zAxisMinimum = 0.1;
	else if (hasNegativeCounts) zAxisMinimum = histMinCount;

	hist->SetMinimum(zAxisMinimum);
	hist->SetMaximum(zAxisMaximum);

	// Hide Rudimentary Elements
	gStyle->SetStatStyle(0);
	hist->SetStats(0);
	hist->SetTitle("");
	// hist->SetFillColor(kOrange);

	// Draw histogram with corresponding options depending on the gain coarse ()
	hist->Draw(drawOption);

	// Draw fit function if not NULL
	if (tf2 != NULL){
		// Set tf2 axis limits (fit function rescaled if not set same limits)
		tf2->SetMaximum(zAxisMaximum);
		tf2->SetMinimum(zAxisMinimum);

		// Set mesh frequency
		tf2->SetNpx(Int_t(hist->GetXaxis()->GetXmax() - hist->GetXaxis()->GetXmin()) * 2);
		tf2->SetNpy(Int_t(hist->GetYaxis()->GetXmax() - hist->GetYaxis()->GetXmin()) * 2);

		// Set mesh line parameters
		tf2->SetLineWidth(1);
		tf2->SetLineColorAlpha(0, 0.4); // white, 40%

		// Draw fit function
		// TCutG *cutg = new TCutG("cutg", 5);
		// cutg->SetPoint(0, 510, 510);
		// cutg->SetPoint(1, 512, 510);
		// cutg->SetPoint(2, 512, 512);
		// cutg->SetPoint(3, 510, 512);
		// cutg->SetPoint(4, 510, 510);

		tf2->Draw("surf same");
		if (e1Min != 0 && e1Max != 0) tf2->GetXaxis()->SetRangeUser(e1Min, e1Max);
		if (e2Min != 0 && e2Max != 0) tf2->GetYaxis()->SetRangeUser(e2Min, e2Max);
	}
	else {
		std::cout << "no fit function provided" << std::endl;
	}

	// Adjust Axis
	if (e1Min != 0 && e1Max != 0) xAxis->SetRangeUser(e1Min, e1Max);
	xAxis->SetTitle("1st Analyzer Energy, keV");
	xAxis->CenterTitle();
	xAxis->SetTitleOffset(titleOffset);
	xAxis->SetTitleSize(labelSize*1.3);
	xAxis->SetLabelSize(labelSize);

	if (e2Min != 0 && e2Max != 0) yAxis->SetRangeUser(e2Min, e2Max);
	yAxis->SetTitle("2nd Analyzer Energy, keV");
	yAxis->CenterTitle();
	yAxis->SetTitleOffset(titleOffset);
	yAxis->SetTitleSize(labelSize*1.3);
	yAxis->SetLabelSize(labelSize);

	zAxis->SetLabelSize(labelSize);

	// Draw 2D Pad
	pad2D->cd();
	if (setLogZ) pad2D->SetLogz();
	pad2D->SetRightMargin(0.1);
	pad2D->SetLeftMargin(0.16);
	pad2D->SetTopMargin(0.1);
	pad2D->SetBottomMargin(0.16);

	hist->Draw("COLZ 0");                 // "0" - empty Binz are NOT drawn

	// Draw background fit (optional)
	if (tf2 != NULL){
		tf2->SetContour(35);
		tf2->Draw("same");
	}

	// Draw crosshair at the bin with maximum count
	TLine *line1 = new TLine(e1_0 - 1, e2_0, e1_0 + 1, e2_0);
	line1->SetLineColor(kBlack);
	line1->SetLineWidth(1);
	line1->Draw();
	TLine *line2 = new TLine(e1_0, e2_0 - 1, e1_0, e2_0 + 1);
	line2->SetLineColor(kBlack);
	line2->SetLineWidth(1);
	line2->Draw();

	if (doRegion){
//            Double_t e511 = e1_0 > 250 ? 511 : 0;    
            drawHistSumRegion(e1Min, e1Max, e2Min, e2Max, e1_0, e2_0, dEplus, dEminus, isRotated);
        }
	// Update pads
	pad2D->Modified();
	pad3D->Modified();
	pad2D->Update();
	pad3D->Update();

	TPadPair p;
	p.first = pad3D;
	p.second = pad2D;
	return p;
}

void GraphicsHelper::drawHist(TH1* hist, TCanvas* c, Bool_t setLogZ, Double_t min, Double_t max){
	TAxis* xAxis = hist->GetXaxis();
	TAxis* yAxis = hist->GetYaxis();
	//TCanvas *c = new TCanvas(name, "canvas1d", 1024, 600);
	c->cd();
	// c->Clear();
	if (setLogZ) c->SetLogy();
	c->SetMargin(0.07, 0.03, 0.15, 0.05);

	// Set Limits
	if (min != 0) hist->SetMinimum(min);
	if (max != 0) hist->SetMaximum(max);

	xAxis->SetTitle("Doppler shift, keV");
	xAxis->CenterTitle();
	xAxis->SetTitleOffset(titleOffset);
	xAxis->SetTitleSize(labelSize*1.3);
	xAxis->SetLabelSize(labelSize);

	yAxis->SetTitleOffset(titleOffset);
	yAxis->SetTitleSize(labelSize*1.3);
	yAxis->SetLabelSize(labelSize);

	hist->SetTitle("");
	hist->SetStats(0);
	hist->Draw();
	c->Modified();
	c->Update();
}

void GraphicsHelper::saveCanvasToImage(TCanvas* c, TString* filename){
	std::cout << "Saving \"" << c->GetName() << "\" to " << filename->Data() << std::endl;
	TImage *imgCanvas = TImage::Create();
//        c->cd();
	imgCanvas->FromPad(c);
	imgCanvas->WriteImage(filename->Data());
}