/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FittingFunctions.cpp
 * Author: petrstepanov
 * 
 * Created on August 9, 2017, 10:19 PM
 */

#include <TMath.h>
#include <TF1.h>
#include <TF1Convolution.h>

Double_t FWHMtoSigma =  1/TMath::Sqrt(8*TMath::Log(2)); // 0.424661

Double_t myResolution(Double_t* x, Double_t* par){
    Double_t e = x[0];
    Double_t fwhm = par[0];
    Double_t sigma = fwhm * 1/TMath::Sqrt(8*TMath::Log(2));
    return TMath::Gaus(e, 0, fwhm * sigma, kTRUE);
}

Double_t myRidgeProfile(Double_t *x, Double_t* par){
	Float_t e = x[0];

	Double_t countHiE  = par[0];
	Double_t countLowE = par[1];
	Double_t lowExpContrib = par[2];
	Double_t lowExpStretch = par[3];
	Double_t hiExpContrib  = par[4];
//	Double_t hiExpStretch  = par[5];
	Double_t mc2 = par[6];

	// Coincidence with Compton scattered gamma-ray
	Double_t ridge = countHiE;

	// Three-gamma annihilation contribution
	if (e < mc2){
            ridge += countLowE;
            ridge += TMath::Exp((e-mc2)/lowExpStretch) * countLowE * lowExpContrib;
            // ridge += orePowell(e, mc2) * threeGammaInt;
	}
	if (e >= mc2){
		ridge += TMath::Exp(-(e-mc2)/lowExpStretch) * countHiE * hiExpContrib;
	}
	// Exponential tail due to pile-up,
//        if (e >= mc2){
//                ridge += exp(-(e-mc2)/hiExpStretch) * hiExpContrib * comptonInt;
//        }

	// Exponential tail due to a ballistic deficit and pulse shaping problems
//        if (e < mc2){
//                ridge += exp((e-mc2)/lowExpStretch) * lowExpContrib * (threeGammaInt + comptonInt);
//        }
	return ridge;
}

void initializeConvolution(Double_t eMin, Double_t eMax){
	TF1* resolution = new TF1("resolution",myResolution, -10, 10, 1);
	TF1* ridge = new TF1("ridge",myRidgeProfile, eMin, eMax, 6);
	TF1Convolution* ridgeGausConv = new TF1Convolution(ridge, resolution);
	ridgeGausConv->SetNofPointsFFT((int)eMax - eMin);
	TF1* ridgeGausConvFunc = new TF1("ridgeGausConvFunc", *ridgeGausConv, ridgeGausConv->GetXmin(), ridgeGausConv->GetXmax(), ridgeGausConv->GetNpar());
}

//class FittingContext {
//	public:
//		static FittingContext* getInstance();
//		TF1* gauss;
//		TF1* ridge;
//		TF1Convolution* ridgeGausConv;
//		TF1* ridgeGausConvFunc;
//
//	private:
//		FittingContext();
//		static FittingContext* instance;
//};
//
//FittingContext::FittingContext(){
//	gauss = new TF1("gauss","FittingFunctions::gaus", -10, 10, 1);
//	ridge = new TF1("ridge","FittingFunctions::ridgeProfile", 411, 611, 6);
//	ridgeGausConv = new TF1Convolution(ridge, gauss);
//	ridgeGausConv->SetNofPointsFFT(1000);
//	ridgeGausConvFunc = new TF1("ridgeGausConvFunc", *ridgeGausConv, ridgeGausConv->GetXmin(), ridgeGausConv->GetXmax(), ridgeGausConv->GetNpar());
//};
//
//FittingContext* FittingContext::instance = NULL;
//
//FittingContext* FittingContext::getInstance(){
//    if (!instance){
//        instance = new FittingContext;
//    }
//    return instance;
//}

//Double_t gausE1(Double_t* x, Double_t* par){
//    Double_t fwhm = par[0];
//    Double_t fwhmToSigma = par[1];
//    Double_t e1 = x[0];
//	return TMath::Gaus(e1, 0, fwhm * fwhmToSigma, kTRUE);
//}
//
//Double_t gausE2(Double_t* x, Double_t* par){
//    Double_t fwhm = par[0];
//    Double_t fwhmToSigma = par[1];
//    Double_t e2 = x[0];
//	return TMath::Gaus(e2, 0, fwhm * fwhmToSigma, kTRUE);
//}

