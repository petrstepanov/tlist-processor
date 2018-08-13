/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   HistProcessor.cpp
 * Author: petrstepanov
 *
 * Created on August 9, 2017, 10:11 PM
 */

#include "HistProcessor.h"
#include "StringUtils.h"

HistProcessor::HistProcessor(){};

HistProcessor::HistProcessor(HistProcessor const& fileUtil){};

HistProcessor* HistProcessor::instance = NULL;

HistProcessor* HistProcessor::getInstance(){
	if (!instance){
		instance = new HistProcessor;
	}
	return instance;
}

ConversionGain HistProcessor::getConversionGain(TH2* hist){
    TAxis* xAxis = hist->GetXaxis();
    Double_t binWidth = xAxis->GetBinUpEdge(1) - xAxis->GetBinLowEdge(1);
    if (binWidth >= 0.15) return GAIN_LOW;      // ~ 0.2-0.3 keV     8192 Channels
    if (binWidth >= 0.1)  return GAIN_MEDIUM;   // ~ 0.12 keV       16384 Channels
    return GAIN_HIGH;                           // ~ 0.06 keV       32768 Channels
}

Double_t HistProcessor::getHiSleeveMaxCount(TH2* hist, Bool_t isRotated){
    Double_t maxCount = 0;
    if (isRotated){
        for (Int_t binX = 10; binX <= hist->GetXaxis()->GetNbins()-10; binX++){
            for (Int_t binY = 10; binY < hist->GetYaxis()->GetNbins()*1/5; binY++){
                Double_t count = hist->GetBinContent(binX, binY);
                if (count > maxCount) maxCount = count;
            }
        }
        return maxCount;
    }
    for (Int_t binX = 10; binX < hist->GetXaxis()->GetNbins()*1/5; binX++){
        for (Int_t binY = hist->GetYaxis()->GetNbins()*2/5; binY <= hist->GetYaxis()->GetNbins()*3/5; binY++){
            Double_t count = hist->GetBinContent(binX, binY);
            if (count > maxCount) maxCount = count;
        }
    }
    return maxCount;
}

Double_t HistProcessor::getLowSleeveMaxCount(TH2* hist, Bool_t isRotated){
    Double_t maxCount = 0;
    if (isRotated){
        for (Int_t binX = 10; binX <= hist->GetXaxis()->GetNbins()-10; binX++){
            for (Int_t binY = hist->GetYaxis()->GetNbins()*4/5; binY < hist->GetYaxis()->GetNbins()-10; binY++){
                Double_t count = hist->GetBinContent(binX, binY);
                if (count > maxCount) maxCount = count;
            }
        }
        return maxCount;
    }
    for (Int_t binX = hist->GetXaxis()->GetNbins()*4/5; binX < hist->GetXaxis()->GetNbins()-10; binX++){
        for (Int_t binY = hist->GetYaxis()->GetNbins()*2/5; binY <= hist->GetYaxis()->GetNbins()*3/5; binY++){
            Double_t count = hist->GetBinContent(binX, binY);
            if (count > maxCount) maxCount = count;
        }
    }
    return maxCount;
}

TH2* HistProcessor::subtractFitFromHist(TH2* h, Double_t *par, Bool_t doRotate){
    TH2* hist = (TH2F*)h->Clone("histWithSubtractedBackground");
    TAxis* xAxis = hist->GetXaxis();
    TAxis* yAxis = hist->GetYaxis();
    //TCanvas* c = new TCanvas("test_c", "test_c");
    //c->cd();
    //TF2* tf2 = new TF2("ttfff2", bgfunc, 501, 521, 501, 521, 8);
    //tf2->SetParameters(par);
    //tf2->Draw();
    for (Int_t binX = 1; binX < 1 + hist->GetNbinsX(); binX++){
        for (Int_t binY = 1; binY < 1 + hist->GetNbinsY(); binY++){
            Double_t count = hist->GetBinContent(binX, binY);
            Double_t e1 = xAxis->GetBinCenter(binX);
            Double_t e2 = yAxis->GetBinCenter(binY);
            Double_t x[2] = { e1, e2 };
            Double_t bg = doRotate ? bgfuncrotate(x, par) : bgfunc(x, par);
            // Double_t bg = doRotate ? bgfuncrotate(x, par) : bgfuncconv(x, par);
            hist->SetBinContent(binX, binY, count - bg); // std::max(0.0, count - bg));
        }
    }
    return hist;
}

