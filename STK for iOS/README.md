##Setup

1. Clone or download the STK into a folder within your project.

1. Open the **STK for iOS** folder, and drag and drop **STK.xcodeproj** into your Xcode project.

1. Open your project's settings, open the *Build Phases* tab. In the *Link Binary with Libraries* section, add **libSTK.a**. 
![][linking_libSTK]

1. In your project's settings, open the *Build Settings* tab. In the *Search Paths* section, double click on the field to the right of *Header Search Paths*, and add the path to the STK's **include** folder relative to your Xcode project.
![][header_search_paths]


##Usage

1. Import the STK classes in the source files you require. E.g.
`#import "SineWave.h"`

1. Change the extension of Objective-C files that import STK files to **.mm**. E.g. **ViewController.m** —> **ViewController.mm**

You can also look at the [iOS Demo project](..projects/demo/iOS%20Demo) for a sample usage. 


##Troubleshooting

###`Lexical or preprocessor Issue 'FileName.h' file not found

You have the wrong header search path in your project's settings. 

This is the path to the STK's **include** folder relative to your project's folder (as if you were `cd`ing into it). For example, it is `../stk/include/` if both share the same directory, but it would be `stk/include/` if the stk folder is inside your project's folder. 

If this problem doesn't go away:

1. Delete **STK.xcodeproj** from your Xcode project
1. Move the STK directory within your project's directory. 
1. Follow step 1 from **Setup**, add `stk/include` to the *Header Search Paths*.

[!](header_search_paths)


###Raw waves

If you use a class that makes use of raw waves (such as `Mandolin`, `Wurley`, or `Rhodey`) you need to copy the STK's raw wave files into your bundle. You'll know you need to if you get this runtime error: 
`FileRead::open: could not open or find file (../../rawwaves/filename.raw)!`

1. Open your project's settings, open the *Build Phases* tab. 
1. In the *Copy Bundle Resources*, drag and drop **rawwaves.bundle** (it's located in **STK.xcodeproj**'s **Helpers** folder). 
1. Then add this code before using a class that needs the raw waves: 

```objective-c
NSBundle *rawwaveBundle = [NSBundle bundleWithURL:[[NSBundle mainBundle] URLForResource:@"rawwaves" withExtension:@"bundle"]];
stk::Stk::setRawwavePath([[rawwaveBundle resourcePath] UTF8String]);
```


###`rawwaves.bundle: No such file or directory`

This means that **rawwaves.bundle** hasn't been copied to the build folder, so you'll need to do it manually:

Select the rawwaves scheme:

![][rawwaves_scheme]
  
Build it (⌘+B)  then build your project's main scheme. 

###`Apple Mach-O Linker Error`

This means that **STKLib.a** isn't being linked to your binary. Follow step 2 above in Setup. 


[rawwaves_scheme]: http://i.imgur.com/PKd7epf.png

[linking_libSTK]: http://i.imgur.com/cLbGrtq.png
[header_search_paths]: http://i.imgur.com/iBTC06h.png