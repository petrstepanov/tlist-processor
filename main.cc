#include <TROOT.h>
#include <TApplication.h>
#include "TlistProcessorFrame.h"

int main(int argc, char **argv) {
    TApplication theApp("App", &argc, argv);
    new TlistProcessorFrame(gClient->GetRoot(), 800, 600);
    theApp.Run();
    return 0;
}
