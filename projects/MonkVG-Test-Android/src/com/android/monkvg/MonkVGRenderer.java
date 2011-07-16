/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.monkvg;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;

public class MonkVGRenderer implements GLSurfaceView.Renderer {
    Context _context;
    
    public MonkVGRenderer(Context context)
    {
        _context = context;
    }
    
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        MonkVGJNILib.created();
    }
    
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
       if (height == 0) {
           height = 1;   // To prevent divide by zero
       }
       MonkVGJNILib.changed(width, height);
    }
    
    // Call back to draw the current frame.
    @Override
    public void onDrawFrame(GL10 gl) {
        MonkVGJNILib.step();
    }

}
