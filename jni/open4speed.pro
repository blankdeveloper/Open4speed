# -------------------------------------------------
# Project created by QtCreator 2011-11-11T16:55:36
# -------------------------------------------------
LIBS += -lglut \
    -lGL \
    -lGLU \
    -lX11 \
    -lpng \
    -lzip
TARGET = open4speed
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += \
    ../support/bullet3-2.83.7/BulletCollision/BroadphaseCollision/*.cpp \
    ../support/bullet3-2.83.7/BulletCollision/CollisionDispatch/*.cpp \
    ../support/bullet3-2.83.7/BulletCollision/CollisionShapes/*.cpp \
    ../support/bullet3-2.83.7/BulletCollision/Gimpact/*.cpp \
    ../support/bullet3-2.83.7/BulletCollision/NarrowPhaseCollision/*.cpp \
    ../support/bullet3-2.83.7/BulletDynamics/ConstraintSolver/*.cpp \
    ../support/bullet3-2.83.7/BulletDynamics/Dynamics/*.cpp \
    ../support/bullet3-2.83.7/BulletDynamics/Featherstone/*.cpp \
    ../support/bullet3-2.83.7/BulletDynamics/Character/*.cpp \
    ../support/bullet3-2.83.7/BulletDynamics/MLCPSolvers/*.cpp \
    ../support/bullet3-2.83.7/BulletDynamics/Vehicle/*.cpp \
    ../support/bullet3-2.83.7/LinearMath/*.cpp \
    engine/car.cpp \
    engine/io.cpp \
    engine/math.cpp \
    engine/matrices.cpp \
    engine/model.cpp \
    engine/switch.cpp \
    files/extfile.cpp \
    files/zipfile.cpp \
    input/airacer.cpp \
    input/keyboard.cpp \
    physics/bullet/bullet.cpp \
    renderers/opengl/gles20.cpp \
    renderers/opengl/glsl.cpp \
    renderers/opengl/gltexture.cpp \
    open4speed.cpp
HEADERS += \
    engine/car.h \
    engine/io.h \
    engine/math.h \
    engine/matrices.h \
    engine/model.h \
    engine/switch.h \
    files/extfile.h \
    files/zipfile.h \
    input/airacer.h \
    input/keyboard.h \
    interfaces/file.h \
    interfaces/input.h \
    interfaces/physics.h \
    interfaces/renderer.h \
    interfaces/shader.h \
    interfaces/texture.h \
    physics/bullet/bullet.h \
    renderers/opengl/gles20.h \
    renderers/opengl/glsl.h \
    renderers/opengl/gltexture.h
INCLUDEPATH += ../support/bullet3-2.83.7
