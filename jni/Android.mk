LOCAL_PATH := $(call my-dir)

#
# PNG Library
# 
LOCAL_PATH := ../support/libpng-1.6.3
include $(CLEAR_VARS)

LOCAL_MODULE    := libpng
LOCAL_SRC_FILES := \
png.c \
pngerror.c \
pngget.c \
pngmem.c \
pngpread.c \
pngread.c \
pngrio.c \
pngrtran.c \
pngrutil.c \
pngset.c \
pngtest.c \
pngtrans.c \
pngwio.c \
pngwrite.c \
pngwtran.c \
pngwutil.c
include $(BUILD_STATIC_LIBRARY)

#
# ZIP Library
# 
LOCAL_PATH := ../support/libzip-0.11.1/lib
include $(CLEAR_VARS)

LOCAL_MODULE    := libzip
LOCAL_SRC_FILES := \
  zip_add.c \
  zip_add_dir.c \
  zip_add_entry.c \
  zip_close.c \
  zip_delete.c \
  zip_dir_add.c \
  zip_dirent.c \
  zip_discard.c \
  zip_entry.c \
  zip_err_str.c \
  zip_error.c \
  zip_error_clear.c \
  zip_error_get.c \
  zip_error_get_sys_type.c \
  zip_error_strerror.c \
  zip_error_to_str.c \
  zip_extra_field.c \
  zip_extra_field_api.c \
  zip_fclose.c \
  zip_fdopen.c \
  zip_file_add.c \
  zip_file_error_clear.c \
  zip_file_error_get.c \
  zip_file_get_comment.c \
  zip_file_get_offset.c \
  zip_file_rename.c \
  zip_file_replace.c \
  zip_file_set_comment.c \
  zip_file_strerror.c \
  zip_filerange_crc.c \
  zip_fopen.c \
  zip_fopen_encrypted.c \
  zip_fopen_index.c \
  zip_fopen_index_encrypted.c \
  zip_fread.c \
  zip_get_archive_comment.c \
  zip_get_archive_flag.c \
  zip_get_compression_implementation.c \
  zip_get_encryption_implementation.c \
  zip_get_file_comment.c \
  zip_get_name.c \
  zip_get_num_entries.c \
  zip_get_num_files.c \
  zip_memdup.c \
  zip_name_locate.c \
  zip_new.c \
  zip_open.c \
  zip_rename.c \
  zip_replace.c \
  zip_set_archive_comment.c \
  zip_set_archive_flag.c \
  zip_set_default_password.c \
  zip_set_file_comment.c \
  zip_set_file_compression.c \
  zip_set_name.c \
  zip_source_buffer.c \
  zip_source_close.c \
  zip_source_crc.c \
  zip_source_deflate.c \
  zip_source_error.c \
  zip_source_file.c \
  zip_source_filep.c \
  zip_source_free.c \
  zip_source_function.c \
  zip_source_layered.c \
  zip_source_open.c \
  zip_source_pkware.c \
  zip_source_pop.c \
  zip_source_read.c \
  zip_source_stat.c \
  zip_source_window.c \
  zip_source_zip.c \
  zip_source_zip_new.c \
  zip_stat.c \
  zip_stat_index.c \
  zip_stat_init.c \
  zip_strerror.c \
  zip_string.c \
  zip_unchange.c \
  zip_unchange_all.c \
  zip_unchange_archive.c \
  zip_unchange_data.c \
  zip_utf-8.c
include $(BUILD_STATIC_LIBRARY)

#
# Bullet Physics Static Library
# 
LOCAL_PATH := ../support/bullet-2.81-rev2613/src
include $(CLEAR_VARS)

