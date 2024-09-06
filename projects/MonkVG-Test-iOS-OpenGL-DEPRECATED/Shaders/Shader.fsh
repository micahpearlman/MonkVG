//
//  Shader.fsh
//  MonkVG-Test-iOS-OpenGL
//
//  Created by Micah Pearlman on 8/11/10.
//  Copyright Zero Vision 2010. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
