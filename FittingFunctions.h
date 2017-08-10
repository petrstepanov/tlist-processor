/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   FittingFunctions.h
 * Author: petrstepanov
 *
 * Created on August 9, 2017, 10:19 PM
 */

#ifndef FITTINGFUNCTIONS_H
#define FITTINGFUNCTIONS_H

#include "TROOT.h"

//class FittingFunctions {
//public:
//    FittingFunctions();
//    FittingFunctions(const FittingFunctions& orig);
//    virtual ~FittingFunctions();
//private:
//
//};

Double_t convolutionGauss(Double_t (*f)(Double_t, Double_t*), Double_t x, Double_t* par, Double_t convFWHM);

Double_t convolutionGauss(Double_t (*f)(Double_t*, Double_t*), Double_t* x, Double_t* par, Double_t convFWHM);

Double_t orePowell(Double_t e, Double_t mc2);

Double_t ridgeProfile(Double_t e, Double_t* par);

Double_t bgfunc(Double_t *x, Double_t *par);

Double_t spectfunc(Double_t *x, Double_t *par);

Double_t bgfuncrotate(Double_t *x, Double_t *par);

Double_t peakfunc(Double_t *x, Double_t *par);


#endif /* FITTINGFUNCTIONS_H */

