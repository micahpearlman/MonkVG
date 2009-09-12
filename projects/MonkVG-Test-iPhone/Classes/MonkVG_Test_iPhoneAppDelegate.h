//
//  MonkVG_Test_iPhoneAppDelegate.h
//  MonkVG-Test-iPhone
//
//  Created by Micah Pearlman on 2/22/09.
//  Copyright Monk Games 2009. All rights reserved.
//

#import <UIKit/UIKit.h>

@class EAGLView;

@interface MonkVG_Test_iPhoneAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    EAGLView *glView;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;

@end