// Custom 2D convolution

Double_t convolutionGauss(Double_t (*f)(Double_t*, Double_t*), Double_t* x, Double_t* par, Double_t convFWHM){
    // Control constants
    Int_t np = 10;          // number of convolution steps
    Double_t sc = 3.0;      // convolution extends to +-sc Gaussian sigmas

    // Range of convolution integrals
    Double_t e1low = x[0] - sc * convFWHM * FWHMtoSigma;
    Double_t e1upp = x[0] + sc * convFWHM * FWHMtoSigma;
    Double_t e2low = x[1] - sc * convFWHM * FWHMtoSigma;
    Double_t e2upp = x[1] + sc * convFWHM * FWHMtoSigma;

    Double_t e1step = (e1upp-e1low) / np;
    Double_t e2step = (e2upp-e2low) / np;

    Double_t sum = 0.0;
    // Convolution integral of Background and Gaussian by sum
    for (Double_t e1=e1low; e1<=e1upp; e1+=e1step) {
        for (Double_t e2=e2low; e2<=e2upp; e2+=e2step) {
            Double_t e[] = {e1, e2};
            sum += (*f)(e, par) * TMath::Gaus(x[0]-e1,0,convFWHM * FWHMtoSigma, kTRUE)*TMath::Gaus(x[1]-e2,0,convFWHM * FWHMtoSigma, kTRUE) * e1step * e2step;
        }
    }
    return sum;
}

Double_t orePowell(Double_t e, Double_t mc2){
	return e*(mc2-e)/pow(2*mc2-e,2) + (2*mc2-e)/e + (2*mc2*(mc2-e)/(e*e) - 2*mc2*pow(mc2-e,2)/pow(2*mc2-e,3))*log(1-e/mc2);
}

