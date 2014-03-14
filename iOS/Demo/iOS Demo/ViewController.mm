//
//  ViewController.m
//  iOS Demo
//
//  Created by Ariel Elkin on 03/03/2014.
//

#import "ViewController.h"

#import "SineWave.h"
#import "Brass.h"
#import "Mandolin.h"

@implementation ViewController {
    stk::SineWave *sineWave;
    stk::Brass *brass;
}

- (void)loadView {
    self.view = [UIView new];
    [self.view setBackgroundColor:[UIColor whiteColor]];
    [self setupUI];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
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
    
    //Test Mandolin:
    stk::Mandolin *mandolin = new stk::Mandolin(400);
    
    mandolin->pluck(1);
    
    for (NSUInteger i = 0; i < samplesToGenerate; i ++) {
        float sample = mandolin->tick();
        NSLog(@"Mandolin sample: %f", sample);
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
    
    NSDictionary *metrics = @{@"sliderWidth": @200};
    
    NSArray *sliderConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-40-[sineSlider(sliderWidth)]" options:0 metrics:metrics views:@{@"sineSlider": sineSlider}];
    [self.view addConstraints:sliderConstraints];
    
    //Add slider to control brass's frequency:
    UISlider *brassSlider = [[UISlider alloc] init];
    [brassSlider addTarget:self action:@selector(brassSliderMoved:) forControlEvents:UIControlEventValueChanged];
    [brassSlider setMinimumValue:0];
    [brassSlider setMaximumValue:800];
    [brassSlider setTranslatesAutoresizingMaskIntoConstraints:NO];
    [self.view addSubview:brassSlider];
    
    sliderConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"H:|-40-[brassSlider(sliderWidth)]" options:0 metrics:metrics views:@{@"brassSlider": brassSlider}];
    [self.view addConstraints:sliderConstraints];
    sliderConstraints = [NSLayoutConstraint constraintsWithVisualFormat:@"V:|-40-[sineSlider]-[brassSlider]" options:0 metrics:nil views:@{@"sineSlider": sineSlider, @"brassSlider": brassSlider}];
    [self.view addConstraints:sliderConstraints];
}

@end
