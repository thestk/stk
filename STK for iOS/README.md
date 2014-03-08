##Usage

1. Drag and drop **STK.xcodeproj** into your Xcode project.

1. Open your project's settings, open the *Build Phases* tab. In the *Link Binary with Libraries* section, add **libSTK.a**. 

1. Add the STK's **include** folder to your project's header search paths: `../stk/include/`. Use `..` for each step above you need to go. 

1. Import the STK classes in the files you require:
`#import "SineWave.h"`

1. Rename any Objective-C files that import STK files with the **.mm** extension. E.g. **ViewController.m** —> **ViewController.mm**

1. Look at the [iOS Demo project](..projects/demo/iOS%20Demo) for a sample usage. 


1. If you use a class that makes use of raw waves (such as `Mandolin`, `Wurley`, or `Rhodey`) you need to copy the STK's raw wave files into your bundle. You'll know you need to if you get this runtime error: 
`FileRead::open: could not open or find file (../../rawwaves/filename.raw)!`
You then need to copy the rawwaves into your bundle. Open your project's settings, open the *Build Phases* tab. In the *Copy Bundle Resources*, drag and drop **rawwaves.bundle** (it's located in **STK.xcodeproj**'s **Helpers** folder). 

Add this code once in your app if using a class that needs the raw waves: 

```objective-c
NSBundle *rawwaveBundle = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:@"rawwaves" withExtension:@"bundle"]];
stk::Stk::setRawwavePath([[rawwaveBundle resourcePath] UTF8String]);
```

If you get a `rawwaves.bundle: No such file or directory` error:

This means that **rawwaves.bundle** hasn't been copied to the build folder, so you'll need to do it manually:

Select the rawwaves scheme:

  ![][rawwaves_scheme]
  
Build it (⌘+B)  then build your project's main scheme. 

[rawwaves_scheme]: http://i.imgur.com/PKd7epf.png
