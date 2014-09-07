# -------------------------------------------------
# Project created by QtCreator 2011-11-11T16:55:36
# -------------------------------------------------
LIBS += -lGL \
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
    physics/bullet.cpp \
    input/airacer.cpp \
    open4speed.cpp \
    loaders/rgb.cpp \
    loaders/modelo4s.cpp \
    utils/switch.cpp \
    utils/math.cpp \
    utils/io.cpp \
    renderers/opengl/gles20.cpp \
    renderers/opengl/glsl.cpp \
    car.cpp \
    loaders/pngloader.cpp \
    common.cpp \
    sound/soundpool.cpp \
    interfaces/model.cpp \
    renderers/opengl/gltexture.cpp \
    renderers/opengl/glfbo.cpp \
    renderers/opengl/glvbo.cpp
HEADERS += \
    input/keyboard.h \
    interfaces/input.h \
    interfaces/physics.h \
    physics/bullet.h \
    input/airacer.h \
    interfaces/renderer.h \
    interfaces/texture.h \
    loaders/rgb.h \
    interfaces/model.h \
    loaders/modelo4s.h \
    interfaces/sound.h \
    utils/switch.h \
    utils/math.h \
    utils/io.h \
    renderers/opengl/gles20.h \
    renderers/opengl/glsl.h \
    car.h \
    loaders/pngloader.h \
    interfaces/shader.h \
    common.h \
    sound/soundpool.h \
    renderers/opengl/gltexture.h \
    renderers/opengl/glfbo.h \
    interfaces/fbo.h \
    interfaces/vbo.h \
    renderers/opengl/glvbo.h
INCLUDEPATH += /usr/include/bullet
