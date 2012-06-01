//
//  MainView.m
//  MonkVG-Test-Quartz
//
//  Created by Micah Pearlman on 3/2/09.
//  Copyright Monk Games 2009. All rights reserved.
//

#import "MainView.h"
#import <QuartzCore/QuartzCore.h>
#include "MonkVG/openvg.h"
#include "MonkVG/vgu.h"

extern "C" void loadTiger();
extern "C" void display(float dt);

@implementation MainView


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
        // Initialization code
		//loadTiger();
    }
    return self;
}


- (void)drawRect:(CGRect)rect {
	
//	vgCreateContextSH(320,480);

    // Drawing code...
//	loadTiger();
	
	display(0.05f);

//	VGPaint strokePaint = vgCreatePaint();
//	vgSetPaint(strokePaint, VG_STROKE_PATH);
//	
//	VGfloat color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
//    vgSetParameterfv(strokePaint, VG_PAINT_COLOR, 4, &color[0]);
//	
//	VGPath line = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,1,0,0,0, VG_PATH_CAPABILITY_ALL);
//	vguLine(line, 20,20,130,130);
//	
//	VGPath square = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,1,0,0,0, VG_PATH_CAPABILITY_ALL);
//	vguRect( square, 10.0f, 10.0f, 130.0f, 50.0f );
//	
//	vgSetf( VG_STROKE_LINE_WIDTH, 7.0f );
//	vgDrawPath(line, VG_STROKE_PATH);
//	vgDrawPath(square, VG_STROKE_PATH);
//	
//	
//	CGContextRef context = UIGraphicsGetCurrentContext();
//	// Drawing with a white stroke color
//	CGContextSetRGBStrokeColor(context, 1.0, 1.0, 1.0, 1.0);
//	// And drawing with a blue fill color
//	CGContextSetRGBFillColor(context, 0.0, 0.0, 1.0, 1.0);
//	// Draw them with a 2.0 stroke width so they are a bit more visible.
//	CGContextSetLineWidth(context, 2.0);
//	
//	// Add Rect to the current path, then stroke it
//	CGContextAddRect(context, CGRectMake(30.0, 30.0, 60.0, 60.0));
//	CGContextStrokePath(context);
	
}


- (void)dealloc {
    [super dealloc];
}


@end
