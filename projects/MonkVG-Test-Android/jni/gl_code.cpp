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

// OpenGL ES 1.1 code

#include <jni.h>
#include <android/log.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <MonkVG/openvg.h>
#include <MonkVG/vgu.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define  LOG_TAG    "libmonkvg"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

extern "C" {
    JNIEXPORT void JNICALL Java_com_android_monkvg_MonkVGJNILib_created(JNIEnv * env, jobject obj);
    JNIEXPORT void JNICALL Java_com_android_monkvg_MonkVGJNILib_changed(JNIEnv * env, jobject obj,  jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_monkvg_MonkVGJNILib_step(JNIEnv * env, jobject obj);
};

VGPath  _path;
int _width = 0;
int _height = 0;

JNIEXPORT void JNICALL Java_com_android_monkvg_MonkVGJNILib_created(JNIEnv * env, jobject obj)
{
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DITHER);

    _path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1, 0, 0, 0, VG_PATH_CAPABILITY_ALL);
    vguEllipse( _path, 0, 0, 90.0f, 50.0f );
}

JNIEXPORT void JNICALL Java_com_android_monkvg_MonkVGJNILib_changed(JNIEnv * env, jobject obj,  jint width, jint height)
{
    LOGI("w:%d h:%d", width, height);
    _width = width;
    _height = height;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
}

GLfloat vertexBuffer[] =
{
   100.0f, 100.0f, 0.0f,
    50.0f,  50.0f, 0.0f,
   150.0f,  50.0f, 0.0f
};

GLubyte indexBuffer[] = { 0, 1, 2 };

JNIEXPORT void JNICALL Java_com_android_monkvg_MonkVGJNILib_step(JNIEnv * env, jobject obj)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    VGfloat clearColor[] = {1,1,1,1};
    vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
    vgClear(0, 0, _width, _height);

    vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
    vgLoadIdentity();
    vgTranslate( _width/2, _height/2 );
    vgDrawPath( _path, VG_FILL_PATH );
}
