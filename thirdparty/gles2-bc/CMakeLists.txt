cmake_minimum_required(VERSION 3.0.0)
project(gles2-bc VERSION 0.1.0)

add_library(gles2-bc
        ./Sources/OpenGLES/OpenGLESConfig.cpp 
        ./Sources/OpenGLES/OpenGLESContext.cpp 
        ./Sources/OpenGLES/OpenGLESFile.cpp 
        ./Sources/OpenGLES/OpenGLESImplementation.cpp 
        ./Sources/OpenGLES/OpenGLESString.cpp 
        ./Sources/OpenGLES/OpenGLESUtil.cpp 
        ./Sources/OpenGLES/OpenGLES11/OpenGLES11Context.cpp 
        ./Sources/OpenGLES/OpenGLES11/OpenGLES11Implementation.cpp 
        ./Sources/OpenGLES/OpenGLES20/Attribute.cpp 
        ./Sources/OpenGLES/OpenGLES20/MatrixStack.cpp 
        ./Sources/OpenGLES/OpenGLES20/OpenGLES20Context.cpp 
        ./Sources/OpenGLES/OpenGLES20/OpenGLES20Implementation.cpp 
        ./Sources/OpenGLES/OpenGLES20/OpenGLESState.cpp 
        ./Sources/OpenGLES/OpenGLES20/Shader.cpp 
        ./Sources/OpenGLES/OpenGLES20/ShaderFile.cpp 
        ./Sources/OpenGLES/OpenGLES20/ShaderProgram.cpp 
        ./Sources/OpenGLES/OpenGLES20/ShaderSource.cpp 
        ./Sources/OpenGLES/OpenGLES20/Uniform.cpp
)

target_include_directories(gles2-bc PUBLIC ./Sources/OpenGLES ./Sources/OpenGLES/OpenGLES20 OpenGLES11)