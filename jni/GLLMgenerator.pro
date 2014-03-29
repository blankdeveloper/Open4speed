# -------------------------------------------------
# Project created by QtCreator 2011-11-11T16:55:36
# -------------------------------------------------
LIBS += -lGL \
    -lglut \
    -lpng \
    -lzip \
    -lBulletDynamics \
    -lBulletCollision \
    -lLinearMath \
    ../support/fmodapi-4.44.08/libfmodex-4.44.08.so
TARGET = GLLMgenerator
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += \
    input/keyboard.cpp \
    physics/bullet.cpp \
    input/airacer.cpp \
    input/aitraffic.cpp \
    GLLMgenerator.cpp \
    loaders/dynamiclight.cpp \
    loaders/rgb.cpp \
    loaders/modelo4s.cpp \
    utils/switch.cpp \
    utils/math.cpp \
    utils/io.cpp \
    utils/engine.cpp \
    renderers/opengl/gles20.cpp \
    renderers/opengl/glsl.cpp \
    renderers/opengl/gltexture.cpp \
    sound/fmodapi.cpp \
    car.cpp \
    loaders/pngloader.cpp \
    common.cpp \
    sound/soundpool.cpp \
    utils/scripting.cpp \
    interfaces/model.cpp \
    renderers/opengl/glfbo.cpp \
    renderers/opengl/glvbo.cpp \
    raycaster/octreenode.cpp \
    raycaster/triangle.cpp \
    raycaster/utils.cpp
HEADERS += \
    input/keyboard.h \
    interfaces/input.h \
    interfaces/physics.h \
    physics/bullet.h \
    input/airacer.h \
    input/aitraffic.h \
    interfaces/renderer.h \
    interfaces/texture.h \
    loaders/dynamiclight.h \
    loaders/rgb.h \
    interfaces/model.h \
    loaders/modelo4s.h \
    interfaces/sound.h \
    utils/switch.h \
    utils/math.h \
    utils/io.h \
    utils/engine.h \
    renderers/opengl/gles20.h \
    renderers/opengl/glsl.h \
    renderers/opengl/gltexture.h \
    sound/fmodapi.h \
    car.h \
    stdafx.h \
    loaders/pngloader.h \
    interfaces/shader.h \
    common.h \
    sound/soundpool.h \
    utils/scripting.h \
    interfaces/fbo.h \
    renderers/opengl/glfbo.h \
    renderers/opengl/glvbo.h \
    raycaster/octreenode.h \
    raycaster/triangle.h \
    raycaster/utils.h
INCLUDEPATH += ../support ../support/fmodapi-4.44.08 ../support/bullet-2.81-rev2613/src
