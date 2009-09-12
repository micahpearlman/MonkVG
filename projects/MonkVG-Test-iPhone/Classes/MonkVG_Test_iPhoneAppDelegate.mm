//
//  MonkVG_Test_iPhoneAppDelegate.m
//  MonkVG-Test-iPhone
//
//  Created by Micah Pearlman on 2/22/09.
//  Copyright Monk Games 2009. All rights reserved.
//

#import "MonkVG_Test_iPhoneAppDelegate.h"
#import "EAGLView.h"

@implementation MonkVG_Test_iPhoneAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	glView.animationInterval = 1.0 / 60.0;
	[glView startAnimation];
}


- (void)applicationWillResignActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 5.0;
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
	glView.animationInterval = 1.0 / 60.0;
}


- (void)dealloc {
	[window release];
	[glView release];
	[super dealloc];
}

@end
