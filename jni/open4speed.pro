# -------------------------------------------------
# Project created by QtCreator 2011-11-11T16:55:36
# -------------------------------------------------
LIBS += -lglut \
    -lGL \
    -lGLU \
    -lX11 \
    -lpng \
    -lzip \
    -lBulletDynamics \
    -lBulletCollision \
    -lLinearMath
TARGET = open4speed
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += \
    engine/car.cpp \
    engine/io.cpp \
    engine/math.cpp \
    engine/model.cpp \
    engine/switch.cpp \
    files/extfile.cpp \
    files/zipfile.cpp \
    input/airacer.cpp \
    input/keyboard.cpp \
    physics/bullet/bullet.cpp \
    physics/bullet/GL_ShapeDrawer.cpp \
    physics/bullet/GLDebugFont.cpp \
    renderers/opengl/gles20.cpp \
    renderers/opengl/glfbo.cpp \
    renderers/opengl/glsl.cpp \
    renderers/opengl/gltexture.cpp \
    renderers/opengl/glvbo.cpp \
    renderers/simple/simple.cpp \
    open4speed.cpp
HEADERS += \
    engine/car.h \
    engine/io.h \
    engine/math.h \
    engine/model.h \
    engine/switch.h \
    engine/textures.hpp \
    files/extfile.h \
    files/zipfile.h \
    input/airacer.h \
    input/keyboard.h \
    interfaces/fbo.h \
    interfaces/file.h \
    interfaces/input.h \
    interfaces/physics.h \
    interfaces/renderer.h \
    interfaces/shader.h \
    interfaces/texture.h \
    interfaces/vbo.h \
    physics/bullet/bullet.h \
    physics/bullet/GL_ShapeDrawer.h \
    physics/bullet/GLDebugFont.h \
    physics/bullet/GlutStuff.h \
    renderers/opengl/gles20.h \
    renderers/opengl/glfbo.h \
    renderers/opengl/glsl.h \
    renderers/opengl/gltexture.h \
    renderers/opengl/glvbo.h \
    renderers/simple/simple.h
INCLUDEPATH += /usr/include/bullet