TH1Pair HistProcessor::projectHist(TH2* hist, Double_t e1Mean, Double_t e2Mean, Double_t dEplus, Double_t dEminus, Bool_t doRegion){
    std::cout << "projecting histogram with dE+: " << dEplus << " dE-: " << dEminus << std::endl;
    // Set binning of the Doppler histogram depending on the analyzer resolution
    Int_t bins;
    ConversionGain gain = getConversionGain(hist);
    if (gain == GAIN_HIGH) bins = 740;			// the point here is that doppler bin width has to be larger than
    else if (gain == GAIN_MEDIUM) bins = 370;   // any 2D bin "diagonal" length (in correspondent variables)
    else bins = 185;

    std::cout << "Projecting histogram. Gain is " << gain << std::endl;
    Double_t diagMin = -60; // these value is quite randomly picked - doppler spectrum limits
    Double_t diagMax = 60;

    TH1* histDoppler = new TH1F("histDoppler", "Diagonal doppler histogram", bins, diagMin, diagMax);
    TH1* histSumNumber = new TH1F("histSumNumber", "Number of events in a diagonal histogram", bins, diagMin, diagMax);

    TAxis* xAxis = hist->GetXaxis();
    TAxis* yAxis = hist->GetYaxis();

    //Double_t lynx1BinWidth = (fLynx1Emax->GetNumber() - fLynx1Emin->GetNumber()) / (Double_t)(iLynx1Bins->GetNumber());
    //Double_t lynx2BinWidth = (fLynx2Emax->GetNumber() - fLynx2Emin->GetNumber()) / (Double_t)(iLynx2Bins->GetNumber());

    Double_t maxDiag = 0; // Track maximum bin diagonal width to be less than doppler bin width
    for (Int_t binX = 1; binX <= hist->GetNbinsX(); binX++){
        for (Int_t binY = 1; binY <= hist->GetNbinsY(); binY++){
            Double_t count = hist->GetBinContent(binX, binY);
            if (count != 0){
                Double_t e1 = xAxis->GetBinCenter(binX);
                Double_t e2 = yAxis->GetBinCenter(binY);

                // [keV] - ionization potential

                Double_t D = e1 - e2;
                Double_t e1A = xAxis->GetBinLowEdge(binX);
                Double_t e1B = xAxis->GetBinUpEdge(binX);
                Double_t e2A = yAxis->GetBinUpEdge(binY);
                Double_t e2B = yAxis->GetBinLowEdge(binY);
                Double_t de1 = e1B - e1A;
                Double_t de2 = e2A - e2B;
                Double_t DA = e1A - e2A;
                Double_t DB = e1B - e2B;
                // Double_t min = std::min(de1, de2);
                Double_t area = de1*de2;
                Int_t binDiag = histDoppler->FindBin(D);

                // Key to success - see Serge Stepanov sum region
                // TODO: check region
                // if (e1 + e2 >= e1Mean + e2Mean - dEminus - min && e1 + e2 < e1Mean + e2Mean + dEplus + min &&
                // 	e2 - e1 >= 2*e2Min - e1Mean - e2Mean + dEminus + dEplus && e2 - e1 < -2*e1Min + e1Mean + e2Mean - dEminus - dEplus){

                if (!doRegion || (doRegion && e1 + e2 >= e1Mean + e2Mean - dEminus - de2 && e1 + e2 < e1Mean + e2Mean + dEplus + de2)){

                    Double_t diag = DB - DA;
                    if (diag > maxDiag) maxDiag = diag;

                    // border ratio is ratio of areas cut by the doppler region border
                    Double_t borderRatio = 1;

                    if (doRegion){
                        // TODO: check region
                        if (e1 + e2 < e1Mean + e2Mean - dEminus){
                            Double_t d = Geometry::distance(1, 1, -e1Mean - e2Mean + dEminus, e1B, e2A);
                            borderRatio = d*d / area;
                        }
                        else if (e1 + e2 < e1Mean + e2Mean - dEminus + de2){
                            Double_t d = Geometry::distance(1, 1, -e1Mean - e2Mean + dEminus, e1A, e2B);
                            borderRatio = 1 - d*d / area;
                        }
                        else if (e1 + e2 >= e1Mean + e2Mean + dEplus) {
                            Double_t d = Geometry::distance(1, 1, -e1Mean - e2Mean - dEplus, e1A, e2B);
                            borderRatio = d*d / area;
                        }
                        else if (e1 + e2 >= e1Mean + e2Mean + dEplus - de2){
                            Double_t d = Geometry::distance(1, 1, -e1Mean - e2Mean - dEplus, e1B, e2A);
                            borderRatio = 1 - d*d / area;
                        }
                    }

                    if (histDoppler->GetXaxis()->GetBinLowEdge(binDiag) >= DA){
                            Double_t ratio = Geometry::getRectAreaRatio(e1A, e2A, e1B, e2B, histDoppler->GetXaxis()->GetBinLowEdge(binDiag));
                            if (binDiag - 1 >= 1 && binDiag <= histDoppler->GetXaxis()->GetNbins()){
                                    histDoppler->AddBinContent(binDiag - 1, count*ratio*borderRatio);
                                    histSumNumber->AddBinContent(binDiag - 1, ratio*borderRatio);
                                    histDoppler->AddBinContent(binDiag, count*(1 - ratio)*borderRatio);
                                    histSumNumber->AddBinContent(binDiag, (1 - ratio)*borderRatio);
                            }
                    }
                    else if (histDoppler->GetXaxis()->GetBinUpEdge(binDiag) < DB){
                            Double_t ratio = Geometry::getRectAreaRatio(e1A, e2A, e1B, e2B, histDoppler->GetXaxis()->GetBinUpEdge(binDiag));
                            if (binDiag >= 1 && binDiag + 1 <= histDoppler->GetXaxis()->GetNbins()){
                                    histDoppler->AddBinContent(binDiag, count*ratio*borderRatio);
                                    histSumNumber->AddBinContent(binDiag, ratio*borderRatio);
                                    histDoppler->AddBinContent(binDiag + 1, count*(1 - ratio)*borderRatio);
                                    histSumNumber->AddBinContent(binDiag + 1, (1 - ratio)*borderRatio);
                            }
                    }
                    else {
                            if (binDiag >= 1 && binDiag <= histDoppler->GetXaxis()->GetNbins()){
                                    histDoppler->AddBinContent(binDiag, count*borderRatio);
                                    histSumNumber->AddBinContent(binDiag, 1*borderRatio);
                            }
                    }
                }
            }
        }
    }
    std::cout << "E1 axis bins: " << xAxis->GetNbins() << std::endl;
    std::cout << "E1 axis range:" << xAxis->GetXmin() << " - " << xAxis->GetXmax() << std::endl;
    std::cout << "E2 axis bins: " << yAxis->GetNbins() << std::endl;
    std::cout << "E2 axis range:" << yAxis->GetXmin() << " - " << yAxis->GetXmax() << std::endl;

    std::cout << "Diagonal axis bins: " << bins << std::endl;
    std::cout << "Diagonal axis range: " << diagMin << " - " << diagMax << std::endl;
    std::cout << "Number of bins on diagonal axis: " << bins << std::endl;

    std::cout << "X: " << (hist->GetNbinsX()) << " Y: " << (hist->GetNbinsY()) << std::endl;
    std::cout << "projectHist: Diagonal axis bin width " << (diagMax - diagMin) / bins << std::endl;
    std::cout << "projectHist: Maximum event doppler shift " << maxDiag << std::endl;
    std::cout << "Make sure maximum event doppler shift is less than diagonal bin width." << std::endl;

    TH1Pair pair;
    pair.first = histDoppler;
    pair.second = histSumNumber;

    return pair;
}

