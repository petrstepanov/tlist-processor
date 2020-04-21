/*
 * Utils.cpp
 *
 *  Created on: Apr 2, 2020
 *      Author: petrstepanov
 */

#include "Utils.h"

Double_t Utils::roundToOneDecimal(Double_t val){
	Double_t value = (int)(val * 10 + .5);
    return (Double_t) value / 100;
}
