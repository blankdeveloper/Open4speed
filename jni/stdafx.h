//----------------------------------------------------------------------------------------
/**
 * \file       stdafx.h
 * \author     Vonasek Lubos
 * \date       2014/01/05
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

#include "components/interfaces/texture.h"
#include "components/interfaces/shader.h"
#include "components/interfaces/fbo.h"
#include "components/interfaces/input.h"
#include "components/interfaces/model.h"
#include "components/interfaces/sound.h"
#include "car.h"
#include "components/interfaces/physics.h"
#include "components/interfaces/renderer.h"
#include "components/physics/bullet.h"

#ifndef ANDROID
#include "components/sound/fmodapi.h"
#endif
#include "components/sound/soundpool.h"

#include "components/input/airacer.h"
#include "components/input/aitraffic.h"
#include "components/input/keyboard.h"

#include "utils/io.h"
#include "components/model/modelo4s.h"
#include "components/renderer/opengl/gltexture.h"
#include "components/renderer/opengl/glfbo.h"
#include "components/renderer/opengl/gles20.h"
#include "components/renderer/opengl/glsl.h"
#include "components/texture/pngloader.h"
#include "components/texture/rgb.h"

#include "utils/engine.h"
#include "utils/math.h"
#include "utils/switch.h"
#include "common.h"
#include "utils/scripting.h"
