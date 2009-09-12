//
//  MonkVG_Test_QuartzAppDelegate.m
//  MonkVG-Test-Quartz
//
//  Created by Micah Pearlman on 3/2/09.
//  Copyright Monk Games 2009. All rights reserved.
//

#import "MonkVG_Test_QuartzAppDelegate.h"
#import "RootViewController.h"

@implementation MonkVG_Test_QuartzAppDelegate


@synthesize window;
@synthesize rootViewController;


- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
    [window addSubview:[rootViewController view]];
    [window makeKeyAndVisible];
}


- (void)dealloc {
    [rootViewController release];
    [window release];
    [super dealloc];
}

@end
