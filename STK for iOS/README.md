##Usage

1. Drag and drop STK.xcodeproj into your xcode project

1. Open the project's settings, open the *Build Phases* tab. In the *Link Binary with Libraries section*, add add **libSTK.a**. 

1. Make the STK static library compile the STK source files you require. Click on **STK.xcodeproj**, open the *Build Phases* tab, and add them in the *Compile Sources* section. For example, if you want to use `SineWave`, add **SineWave.cpp**. If you want to use Brass, add 

1. Add the STK's **include** folder to your project's header search paths: `../stk/include/`. Use `..` for each step above you need to go. 

1. Import the STK classes in the files you require:
#import "SineWave.h"

1. Rename any Objective-C files that import STK files with the **.mm** extension. E.g. **ViewController.m** —> **ViewController.mm**