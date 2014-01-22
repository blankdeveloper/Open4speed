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
TARGET = open4speed
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += \
    components/input/keyboard.cpp \
    components/physics/bullet.cpp \
    components/input/airacer.cpp \
    components/input/aitraffic.cpp \
    open4speed.cpp \
    components/texture/rgb.cpp \
    components/model/modelo4s.cpp \
    utils/switch.cpp \
    utils/math.cpp \
    utils/io.cpp \
    utils/engine.cpp \
    components/renderer/gles20.cpp \
    components/sound/fmodapi.cpp \
    car.cpp \
    components/texture/pngloader.cpp \
    components/shader/glsl.cpp \
    components/texture/pxxloader.cpp \
    common.cpp \
    components/sound/soundpool.cpp \
    utils/scripting.cpp \
    components/interfaces/model.cpp
HEADERS += \
    components/input/keyboard.h \
    components/interfaces/input.h \
    components/interfaces/physics.h \
    components/physics/bullet.h \
    components/input/airacer.h \
    components/input/aitraffic.h \
    components/interfaces/renderer.h \
    components/interfaces/texture.h \
    components/texture/rgb.h \
    components/interfaces/model.h \
    components/model/modelo4s.h \
    components/interfaces/sound.h \
    utils/switch.h \
    utils/math.h \
    utils/io.h \
    utils/engine.h \
    components/renderer/gles20.h \
    components/sound/fmodapi.h \
    car.h \
    stdafx.h \
    components/texture/pngloader.h \
    components/interfaces/shader.h \
    components/shader/glsl.h \
    components/texture/pxxloader.h \
    common.h \
    components/sound/soundpool.h \
    utils/scripting.h
INCLUDEPATH += ../support ../support/fmodapi-4.44.08 /usr/include/bullet
