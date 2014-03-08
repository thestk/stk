##Usage

1. Drag and drop STK.xcodeproj into your xcode project

1. Open the project's settings, open the *Build Phases* tab. In the *Link Binary with Libraries section*, add add **libSTK.a**. 

1. Add the STK's **include** folder to your project's header search paths: `../stk/include/`. Use `..` for each step above you need to go. 

1. Import the STK classes in the files you require:
`#import "SineWave.h"`

1. Rename any Objective-C files that import STK files with the **.mm** extension. E.g. **ViewController.m** —> **ViewController.mm**


1. Some classes (such as `Mandolin`, `Wurley`, or `Rhodey`) require you to copy the STK's raw wave files into your bundle. You'll know because you will be getting this runtime error: 
`FileRead::open: could not open or find file (../../rawwaves/filename.raw)!`
You then need to copy the rawwaves into your bundle. Open your project's settings, open the *Build Phases* tab. In the *Copy Bundle Resources*, drag and drop **rawwaves.bundle** (it's located in **STK.xcodeproj**'s **Helpers** folder). 

Add this code once in your app if using a class that needs the raw waves: 

```
    NSBundle *rawwaveBundle = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:@"rawwaves" withExtension:@"bundle"]];

    stk::Stk::setRawwavePath([[rawwaveBundle resourcePath] UTF8String]);
```

If you get this error: `rawwaves.bundle: No such file or directory`

Then select the rawwaves scheme, build it, then build your project's main scheme. 
  