Double_t bgfunc(Double_t *x, Double_t *par) {
    Double_t countLowE1 = par[0]; // Ridge count for E1 < 511
    Double_t countHiE1 = par[1];  // Ridge count for E1 > 511
    Double_t countLowE2 = par[2];
    Double_t countHiE2 = par[3];
    Double_t meanE1 = par[4];
    Double_t meanE2 = par[5];
    Double_t armFWHM1 = par[6];
    Double_t armFWHM2 = par[7];
    Double_t secondGaussFraction = par[8];
    Double_t bg1 = par[9];
    Double_t bg24 = par[10];
    Double_t bg3 = par[11];
    Double_t lowExpStretch = par[12];
    Double_t hiExpStretch = par[13];
    Double_t lowExpContrib = par[14];
    Double_t hiExpContrib = par[15];
    // Double_t FWHM1 = par[16];
    // Double_t FWHM2 = par[17];
    // Double_t FWHM3 = par[18];
    // Double_t spectFWHM = par[19];
    // Double_t maxCount = par[20];
    // Double_t g1Int = par[21];
    // Double_t g2Int = par[22];
    Double_t xMin = par[23];
    Double_t xMax = par[24];
    Double_t yMin = par[25];
    Double_t yMax = par[26];
    Int_t fitRegion = par[27];
    Double_t resolutionFWHM = par[28];
    Bool_t isRotated = par[31];
    Bool_t isRelative = par[32];
    
    Double_t e1 = x[0], e2 = x[1];
    Double_t mc2E1 = isRelative ? meanE1 : 511;
    Double_t mc2E2 = isRelative ? meanE2 : 511;

    // Fit region 
    Int_t borderWidth = 2;
    Int_t spectHalfWidth = 8;
    switch(fitRegion){
        case 1: {   // only sides 
            if (e1 > xMin + borderWidth && e1 < xMax - borderWidth && e2 > yMin + borderWidth && e2 < yMax - borderWidth){
                    TF1::RejectPoint();
                    // return 0;
            }
            if (e2 < -e1 + mc2E1 + mc2E2 - spectHalfWidth && e2 > -e1 + mc2E1 + mc2E2 + spectHalfWidth){
                    TF1::RejectPoint();
                    // return 0;
            }            
            break;
        }
        case 2: {   //all but the peak
            if (e2 < -e1 + mc2E1 + mc2E2 + spectHalfWidth && e2 > -e1 + mc2E1 + mc2E2 - spectHalfWidth - 1.5){
                    TF1::RejectPoint();
//                     return 0;
            }
            break;
        }
        default: break;
    }

    if (gROOT->GetFunction("ridgeGausConvFunc") == NULL){
    	initializeConvolution(TMath::Min(xMin, yMin), TMath::Max(xMax, yMax));
    }
    TF1* ridgeGausConvFunc = (TF1*) gROOT->GetFunction("ridgeGausConvFunc");

    // Ridge along E1
    // In TF1Convolution the parameters of f() and g() are joined
    // Double_t ridgePar[7] = {countHiE1, countLowE1, lowExpContrib, lowExpStretch, hiExpContrib, hiExpStretch, mc2E1}; // parameters of ridge
    // Double_t gaussPar[1] = {resolutionFWHM}; // parameters of gauss
    Double_t ridgeGausConvParE1[8] = {countHiE1, countLowE1, lowExpContrib, lowExpStretch, hiExpContrib, hiExpStretch, mc2E1, resolutionFWHM};
    ridgeGausConvFunc->SetParameters(ridgeGausConvParE1);

    // Multiply convoluted ridge profile on perpendicular two gausses
    Double_t ridgeAlongE1 = ridgeGausConvFunc->Eval(e1) * (TMath::Gaus(e2, meanE2, armFWHM1 * FWHMtoSigma, kTRUE)*(1 - secondGaussFraction) + TMath::Gaus(e2, meanE2, armFWHM2 * FWHMtoSigma, kTRUE)*secondGaussFraction);

    // Ridge along E2
    Double_t ridgeGausConvParE2[8] = {countHiE2, countLowE2, lowExpContrib, lowExpStretch, hiExpContrib, hiExpStretch, mc2E2, resolutionFWHM};
    ridgeGausConvFunc->SetParameters(ridgeGausConvParE2);

    Double_t ridgeAlongE2 = ridgeGausConvFunc->Eval(e2) * (TMath::Gaus(e1, meanE1, armFWHM1 * FWHMtoSigma, kTRUE)*(1 - secondGaussFraction) + TMath::Gaus(e1, meanE1, armFWHM2 * FWHMtoSigma, kTRUE)*secondGaussFraction);

    // Sum ridges along E1 and E2

    Double_t returnVal = ridgeAlongE1 + ridgeAlongE2;

    // Descartes Plane 1
    if (e1 >= meanE1 && e2 >= meanE2){
        returnVal += bg1;
    }
    // Descartes Plane 2 & 4
    else if ((e1 >= meanE1 && e2 < meanE2) || (e1 < meanE1 && e2 >= meanE2)){
        returnVal += bg24;
    }
    // Descartes Plane 3
    else {
        returnVal += bg3;
    }

    return returnVal;
}

