//
//  ViewController.m
//  iOS Demo
//
//  Created by Ariel Elkin on 03/03/2014.
//

#import "ViewController.h"

/* 
 
 Cherry-pick the files you'll need for your app:
 
 Click on STK.xcodeproj, select the STK target,
 go to the Build Phases, and add the .cpp files you
 need in the 'Compile Sources' section.
 */

#import "SineWave.h" //Add SineWave.cpp
#import "Brass.h"  //Add ADSR.cpp, BiQuad.cpp, DelayA.cpp, and PoleZero.cpp

@implementation ViewController {
    stk::SineWave *sineWave;
    stk::Brass *brass;
}

- (void)loadView {
    self.view = [UIView new];
    [self.view setBackgroundColor:[UIColor whiteColor]];
    [self setupUI];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSUInteger samplesToGenerate = 1000;
    
    //Test SineWave:
    sineWave = new stk::SineWave();
    
    for (NSUInteger i = 0; i < samplesToGenerate; i ++) {
        float sample = sineWave->tick();
        NSLog(@"SineWave sample: %f", sample);
    }
    
    //Test Brass:
    brass = new stk::Brass();
    
    brass->noteOn(400, 1);
    
    for (NSUInteger i = 0; i < samplesToGenerate; i ++) {
        float sample = brass->tick();
        NSLog(@"Brass sample: %f", sample);
    }
    
    #pragma mark TODO - Audio playback
}


- (void)sineSliderMoved:(UISlider *)slider {
    sineWave->setFrequency(slider.value);
    NSLog(@"Setting SineWave frequency to %.2f", slider.value);
}

- (void)brassSliderMoved:(UISlider *)slider {
    brass->setFrequency(slider.value);
    NSLog(@"Setting Brass frequency to %.2f", slider.value);
}

- (void)setupUI {
    
    //Add slider to control sine wave frequency:
    UISlider *sineSlider = [[UISlider alloc] init];
    [sineSlider addTarget:self action:@selector(sineSliderMoved:) forControlEvents:UIControlEventValueChanged];
    [sineSlider setMinimumValue:0];
    [sineSlider setMaximumValue:800];
    [sineSlider setTranslatesAutoresizingMaskIntoConstraints:NO];
    [self.view addSubview:sineSlider];
    
    NSArray *sliderConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-40-[sineSlider(200)]" options:0 metrics:nil views:@{@"sineSlider": sineSlider}];
    [self.view addConstraints:sliderConstraints];
    sliderConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-40-[sineSlider]" options:0 metrics:nil views:@{@"sineSlider": sineSlider}];
    [self.view addConstraints:sliderConstraints];
    
    
    //Add slider to control brass's frequency:
    UISlider *brassSlider = [[UISlider alloc] init];
    [brassSlider addTarget:self action:@selector(brassSliderMoved:) forControlEvents:UIControlEventValueChanged];
    [brassSlider setMinimumValue:0];
    [brassSlider setMaximumValue:800];
    [brassSlider setTranslatesAutoresizingMaskIntoConstraints:NO];
    [self.view addSubview:brassSlider];
    
    sliderConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-40-[brassSlider(200)]" options:0 metrics:nil views:@{@"brassSlider": brassSlider}];
    [self.view addConstraints:sliderConstraints];
    sliderConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-40-[brassSlider]" options:0 metrics:nil views:@{@"brassSlider": brassSlider}];
    [self.view addConstraints:sliderConstraints];
}

@end
