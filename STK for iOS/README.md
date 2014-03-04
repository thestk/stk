##Usage

1. Drag and drop **STK.xcodeproj** into your Xcode project.

1. Open your project's settings, open the *Build Phases* tab. In the *Link Binary with Libraries* section, add **libSTK.a**. 

1. Click on **STK.xcodeproj**, open the *Build Phases* tab, and add the STK files you require in the *Compile Sources* section. For example, if you want to use `SineWave`, add **SineWave.cpp**. If you want to use `Brass`, add **Brass.cpp**, **ADSR.cpp**, **Biquad.cpp**, **DelayA**, and **PoleZero.cpp**.

1. Open your project's settings, open the *Build Settings* tab. Add the STK's **include/** directory to your project's header search paths. E.g. `../stk/include/`. 

1. Import the STK classes in the files you require: 
    `#import "SineWave.h"`

1. Rename any Objective-C files that import STK files with the **.mm** extension. E.g. **ViewController.m** —> **ViewController.mm**.