TH1Pair HistProcessor::projectHistRotated(TH2* hist, Double_t e1Mean, Double_t e2Mean, Double_t dEplus, Double_t dEminus, Bool_t doRegion){
    std::cout << "projecting rotated histogram with dE+: " << dEplus << " dE-: " << dEminus << std::endl;
    // Set binning of the Doppler histogram depending on the analyzer resolution
    TAxis* xAxis = hist->GetXaxis();
    TAxis* yAxis = hist->GetYaxis();
    Int_t bins = xAxis->GetNbins();
    Double_t diagMin = xAxis->GetBinLowEdge(1);
    Double_t diagMax = xAxis->GetBinUpEdge(bins);
    TH1* histDoppler = new TH1F("histDoppler", "Diagonal doppler histogram", bins, diagMin, diagMax);
    TH1* histSumNumber = new TH1F("histSumNumber", "Number of events in a diagonal histogram", bins, diagMin, diagMax);

    for (Int_t binX = 1; binX <= hist->GetNbinsX(); binX++){
        for (Int_t binY = 1; binY <= hist->GetNbinsY(); binY++){
            Double_t e1 = xAxis->GetBinCenter(binX);
            Double_t e2 = yAxis->GetBinCenter(binY);
            if (!doRegion || (doRegion && e2 >= e2Mean - dEminus / sqrt(2.) && e2 <= e2Mean + dEplus / sqrt(2.))){
                Double_t count = hist->GetBinContent(binX, binY);
                Int_t binDiag = histDoppler->FindBin(e1);
                if (binDiag >= 1 && binDiag <= histDoppler->GetXaxis()->GetNbins()){
                        histDoppler->AddBinContent(binDiag, count);
                        histSumNumber->AddBinContent(binDiag, 1);
                }
            }
        }
    }

    // Stretch histograms sqrt(2.) times
    histDoppler->GetXaxis()->SetLimits(diagMin*std::sqrt(2), diagMax*std::sqrt(2));
    histSumNumber->GetXaxis()->SetLimits(diagMin*std::sqrt(2), diagMax*std::sqrt(2));
    std::cout << "Number of bins in Doppler spectrum: " << bins << std::endl;

    TH1Pair pair;
    pair.first = histDoppler;
    pair.second = histSumNumber;
    return pair;
}

