//
//  MainViewController.m
//  MonkVG-Test-Quartz
//
//  Created by Micah Pearlman on 3/2/09.
//  Copyright Monk Games 2009. All rights reserved.
//

#import "MainViewController.h"
#import "MainView.h"
#include "MonkVG/openvg.h"
#include "MonkVG/vgu.h"

extern "C" void loadTiger();

@implementation MainViewController


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
		vgCreateContextSH(320,480);
		
		loadTiger();
		
    }
    return self;
}



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {
	
    [super viewDidLoad];
	
	[NSTimer scheduledTimerWithTimeInterval:(1.0 / 30.0f ) target:self selector:@selector(renderScene) userInfo:nil repeats:YES];	
	
}

// Renders one scene of the game
- (void)renderScene {
	[self.view setNeedsDisplay];
}



/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
}


- (void)dealloc {
    [super dealloc];
}


@end
