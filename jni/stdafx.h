//----------------------------------------------------------------------------------------
/**
 * \file       stdafx.h
 * \author     Vonasek Lubos
 * \date       2014/02/14
 * \brief      Includition of common header files
*/
//----------------------------------------------------------------------------------------

#pragma once

#ifdef ANDROID
#define ZIP_ARCHIVE
#endif

#include <cstring>
#include <iostream>
#include <math.h>
#include <stack>
#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <png.h>
#include <zip.h>

#ifdef ANDROID
#include <jni.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
extern JNIEnv* instance;            ///< JNI instance
extern zip* APKArchive;             ///< APK archive instance
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include "interfaces/texture.h"
#include "interfaces/shader.h"
#include "interfaces/fbo.h"
#include "interfaces/vbo.h"
#include "interfaces/input.h"
#include "loaders/dynamiclight.h"
#include "interfaces/model.h"
#include "interfaces/sound.h"
#include "car.h"
#include "interfaces/physics.h"
#include "interfaces/renderer.h"
#include "physics/bullet.h"

#ifndef ANDROID
#include "sound/fmodapi.h"
#endif
#include "sound/soundpool.h"

#include "input/airacer.h"
#include "input/aitraffic.h"
#include "input/keyboard.h"

#include "utils/io.h"
#include "renderers/opengl/glvbo.h"
#include "loaders/modelo4s.h"
#include "renderers/opengl/gltexture.h"
#include "renderers/opengl/glfbo.h"
#include "renderers/opengl/gles20.h"
#include "renderers/opengl/glsl.h"
#include "loaders//pngloader.h"
#include "loaders//rgb.h"

#include "utils/engine.h"
#include "utils/math.h"
#include "utils/switch.h"
#include "common.h"
#include "utils/scripting.h"
