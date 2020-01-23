# TLIST Processor

Software is designed to process two-dimensional Doppler energy spectra. The background fitting and subtraction mechanism is implemented. Ridge profile along every energy axis is fitted with a step function that accuonts on higher energetic Compton scattered photons (constant background along the energy axis) and Compton scattering of annihilation photons outside of the detector volume (~constant contribution in the range < 511 keV). An exponential contribution in the range < 511 keV is found due to incomplete charge collection in the detector. Another exponential contribution in the region > 511 keV occurs due to the pile-up effect.

<img src="http://petrstepanov.com/static/screenshot-tlist-processor.png" alt="TLIST Processor desktop application" style="width: 100%;"/>

After the background subtraction software transforms two-dimensional histogram into a one-dimensional by introducing a new variable E1-E2. Check our [research group website SelimLab](http://physics.bgsu.edu/~faselim/).

Application is written in C++ with CERN ROOT libraries and GUI. In order to run the app first we need to install ROOT libraries. Unfortunately it is impossible to do static link your applications object files with the ROOT libraries as well as build their static versions. Moreover, sometimes source headers should be available at runtime.

Currently it is possible to run application either on macOS or Linux. I will try to compile the windows binary whenever the native Windows ROOT 6 build will be released.

## Installation on Windows 10

1. Install Ubuntu linux inside Windows Subsystem for Linux (WSL) as described [here](https://medium.com/@petrstepanov/install-cern-root-roofit-on-macos-and-linux-for-dummies-df787fd41ef4).

2. Open _Start → Ubuntu_ to launch linux Terminal. Proceed with Installation on Linux below.

P.S. Running the program on older versions of Windows is currently not possible. If you are familiar with CMAKE please contribute :-) 

## Installation on Linux or macOS

1. Install CERN ROOT on your system. Details can be found [here](https://medium.com/@petrstepanov/install-cern-root-roofit-on-macos-and-linux-for-dummies-df787fd41ef4).

2. Open your Terminal and clone the program folder on your hard drive:

```
mkdir -p ~/Downloads && cd ~/Downloads
git clone https://github.com/petrstepanov/tlist-processor
cd ./tlist-processor
```

3. Compile the source code and link with ROOT libraries into a binary executable:
```
make
make install
```
4. In order to launch the application type `tlist-processor` in Terminal.

5. Optionally add an application launcher to the list of your system apps. On Linux execute `make install-linux-launcher`. On macOS run `resources/tlist-processor.pkg`.

## Users Guide

Not yet implemented.

---

Shoot me an email with feedback or questions: [petrs@bgsu.edu](mailto:petrs@bgsu.edu)