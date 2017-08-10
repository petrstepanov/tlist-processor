/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Geometry.h
 * Author: petrstepanov
 *
 * Created on August 9, 2017, 10:29 PM
 */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <TROOT.h>

class Geometry {
public:
    Geometry();
    Geometry(const Geometry& orig);
    virtual ~Geometry();
    static Double_t distance(Double_t A, Double_t B, Double_t C, Double_t Mx, Double_t My);
    static Double_t getRectAreaRatio(Double_t e1A, Double_t e2A, Double_t e1B, Double_t e2B, Double_t D_sect);
    
private:

};

#endif /* GEOMETRY_H */