Double_t bgfuncrotate(Double_t *x, Double_t *par){
    const Int_t cutCenterStripeWidth = 5;
    
    Double_t countLowE1 = par[0]; // Ridge count for E1 < 511
    Double_t countHiE1 = par[1];  // Ridge count for E1 > 511
    Double_t countLowE2 = par[2];
    Double_t countHiE2 = par[3];
    Double_t meanE1 = par[4];
    Double_t meanE2 = par[5];
    Double_t armFWHM1 = par[6];
    Double_t armFWHM2 = par[7];
    Double_t secondGaussFraction = par[8];
    Double_t bg1 = par[9];
    Double_t bg24 = par[10];
    Double_t bg3 = par[11];
    Double_t lowExpStretch = par[12];
    Double_t hiExpStretch = par[13];
    Double_t lowExpContrib = par[14];
    Double_t hiExpContrib = par[15];
    // Double_t FWHM1 = par[16];
    // Double_t FWHM2 = par[17];
    // Double_t FWHM3 = par[18];
    // Double_t spectFWHM = par[19];
    // Double_t maxCount = par[20];
    // Double_t g1Int = par[21];
    // Double_t g2Int = par[22];
    Double_t xMin = par[23];
    Double_t xMax = par[24];
    Double_t yMin = par[25];
    Double_t yMax = par[26];
    Int_t fitRegion = par[27];
    Double_t resolutionFWHM = par[28];
    
    Double_t _x = x[0], _y = x[1];
    Double_t mc2 = meanE1 > 250 ? 511 : 0;
    
    // Cut artefacts on the histogram borders (like on Fe spectrum)
    if (_x < xMin || _x > xMax){
            TF1::RejectPoint();
            // return 0;
    }
    if (_y < yMin || _y > yMax){
            TF1::RejectPoint();
            // return 0;
    }    
    
    Int_t borderWidth = 2;
    Int_t spectHalfWidth = 5;
    switch(fitRegion){
        case 1: {   // only sides 
            Double_t r = (xMin-mc2)*TMath::Sqrt2()*4/5;
            if ((_x-mc2)*(_x-mc2) + (_y-mc2)*(_y-mc2) < r*r){
                    TF1::RejectPoint();
                    // return 0;
            } 
            break;
        }
        case 2: {   //all but the peak
            if (_y > -spectHalfWidth/TMath::Sqrt2() && _y < spectHalfWidth/TMath::Sqrt2()){
                    TF1::RejectPoint();
                    // return 0;
            }   
            break;
        }
        default: break;
    }

    // return rotated bg function
    Double_t x_rotate[2];
    Double_t theta = -0.785398; // 45 degrees clockwise rotated
    x_rotate[0] = x[0] * cos(theta) - x[1] * sin(theta);
    x_rotate[1] = x[0] * sin(theta) + x[1] * cos(theta);
    _x = x_rotate[0];
    _y = x_rotate[1];
    // Ridge profiles
    Double_t ridge1Par[7] = {countHiE1, countLowE1, lowExpContrib, lowExpStretch, hiExpContrib, hiExpStretch, mc2};
    Double_t ridgeAlongE1 = (TMath::Gaus(_y, meanE2, armFWHM1 * FWHMtoSigma, kTRUE)*(1 - secondGaussFraction) + TMath::Gaus(_y, meanE2, armFWHM2 * FWHMtoSigma, kTRUE)*secondGaussFraction);
//                          * ridgeProfile(_x, ridge1Par);
//                          * convolutedRidgeCache(_x, ridge1Par, resolutionFWHM);
                            //* convolutionGauss(ridgeProfile, _x, ridge1Par, resolutionFWHM);

    Double_t ridge2Par[7] = {countHiE2, countLowE2, lowExpContrib, lowExpStretch, hiExpContrib, hiExpStretch, mc2};
    Double_t ridgeAlongE2 = (TMath::Gaus(_x, meanE1, armFWHM1 * FWHMtoSigma, kTRUE)*(1 - secondGaussFraction) + TMath::Gaus(_x, meanE1, armFWHM2 * FWHMtoSigma, kTRUE)*secondGaussFraction);
//                          * ridgeProfile(_y, ridge2Par);
//                          * convolutedRidgeCache(_y, ridge2Par, resolutionFWHM);
                            //* convolutionGauss(ridgeProfile, _y, ridge2Par, resolutionFWHM);
    Double_t returnVal = ridgeAlongE1 + ridgeAlongE2;

    // Descartes Plane 1
    if (_x >= meanE1 && _y >= meanE2){
        returnVal += bg1;
    }
    // Descartes Plane 2 & 4
    else if ((_x >= meanE1 && _y < meanE2) || (_x < meanE1 && _y >= meanE2)){
        returnVal += bg24;
    }
    // Descartes Plane 3
    else {
        returnVal += bg3;
    }

    return returnVal;     
}

