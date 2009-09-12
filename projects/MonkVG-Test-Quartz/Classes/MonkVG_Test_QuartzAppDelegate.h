//
//  MonkVG_Test_QuartzAppDelegate.h
//  MonkVG-Test-Quartz
//
//  Created by Micah Pearlman on 3/2/09.
//  Copyright Monk Games 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@class RootViewController;

@interface MonkVG_Test_QuartzAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    RootViewController *rootViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet RootViewController *rootViewController;

@end

