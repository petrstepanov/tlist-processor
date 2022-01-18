# TLIST Processor

Software is designed to process two-dimensional Doppler energy spectra. The background fitting and subtraction mechanism is implemented. Ridge profile along every energy axis is fitted with a step function that accuonts on higher energetic Compton scattered photons (constant background along the energy axis) and Compton scattering of annihilation photons outside of the detector volume (~constant contribution in the range < 511 keV). An exponential contribution in the range < 511 keV is found due to incomplete charge collection in the detector. Another exponential contribution in the region > 511 keV occurs due to the pile-up effect.

<img src="http://petrstepanov.com/static/screenshot-tlist-processor.png" alt="TLIST Processor desktop application" style="width: 100%;"/>

After the background subtraction software transforms two-dimensional histogram into a one-dimensional by introducing a new variable E1-E2. Check our [research group website SelimLab](http://physics.bgsu.edu/~faselim/).

Application is written in C++ with CERN ROOT libraries and GUI. In order to run the app first we need to install ROOT libraries. Unfortunately it is impossible to do static link your applications object files with the ROOT libraries as well as build their static versions. Moreover, sometimes source headers should be available at runtime.

Currently it is possible to run application either on macOS or Linux. I will try to compile the windows binary whenever the native Windows ROOT 6 build will be released.

## Installation on Windows 10

1. Install Ubuntu linux inside Windows Subsystem for Linux (WSL) as described [here](https://medium.com/@petrstepanov/installing-cern-root-in-windows-subsystem-for-linux-wsl-e83533a10a18).

2. Open _Start → Ubuntu_ to launch linux Terminal. Proceed with Installation on Linux below.

P.S. Running the program on older versions of Windows is currently not possible.

## Installation on Linux or macOS

Install CERN ROOT on your system. Refer to [official install page](https://root.cern/install/#linux-package-managers) for more details.

Open Terminal and clone program source files in your home folder:

```
cd
git clone https://github.com/petrstepanov/sw-calculator
cd sw-calculator
```
Check program dependencies and create native makefile with CMake. Finally build and install. System will ask for your password to install the app.

```
cmake ./
make
sudo make install
```
On linux you can install application launcher:

```
make install-launcher
```
To install app launcher on macOS run package `resources/sw-calculator.pkg`.

Program source files are not needed any more and can be removed from your computer:

```
rm -rf ~/sw-calculator
```

## Running program

Find application in the system menu. Alternatively, open Termnal and type `sw-calculator` to launch the program. 


---

Shoot me an email with feedback or questions: [stepanovps@gmail.com](mailto:stepanovps@gmail.com)