LOCAL_MODULE    := libbullet
LOCAL_SRC_FILES := \
BulletCollision/BroadphaseCollision/btAxisSweep3.cpp \
BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp \
BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp \
BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp \
BulletCollision/BroadphaseCollision/btDbvt.cpp \
BulletCollision/BroadphaseCollision/btDispatcher.cpp \
BulletCollision/BroadphaseCollision/btMultiSapBroadphase.cpp \
BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp \
BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp \
BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp \
BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp \
BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp \
BulletCollision/CollisionDispatch/btCollisionObject.cpp \
BulletCollision/CollisionDispatch/btCollisionWorld.cpp \
BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp \
BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp \
BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp \
BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btGhostObject.cpp \
BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp \
BulletCollision/CollisionDispatch/btManifoldResult.cpp \
BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp \
BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp \
BulletCollision/CollisionDispatch/btUnionFind.cpp \
BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp \
BulletCollision/CollisionShapes/btBoxShape.cpp \
BulletCollision/CollisionShapes/btBox2dShape.cpp \
BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp \
BulletCollision/CollisionShapes/btCapsuleShape.cpp \
BulletCollision/CollisionShapes/btCollisionShape.cpp \
BulletCollision/CollisionShapes/btCompoundShape.cpp \
BulletCollision/CollisionShapes/btConcaveShape.cpp \
BulletCollision/CollisionShapes/btConeShape.cpp \
BulletCollision/CollisionShapes/btConvexHullShape.cpp \
BulletCollision/CollisionShapes/btConvexInternalShape.cpp \
BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp \
BulletCollision/CollisionShapes/btConvexPolyhedron.cpp \
BulletCollision/CollisionShapes/btConvexShape.cpp \
BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp \
BulletCollision/CollisionShapes/btConvex2dShape.cpp \
BulletCollision/CollisionShapes/btCylinderShape.cpp \
BulletCollision/CollisionShapes/btEmptyShape.cpp \
BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp \
BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp \
BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp \
BulletCollision/CollisionShapes/btMultiSphereShape.cpp \
BulletCollision/CollisionShapes/btOptimizedBvh.cpp \
BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp \
BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp \
BulletCollision/CollisionShapes/btShapeHull.cpp \
BulletCollision/CollisionShapes/btSphereShape.cpp \
BulletCollision/CollisionShapes/btStaticPlaneShape.cpp \
BulletCollision/CollisionShapes/btStridingMeshInterface.cpp \
BulletCollision/CollisionShapes/btTetrahedronShape.cpp \
BulletCollision/CollisionShapes/btTriangleBuffer.cpp \
BulletCollision/CollisionShapes/btTriangleCallback.cpp \
BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp \
BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp \
BulletCollision/CollisionShapes/btTriangleMesh.cpp \
BulletCollision/CollisionShapes/btTriangleMeshShape.cpp \
BulletCollision/CollisionShapes/btUniformScalingShape.cpp \
BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp \
BulletCollision/NarrowPhaseCollision/btConvexCast.cpp \
BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp \
BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp \
BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp \
BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp \
BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp \
BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp \
BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp \
BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp \
BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp \
BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp \
BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp \
BulletDynamics/ConstraintSolver/btContactConstraint.cpp \
BulletDynamics/ConstraintSolver/btGearConstraint.cpp \
BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp \
BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp \
BulletDynamics/ConstraintSolver/btHingeConstraint.cpp \
BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp \
BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp \
BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp \
BulletDynamics/ConstraintSolver/btSliderConstraint.cpp \
BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp \
BulletDynamics/ConstraintSolver/btTypedConstraint.cpp \
BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp \
BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp \
BulletDynamics/Dynamics/btRigidBody.cpp \
BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp \
BulletDynamics/Dynamics/Bullet-C-API.cpp \
BulletDynamics/Character/btKinematicCharacterController.cpp \
BulletDynamics/Vehicle/btRaycastVehicle.cpp \
BulletDynamics/Vehicle/btWheelInfo.cpp \
LinearMath/btAlignedAllocator.cpp \
LinearMath/btConvexHullComputer.cpp \
LinearMath/btConvexHull.cpp \
LinearMath/btGeometryUtil.cpp \
LinearMath/btPolarDecomposition.cpp \
LinearMath/btQuickprof.cpp \
LinearMath/btSerializer.cpp \
LinearMath/btVector3.cpp
include $(BUILD_STATIC_LIBRARY)

#
# Open4Speed by L.Vonasek engine
# 
include $(CLEAR_VARS)
LOCAL_PATH := .

LOCAL_MODULE := open4speed

LOCAL_CFLAGS := -DANDROID_NDK

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
$(LOCAL_PATH)/../support/libpng-1.6.3 \
$(LOCAL_PATH)/../support/libzip-0.11.1/lib \
$(LOCAL_PATH)/../support/bullet-2.81-rev2613/src \
$(LOCAL_PATH)/../support

LOCAL_SRC_FILES := \
open4speed.cpp \
car.cpp \
components/physics/bullet.cpp \
components/input/keyboard.cpp \
components/input/airacer.cpp \
components/input/aitraffic.cpp \
components/interfaces/model.cpp \
components/renderer/opengl/gles20.cpp \
components/renderer/opengl/glsl.cpp \
components/renderer/opengl/gltexture.cpp \
components/renderer/opengl/glfbo.cpp \
components/texture/pngloader.cpp \
components/texture/rgb.cpp \
components/model/modelo4s.cpp \
components/sound/soundpool.cpp \
utils/switch.cpp \
utils/math.cpp \
utils/io.cpp \
utils/engine.cpp \
utils/scripting.cpp \
common.cpp
LOCAL_LDLIBS := -lGLESv2 -ldl -llog -landroid -lz

LOCAL_STATIC_LIBRARIES := libpng \ libbullet \ libzip

include $(BUILD_SHARED_LIBRARY)
