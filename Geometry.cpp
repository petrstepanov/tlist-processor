/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Geometry.cpp
 * Author: petrstepanov
 * 
 * Created on August 9, 2017, 10:29 PM
 */

#include "Geometry.h"
#include <TMath.h>

Geometry::Geometry() {
}

Geometry::Geometry(const Geometry& orig) {
}

Geometry::~Geometry() {
}

Double_t Geometry::distance(Double_t A, Double_t B, Double_t C, Double_t Mx, Double_t My){
	Double_t d = std::abs(A*Mx + B*My + C) / std::sqrt(A*A + B*B);
	return d;
}

Double_t Geometry::getRectAreaRatio(Double_t e1A, Double_t e2A, Double_t e1B, Double_t e2B, Double_t D_sect){
    Double_t e1 = (e1A+e1B)/2;
    Double_t e2 = (e2A+e2B)/2;
    Double_t D = e1-e2;
    Double_t de1 = e1B - e1A;
    Double_t de2 = e2A - e2B;
    Double_t DA = e1A - e2A;
    Double_t DB = e1B - e2B;
    Double_t min = std::min(de1, de2);
    Double_t cutArea = 0;
    Double_t diag = std::sqrt(de1*de1+de2*de2);
    Double_t ksi;
    Double_t PI = TMath::Pi();
    Double_t SQRT2 = TMath::Sqrt2();
    
    if (de1 >=de2) ksi = PI/4 + std::atan(de2/de1);
    else ksi = PI/4 + std::atan(de1/de2);

//    Double_t diag_perp = diag*std::sin(ksi);
//    Double_t sect = D_sect / SQRT2;

    if (D_sect <= DA){
        return 0;
    }
    if (D_sect > DA && D_sect <= DA + min) {
        Double_t cutTriangHeight = (D_sect - DA)/SQRT2; // corner triangle height
        cutArea = pow(cutTriangHeight, 2);
        return cutArea / (de1*de2);
    }
    if (D_sect > DA + min && D_sect <= DB - min){
        Double_t dD = D_sect - (DA + min); // tricky
        cutArea = min*min/2 + (dD/SQRT2)*(min*SQRT2);
        return cutArea / (de1*de2);
    }
    else if (D_sect > DB - min && D_sect <= DB){
        Double_t cutTriangHeight = (DB-D_sect)/SQRT2; // corner triangle height
        cutArea = de1*de2 - pow(cutTriangHeight, 2);
        return cutArea / (de1*de2);
    }
    return 1;
}
