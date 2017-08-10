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

#include "FittingFunctions.h"
#include <TMath.h>
#include <TF1.h>

//FittingFunctions::FittingFunctions() {
//}
//
//FittingFunctions::FittingFunctions(const FittingFunctions& orig) {
//}
//
//FittingFunctions::~FittingFunctions() {
//}

Double_t convolutionGauss(Double_t (*f)(Double_t, Double_t*), Double_t x, Double_t* par, Double_t convFWHM){
    // Control constants
    Int_t np = 20;          // number of convolution steps
    Double_t sc = 4.0;      // convolution extends to +-sc Gaussian sigmas (5?)
    Double_t FWHMtoSigma =  1./2.3548;

    // Range of convolution integrals
    Double_t eLow = x - sc * convFWHM * FWHMtoSigma;
    Double_t eUpp = x + sc * convFWHM * FWHMtoSigma;

    Double_t step = (eUpp-eLow) / np;

    Double_t sum = 0;
    // Convolution integral of Background and Gaussian by sum
    for (Double_t e = eLow; e <= eUpp; e += step) {
        sum += (*f)(e, par) * TMath::Gaus(x-e,0,convFWHM * FWHMtoSigma, kTRUE) * step;
    }
    return sum;
}

Double_t convolutionGauss(Double_t (*f)(Double_t*, Double_t*), Double_t* x, Double_t* par, Double_t convFWHM){
    // Control constants
    Int_t np = 10;          // number of convolution steps
    Double_t sc = 3.0;      // convolution extends to +-sc Gaussian sigmas
    Double_t FWHMtoSigma =  1./2.3548;

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

Double_t ridgeProfile(Double_t e, Double_t* par){
	Double_t comptonInt    = par[0];
	Double_t threeGammaInt = par[1];
	Double_t lowExpContrib = par[2];
	Double_t lowExpStretch = par[3];
	Double_t hiExpContrib  = par[4];
	Double_t hiExpStretch  = par[5];
	Double_t mc2 = par[6];

	// Coincidence with Compton scattered gamma-ray
	Double_t ridge = comptonInt;

	// Three-gamma annihilation contribution
	if (e < mc2){
		ridge += orePowell(e, mc2) * threeGammaInt;
	}

	// Exponential tail due to pile-up
	if (hiExpContrib > 1E-2){
		if (e >= mc2){
			ridge = exp(-(e-mc2)/hiExpStretch) * hiExpContrib * comptonInt;
		}
	}

	// Exponential tail due to a ballistic deficit and pulse shaping problems
	if (lowExpContrib > 1E-2){
		if (e < mc2){
			ridge = exp((e-mc2)/lowExpStretch) * lowExpContrib * threeGammaInt;
		}
	}

	return ridge;
}

Double_t bgfunc(Double_t *x, Double_t *par) {
    Double_t threeGammaIntE1 = par[0];
    Double_t comptonIntE1 = par[1];
    Double_t threeGammaIntE2 = par[2];
    Double_t comptonIntE2 = par[3];
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
    Double_t histXlength = par[23];
    Double_t histYlength = par[24];
    Double_t region = par[25];

    Double_t _x = x[0], _y = x[1];
    Double_t FWHMtoSigma =  1. / 2.3548;
    Double_t mc2 = meanE1 > 250 ? 511 : 0;

    // Fit region: 1) only sides  2) all but the peak
    switch((int)region){
        case 1: {
            if (x[0] > mc2 - 0.4*histXlength && x[0] < mc2 + 0.4*histXlength && x[1] > mc2 - 0.4*histYlength && x[1] < mc2 + 0.4*histYlength){
                    TF1::RejectPoint();
                    // return 0;
            }
            break;
        }
        case 2: {
            if (x[0] > -x[1] + 2 * mc2 - 9 && x[0] < -x[1] + 2 * mc2 + 8){
                    TF1::RejectPoint();
                    // return 0;
            }
            break;
        }
        default: break;
    }

    // Ridge profiles
    Double_t ridge1Par[7] = {comptonIntE1, threeGammaIntE1, lowExpContrib, lowExpStretch, hiExpContrib, hiExpStretch, meanE1};
    Double_t ridgeAlongE1 = (TMath::Gaus(_y, meanE2, armFWHM1 * FWHMtoSigma, kTRUE)*(1 - secondGaussFraction) + TMath::Gaus(_y, meanE2, armFWHM2 * FWHMtoSigma, kTRUE)*secondGaussFraction)
                          // * ridgeProfile(_x, ridge1Par);
                          * convolutionGauss(ridgeProfile, _x, ridge1Par, 1.5);

    Double_t ridge2Par[7] = {comptonIntE2, threeGammaIntE2, lowExpContrib, lowExpStretch, hiExpContrib, hiExpStretch, meanE2};
    Double_t ridgeAlongE2 = (TMath::Gaus(_x, meanE1, armFWHM1 * FWHMtoSigma, kTRUE)*(1 - secondGaussFraction) + TMath::Gaus(_x, meanE1, armFWHM2 * FWHMtoSigma, kTRUE)*secondGaussFraction)
                          // * ridgeProfile(_y, ridge2Par);
                          * convolutionGauss(ridgeProfile, _y, ridge2Par, 1.5);

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
    Double_t threeGammaIntE1 = par[0];
    Double_t comptonIntE1 = par[1];
    Double_t threeGammaIntE2 = par[2];
    Double_t comptonIntE2 = par[3];
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
    Double_t histXlength = par[23];
    Double_t histYlength = par[24];
    Double_t region = par[25];
    Double_t spectFWHM2 = par[26];
    Double_t spectG1Int = par[27];

    // Add background
    Double_t returnVal = bgfunc(x, par);

    if (region != 1){
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

Double_t bgfuncrotate(Double_t *x, Double_t *par){
    const Int_t cutCenterStripeWidth = 5;
    
    Double_t meanE1 = par[4];
    Double_t center = (meanE1 < 200) ? 0 : 510.7;
    // Exclude horizontal doppler stripe of a width cutCenterStripeWidth
    if (std::abs(x[1]-center) < cutCenterStripeWidth/2){
            TF1::RejectPoint();
            // return 0;
    }
    // Cut artefacts on the histogram borders (like on Fe spectrum)
    Double_t histXlength = par[16];
    Double_t histYlength = par[17];
    // Cut along X axis
    if (std::abs(x[1]-center) > (histYlength/2)*0.95){
            TF1::RejectPoint();
            // return 0;
    }
    // Cut along Y axis
    if (std::abs(x[0]-center) > (histXlength/2)*0.95){
            TF1::RejectPoint();
            // return 0;
    }

    // return rotated bg function
    Double_t x_rotate[2];
    Double_t theta = -0.785398; // 45 degrees clockwise rotated
    x_rotate[0] = x[0] * cos(theta) - x[1] * sin(theta);
    x_rotate[1] = x[0] * sin(theta) + x[1] * cos(theta);
    return bgfunc(x_rotate, par);
}

Double_t peakfunc(Double_t *x, Double_t *par){
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
	Double_t x_rotate[2];
	Double_t theta = -0.785398; // 45 degrees clockwise rotated
	x_rotate[0] = (x[0]-meanE1) * cos(theta) - (x[1]-meanE2) * sin(theta);
	x_rotate[1] = (x[0]-meanE1) * sin(theta) + (x[1]-meanE2) * cos(theta);
	Double_t _x = meanE1 + x_rotate[0];
	Double_t _y = meanE2 + x_rotate[1];

	return TMath::Gaus(_x, meanE1, FWHMres / 2.3548) * TMath::Gaus(_y, meanE2, spectFWHM / 2.3548) * maxCount;
}
