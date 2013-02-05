//
//  all_shaders.h
//  MonkVG-iOS
//
//  Created by Micah Pearlman on 1/17/13.
//  Modified by Gav Wood on 4/2/13.
//
//

#ifndef _all_shaders_h
#define _all_shaders_h

#include "alphaTest.frag.h"
#include "clipPlane.frag.h"
#include "lighting.frag.h"
#include "lightingPerFragment.frag.h"
#include "main.frag.h"
#include "postprocess.frag.h"
#include "texture.frag.h"
#include "texture0.frag.h"
#include "texture1.frag.h"
#include "texture2.frag.h"
#include "clipPlane.vert.h"
#include "lighting.vert.h"
#include "lightingPerFragment.vert.h"
#include "lightingPerVertex.vert.h"
#include "main.vert.h"
#include "postprocess.vert.h"
#include "texture.vert.h"
#include "texture0.vert.h"
#include "texture1.vert.h"
#include "texture2.vert.h"
#include "fog.glsl.h"

#include <string>
struct shaderMap {
    std::string name;
    unsigned char* shader;
    unsigned int len;
};

#define SHADERMAP_CNT 21
shaderMap shaders[SHADERMAP_CNT] = {
    
    // fragment shaders
    { "alphaTest.frag", alphaTest_frag, alphaTest_frag_len },
    { "clipPlane.frag", clipPlane_frag, clipPlane_frag_len },
    { "lighting.frag", lighting_frag, lighting_frag_len },
    { "lightingPerFragment.frag", lightingPerFragment_frag, lightingPerFragment_frag_len },
    { "main.frag", main_frag, main_frag_len },
    { "postprocess.frag", postprocess_frag, postprocess_frag_len },
    { "texture.frag", texture_frag, texture_frag_len },
    { "texture0.frag", texture0_frag, texture0_frag_len },
    { "texture1.frag", texture1_frag, texture1_frag_len },
    { "texture2.frag", texture2_frag, texture2_frag_len },
    
    // vertex shaders
    { "clipPlane.vert", clipPlane_vert, clipPlane_vert_len },
    { "lighting.vert", lighting_vert, lighting_vert_len },
    { "lightingPerFragment.vert", lightingPerFragment_vert, lightingPerFragment_vert_len },
    { "lightingPerVertex.vert", lightingPerVertex_vert, lightingPerVertex_vert_len },
    { "main.vert", main_vert, main_vert_len },
    { "postprocess.vert", postprocess_vert, postprocess_vert_len },
    { "texture.vert", texture_vert, texture_vert_len },
    { "texture0.vert", texture0_vert, texture0_vert_len },
    { "texture1.vert", texture1_vert, texture1_vert_len },
    { "texture2.vert", texture2_vert, texture2_vert_len },
    
    { "fog.glsl", fog_glsl, fog_glsl_len }
    
};
#endif
