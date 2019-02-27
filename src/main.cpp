#include <TROOT.h>
#include <TApplication.h>
#include "TlistProcessorFrame.h"

int main(int argc, char **argv) {
	TApplication* app = new TApplication("TLIST Processor", &argc, argv);
	TlistProcessorFrame* tlistProcessorFrame = new TlistProcessorFrame(gClient->GetRoot());
    app->Run();
    return 0;
}