Double_t spectfunc(Double_t *x, Double_t *par){
    Double_t countLowE1 = par[0];
    Double_t countHiE1 = par[1];
    Double_t countLowE2 = par[2];
    Double_t countHiE2 = par[3];
    Double_t meanE1 = par[4];
    Double_t meanE2 = par[5];
    Double_t armFWHM1 = par[6];
    Double_t armFWHM2 = par[7];
    Double_t secondGaussFraction = par[8];
    Double_t bg1 = par[9];
    Double_t bg24 = par[10];
    Double_t bg3 = par[11];
    Double_t lowExpStretch = par[12];
    Double_t hiExpStretch = par[13];
    Double_t lowExpContrib = par[14];
    Double_t hiExpContrib = par[15];
    Double_t FWHM1 = par[16];
    Double_t FWHM2 = par[17];
    Double_t FWHM3 = par[18];
    Double_t spectFWHM = par[19];
    Double_t maxCount = par[20];
    Double_t g1Int = par[21];
    Double_t g2Int = par[22];
    Double_t xMin = par[23];
    Double_t xMax = par[24];
    Double_t yMin = par[25];
    Double_t yMax = par[26];
    Int_t fitRegion = par[27];
    Double_t spectFWHM2 = par[28];
    Double_t spectG1Int = par[29];

    // Add background
    Double_t returnVal = bgfunc(x, par);

    if (fitRegion != 1){
        // Add annihilation peak
        Double_t x_rotate[2];
        Double_t theta = 0.785398; // 45 degrees clockwise rotated
        x_rotate[0] = (x[0]-meanE1) * cos(theta) - (x[1]-meanE2) * sin(theta);
        x_rotate[1] = (x[0]-meanE1) * sin(theta) + (x[1]-meanE2) * cos(theta);
        Double_t _x = meanE1 + x_rotate[0];
        Double_t _y = meanE2 + x_rotate[1];

        returnVal += (g1Int*TMath::Gaus(_x, meanE1, FWHM1 / 2.3548, kTRUE) + (1-g1Int)*(g2Int*TMath::Gaus(_x, meanE1, FWHM2 / 2.3548, kTRUE) + (1-g2Int)*TMath::Gaus(_x, meanE1, FWHM3 / 2.3548, kTRUE)))
                   * (spectG1Int*TMath::Gaus(_y, meanE2, spectFWHM / 2.3548, kTRUE) + (1-spectG1Int)*TMath::Gaus(_y, meanE2, spectFWHM2 / 2.3548, kTRUE)) * maxCount;
    }

    return returnVal;
}

Double_t peakfunc(Double_t *x, Double_t *par){
	Double_t meanE1 = par[0];
	Double_t meanE2 = par[1];
	Double_t spectFWHM = par[2];
	Double_t FWHMres = par[3];
	Double_t maxCount = par[4];
//	Double_t center = (meanE1 < 200) ? 0 : 510.7;

	// Exclude points from fit
//	if ((x[0]-center)*(x[0]-center) + (x[1]-center)*(x[1]-center) > 2*2){
//		TF1::RejectPoint();
//		// return 0;
//	}
//	if (x[1]<-x[0]+2*center-1.5 || x[1]>-x[0]+2*center+1.5){
//		TF1::RejectPoint();
//		// return 0;
//	}
	if ((x[0]-meanE1)*(x[0]-meanE1) + (x[1]-meanE2)*(x[1]-meanE2) > 2*2){
		TF1::RejectPoint();
		// return 0;
	}
	if (x[1]<-x[0]+meanE1+meanE2-1.5 || x[1]>-x[0]+meanE1+meanE2+1.5){
		TF1::RejectPoint();
		// return 0;
	}

	// Add annihilation peak
	Double_t x_rotate[2];
	Double_t theta = -0.785398; // 45 degrees clockwise rotated
	x_rotate[0] = (x[0]-meanE1) * cos(theta) - (x[1]-meanE2) * sin(theta);
	x_rotate[1] = (x[0]-meanE1) * sin(theta) + (x[1]-meanE2) * cos(theta);
	Double_t _x = meanE1 + x_rotate[0];
	Double_t _y = meanE2 + x_rotate[1];

	return TMath::Gaus(_x, meanE1, FWHMres / 2.3548) * TMath::Gaus(_y, meanE2, spectFWHM / 2.3548) * maxCount;
}

Double_t peakfuncrotate(Double_t *x, Double_t *par){
	Double_t meanE1 = par[0];
	Double_t meanE2 = par[1];
	Double_t spectFWHM = par[2];
	Double_t FWHMres = par[3];
	Double_t maxCount = par[4];
	Double_t center = (meanE1 < 200) ? 0 : 510.7;

	// Exclude points from fit
	if ((x[0]-center)*(x[0]-center) + (x[1]-center)*(x[1]-center) > 2*2){
		TF1::RejectPoint();
		// return 0;
	}
	if (x[1]<-x[0]+2*center-1.5 || x[1]>-x[0]+2*center+1.5){
		TF1::RejectPoint();
		// return 0;
	}

	// Add annihilation peak
	Double_t _x = meanE1 + x[0];
	Double_t _y = meanE2 + x[1];
	return TMath::Gaus(_y, meanE2, FWHMres / 2.3548) * TMath::Gaus(_x, meanE1, spectFWHM / 2.3548) * maxCount;
}
