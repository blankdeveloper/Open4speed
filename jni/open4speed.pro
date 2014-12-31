# -------------------------------------------------
# Project created by QtCreator 2011-11-11T16:55:36
# -------------------------------------------------
LIBS += -lGL \
    -lGLU \
    -lglut \
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
    input/keyboard.cpp \
    physics/bullet/bullet.cpp \
    input/airacer.cpp \
    open4speed.cpp \
    engine/switch.cpp \
    engine/math.cpp \
    engine/io.cpp \
    renderers/opengl/gles20.cpp \
    renderers/opengl/glsl.cpp \
    engine/car.cpp \
    engine/model.cpp \
    renderers/opengl/gltexture.cpp \
    renderers/opengl/glfbo.cpp \
    renderers/opengl/glvbo.cpp \
    physics/bullet/GL_ShapeDrawer.cpp \
    physics/bullet/GLDebugFont.cpp \
    files/extfile.cpp \
    files/zipfile.cpp
HEADERS += \
    input/keyboard.h \
    interfaces/input.h \
    interfaces/physics.h \
    physics/bullet/bullet.h \
    input/airacer.h \
    interfaces/renderer.h \
    interfaces/texture.h \
    engine/model.h \
    engine/switch.h \
    engine/math.h \
    engine/io.h \
    renderers/opengl/gles20.h \
    renderers/opengl/glsl.h \
    engine/car.h \
    engine/textures.hpp \
    interfaces/shader.h \
    renderers/opengl/gltexture.h \
    renderers/opengl/glfbo.h \
    interfaces/fbo.h \
    interfaces/vbo.h \
    renderers/opengl/glvbo.h \
    physics/bullet/GL_ShapeDrawer.h \
    physics/bullet/GLDebugFont.h \
    physics/bullet/GlutStuff.h \
    files/extfile.h \
    files/zipfile.h \
    interfaces/file.h
INCLUDEPATH += /usr/include/bullet