TH2* HistProcessor::readSpectrumFromFile(TString* filename){
    // Check file exists
    TString* strFileName = AppSettings::instance()->getFilename();
    std::ifstream inFile;
    inFile.open(filename->Data());
    if (!inFile){
            std::cout << "File \"" << strFileName->Data() << "\" not found." << std::endl;
    }

    // Skip spectrum header (2 lines)
    std::string str = "";
    const int skipLines = 2;
    for (int j = 0; j<skipLines; j++) {
        StringUtils::safeGetline(inFile, str);
//        std::cout << str << std::endl;
    }

    // Histogram bins centers
    std::vector<Double_t> e1Values;
    std::vector<Double_t> e2Values;

    Int_t count = 0;
    Double_t e1, e2;

    std::vector<Double_t> inputData;

    while (!StringUtils::safeGetline(inFile, str).eof()) {
//        std::cout << str << std::endl;
        std::stringstream ss(str);
        ss >> e1;
        if (ss.peek() == ',') ss.ignore();
        ss >> e2;
        if (ss.peek() == ',') ss.ignore();
        ss >> count;
//        std::cout << e1 << " " << e2 << " " << count << std::endl;
        // Push values to array and cut Kompton edges
//        if (e1>490 && e1 < 530 && e2>490 && e2 < 530){

//            Double_t sqrt2 = std::sqrt(2.);
//            if (checkboxSqrt2->IsOn()){
//                    e1 = e1 / sqrt2;
//                    e2 = e2 / sqrt2;
//            }

            inputData.push_back(e1);
            inputData.push_back(e2);
            inputData.push_back((Double_t)count);

            // Push e1 if vector not yet has it.
            if (std::find(e1Values.begin(), e1Values.end(), e1) == e1Values.end()) {
                    e1Values.push_back(e1);
            }
            // Push e2 if vector not yet has it.
            if (std::find(e2Values.begin(), e2Values.end(), e2) == e2Values.end()) {
                    e2Values.push_back(e2);
            }
//        }
    }
    inFile.close();

    // Sort bins
    std::sort(e1Values.begin(), e1Values.end());
    std::sort(e2Values.begin(), e2Values.end());

    // Print Bins
    // std::cout << std::endl;
    // std::cout << "e1Bins" << std::endl;
    // for(std::vector<Double_t>::iterator iter = e1Values.begin(); iter != e1Values.end(); ++iter){
    //     std::cout << *iter << ' ';
    // }
		//
    // std::cout << std::endl;
    // std::cout << "e2Bins" << std::endl;
    // for(std::vector<Double_t>::iterator iter = e2Values.begin(); iter != e2Values.end(); ++iter){
    //     std::cout << *iter << ' ';
    // }

    // Get Borders Arrays
    // Double_t* e1BinsBordersArray = getBorders(e1Values);
    // Double_t* e2BinsBordersArray = getBorders(e2Values);

    // Create bin borders arrays
    std::vector<Double_t> e1BinsBorders;
    std::vector<Double_t> e2BinsBorders;

    std::vector<Double_t>::iterator it = e1Values.begin();
    e1BinsBorders.push_back(*it - (*(it + 1) - *it) / 2);
    for (it = e1Values.begin(); (it + 1) != e1Values.end(); ++it){
            e1BinsBorders.push_back((*it + *(it + 1)) / 2);
    }
    e1BinsBorders.push_back(*it + (*it - *(it - 1)) / 2);

    it = e2Values.begin();
    e2BinsBorders.push_back(*it - (*(it + 1) - *it) / 2);
    for (it = e2Values.begin(); (it + 1) != e2Values.end(); ++it){
            e2BinsBorders.push_back((*it + *(it + 1)) / 2);
    }
    e2BinsBorders.push_back(*it + (*it - *(it - 1)) / 2);

    // Print Bin Borders
    // std::cout << std::endl;
    // std::cout << "e1BinBorders" << std::endl;
    // for(std::vector<Double_t>::iterator iter = e1BinsBorders.begin(); iter != e1BinsBorders.end(); ++iter){
    //     std::cout << *iter << ' ';
    // }
    // std::cout << std::endl;
		//
    // std::cout << std::endl;
    // std::cout << "e2BinsBorders" << std::endl;
    // for(std::vector<Double_t>::iterator iter = e2BinsBorders.begin(); iter != e2BinsBorders.end(); ++iter){
    //     std::cout << *iter << ' ';
    // }
    // std::cout << std::endl;

    // Create 2D histogram with the binning above
    // Int_t binsE1 = e1Values.size();
    // Int_t binsE2 = e2Values.size();
    Double_t* e1BinsBordersArray = &e1BinsBorders[0];
    Double_t* e2BinsBordersArray = &e2BinsBorders[0];

    // Create 2D histogram with the binning above
    // TODO: check how to properly delete ROOT object
//    if (hist != NULL){
//        delete hist;
//    }
    TH2* hist = new TH2F("hist", "Energy-Energy Histogram", e1Values.size(), e1BinsBordersArray, e2Values.size(), e2BinsBordersArray);

    // hist->Reset();
    // Set zero bins
    // for (Int_t binX = 1; binX < 1 + hist->GetNbinsX(); binX++){
    // 	for (Int_t binY = 1; binY < 1 + hist->GetNbinsY(); binY++){
    // 		hist->SetBinContent(binX, binY, 0);
    // 	}
    // }

    // Fill histogram from array
    for (it = inputData.begin(); it != inputData.end(); it += 3){
            e1 = *it;
            e2 = *(it + 1);
            count = (Int_t)(*(it + 2));
            Int_t bin = hist->FindBin(e1, e2);
            hist->SetBinContent(bin, count);
    }
    return hist;
}

void HistProcessor::saveHistToFile(TH1* hist, const char* filename) {
	std::ofstream outputFile;
	outputFile.open(filename);

	// Write file header
	outputFile << "E₁-E₂ keV, " << "Count" << std::endl;
	for (Int_t bin = 1; bin < 1 + hist->GetNbinsX(); bin++){
		Double_t dE = hist->GetXaxis()->GetBinCenter(bin);
		Double_t count = hist->GetBinContent(bin);
		outputFile << dE << ","
			<< count << std::endl;
	}
	outputFile.close();
}

// Bool_t HistProcessor::isTomskIssue(TH2* hist) {
//     Double_t sideBinCount = hist->GetBinContent(1, 1);
//     Double_t sleeveBinCount = hist->GetBinContent(20, 20);
//     std::cout << "sideBinCount: " << sideBinCount << ", sleeveBinCount: " << sleeveBinCount << std::endl;
//     if (sideBinCount > 5*sleeveBinCount) {
//         return kTRUE;
//     }
//     return kFALSE;
// }
