#-------------------------------------------------
#
# Project created by QtCreator 2018-10-06T21:28:36
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += charts
QT       += serialport
QT       += network
RC_ICONS = dragon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ./utils ./inc ./opencv-win/include ./set ./usb_device

TARGET = WasteRecycle
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        wasterecycle.cpp \
    pricesetdialog.cpp \
    sqloper.cpp \
    utilitytools.cpp \
    utils/json/json_internalarray.inl \
    utils/json/json_internalmap.inl \
    utils/json/json_reader.cpp \
    utils/json/json_value.cpp \
    utils/json/json_valueiterator.inl \
    utils/json/json_writer.cpp \
    cameramodel.cpp \
    opennetstream.cpp \
    monthlystatics.cpp \
    cv_help.cpp \
    image_base64.cpp \
    image_buf.cpp \
    liveness.cpp \
    manager.cpp \
    test_face.cpp \
    compare.cpp \
    set/setting.cpp \
    usb_device/device_enumerator.cpp

HEADERS += \
        wasterecycle.h \
    pricesetdialog.h \
    sqloper.h \
    utilitytools.h \
    inc/OpenNetStreamDefine.h \
    inc/OpenNetStreamError.h \
    inc/OpenNetStreamInterFace.h \
    utils/json/autolink.h \
    utils/json/config.h \
    utils/json/features.h \
    utils/json/forwards.h \
    utils/json/json.h \
    utils/json/json_batchallocator.h \
    utils/json/reader.h \
    utils/json/value.h \
    utils/json/writer.h \
    cameramodel.h \
    opennetstream.h \
    opennetstream_p.h \
    monthlystatics.h \
    inc/baidu_face_api.h \
    inc/hjimi_camera.h \
    inc/OpenNetStreamDefine.h \
    inc/OpenNetStreamError.h \
    inc/OpenNetStreamInterFace.h \
    inc/orbe_camera.h \
    opencv-win/include/opencv/cv.h \
    opencv-win/include/opencv/cv.hpp \
    opencv-win/include/opencv/cvaux.h \
    opencv-win/include/opencv/cvaux.hpp \
    opencv-win/include/opencv/cvwimage.h \
    opencv-win/include/opencv/cxcore.h \
    opencv-win/include/opencv/cxcore.hpp \
    opencv-win/include/opencv/cxeigen.hpp \
    opencv-win/include/opencv/cxmisc.h \
    opencv-win/include/opencv/highgui.h \
    opencv-win/include/opencv/ml.h \
    opencv-win/include/opencv2/calib3d/calib3d.hpp \
    opencv-win/include/opencv2/calib3d/calib3d_c.h \
    opencv-win/include/opencv2/core/hal/hal.hpp \
    opencv-win/include/opencv2/core/hal/interface.h \
    opencv-win/include/opencv2/core/hal/intrin.hpp \
    opencv-win/include/opencv2/core/hal/intrin_cpp.hpp \
    opencv-win/include/opencv2/core/hal/intrin_neon.hpp \
    opencv-win/include/opencv2/core/hal/intrin_sse.hpp \
    opencv-win/include/opencv2/core/affine.hpp \
    opencv-win/include/opencv2/core/base.hpp \
    opencv-win/include/opencv2/core/bufferpool.hpp \
    opencv-win/include/opencv2/core/core.hpp \
    opencv-win/include/opencv2/core/core_c.h \
    opencv-win/include/opencv2/core/cuda.hpp \
    opencv-win/include/opencv2/core/cuda.inl.hpp \
    opencv-win/include/opencv2/core/cuda_stream_accessor.hpp \
    opencv-win/include/opencv2/core/cuda_types.hpp \
    opencv-win/include/opencv2/core/cvdef.h \
    opencv-win/include/opencv2/core/cvstd.hpp \
    opencv-win/include/opencv2/core/cvstd.inl.hpp \
    opencv-win/include/opencv2/core/directx.hpp \
    opencv-win/include/opencv2/core/eigen.hpp \
    opencv-win/include/opencv2/core/fast_math.hpp \
    opencv-win/include/opencv2/core/ippasync.hpp \
    opencv-win/include/opencv2/core/mat.hpp \
    opencv-win/include/opencv2/core/mat.inl.hpp \
    opencv-win/include/opencv2/core/matx.hpp \
    opencv-win/include/opencv2/core/neon_utils.hpp \
    opencv-win/include/opencv2/core/ocl.hpp \
    opencv-win/include/opencv2/core/ocl_genbase.hpp \
    opencv-win/include/opencv2/core/opengl.hpp \
    opencv-win/include/opencv2/core/operations.hpp \
    opencv-win/include/opencv2/core/optim.hpp \
    opencv-win/include/opencv2/core/ovx.hpp \
    opencv-win/include/opencv2/core/persistence.hpp \
    opencv-win/include/opencv2/core/private.cuda.hpp \
    opencv-win/include/opencv2/core/private.hpp \
    opencv-win/include/opencv2/core/ptr.inl.hpp \
    opencv-win/include/opencv2/core/saturate.hpp \
    opencv-win/include/opencv2/core/sse_utils.hpp \
    opencv-win/include/opencv2/core/traits.hpp \
    opencv-win/include/opencv2/core/types.hpp \
    opencv-win/include/opencv2/core/types_c.h \
    opencv-win/include/opencv2/core/utility.hpp \
    opencv-win/include/opencv2/core/va_intel.hpp \
    opencv-win/include/opencv2/core/version.hpp \
    opencv-win/include/opencv2/core/wimage.hpp \
    opencv-win/include/opencv2/features2d/features2d.hpp \
    opencv-win/include/opencv2/flann/all_indices.h \
    opencv-win/include/opencv2/flann/allocator.h \
    opencv-win/include/opencv2/flann/any.h \
    opencv-win/include/opencv2/flann/autotuned_index.h \
    opencv-win/include/opencv2/flann/composite_index.h \
    opencv-win/include/opencv2/flann/config.h \
    opencv-win/include/opencv2/flann/defines.h \
    opencv-win/include/opencv2/flann/dist.h \
    opencv-win/include/opencv2/flann/dummy.h \
    opencv-win/include/opencv2/flann/dynamic_bitset.h \
    opencv-win/include/opencv2/flann/flann.hpp \
    opencv-win/include/opencv2/flann/flann_base.hpp \
    opencv-win/include/opencv2/flann/general.h \
    opencv-win/include/opencv2/flann/ground_truth.h \
    opencv-win/include/opencv2/flann/hdf5.h \
    opencv-win/include/opencv2/flann/heap.h \
    opencv-win/include/opencv2/flann/hierarchical_clustering_index.h \
    opencv-win/include/opencv2/flann/index_testing.h \
    opencv-win/include/opencv2/flann/kdtree_index.h \
    opencv-win/include/opencv2/flann/kdtree_single_index.h \
    opencv-win/include/opencv2/flann/kmeans_index.h \
    opencv-win/include/opencv2/flann/linear_index.h \
    opencv-win/include/opencv2/flann/logger.h \
    opencv-win/include/opencv2/flann/lsh_index.h \
    opencv-win/include/opencv2/flann/lsh_table.h \
    opencv-win/include/opencv2/flann/matrix.h \
    opencv-win/include/opencv2/flann/miniflann.hpp \
    opencv-win/include/opencv2/flann/nn_index.h \
    opencv-win/include/opencv2/flann/object_factory.h \
    opencv-win/include/opencv2/flann/params.h \
    opencv-win/include/opencv2/flann/random.h \
    opencv-win/include/opencv2/flann/result_set.h \
    opencv-win/include/opencv2/flann/sampling.h \
    opencv-win/include/opencv2/flann/saving.h \
    opencv-win/include/opencv2/flann/simplex_downhill.h \
    opencv-win/include/opencv2/flann/timer.h \
    opencv-win/include/opencv2/highgui/highgui.hpp \
    opencv-win/include/opencv2/highgui/highgui_c.h \
    opencv-win/include/opencv2/imgcodecs/imgcodecs.hpp \
    opencv-win/include/opencv2/imgcodecs/imgcodecs_c.h \
    opencv-win/include/opencv2/imgcodecs/ios.h \
    opencv-win/include/opencv2/imgproc/detail/distortion_model.hpp \
    opencv-win/include/opencv2/imgproc/hal/hal.hpp \
    opencv-win/include/opencv2/imgproc/hal/interface.h \
    opencv-win/include/opencv2/imgproc/imgproc.hpp \
    opencv-win/include/opencv2/imgproc/imgproc_c.h \
    opencv-win/include/opencv2/imgproc/types_c.h \
    opencv-win/include/opencv2/ml/ml.hpp \
    opencv-win/include/opencv2/objdetect/detection_based_tracker.hpp \
    opencv-win/include/opencv2/objdetect/objdetect.hpp \
    opencv-win/include/opencv2/objdetect/objdetect_c.h \
    opencv-win/include/opencv2/photo/cuda.hpp \
    opencv-win/include/opencv2/photo/photo.hpp \
    opencv-win/include/opencv2/photo/photo_c.h \
    opencv-win/include/opencv2/shape/emdL1.hpp \
    opencv-win/include/opencv2/shape/hist_cost.hpp \
    opencv-win/include/opencv2/shape/shape.hpp \
    opencv-win/include/opencv2/shape/shape_distance.hpp \
    opencv-win/include/opencv2/shape/shape_transformer.hpp \
    opencv-win/include/opencv2/stitching/detail/autocalib.hpp \
    opencv-win/include/opencv2/stitching/detail/blenders.hpp \
    opencv-win/include/opencv2/stitching/detail/camera.hpp \
    opencv-win/include/opencv2/stitching/detail/exposure_compensate.hpp \
    opencv-win/include/opencv2/stitching/detail/matchers.hpp \
    opencv-win/include/opencv2/stitching/detail/motion_estimators.hpp \
    opencv-win/include/opencv2/stitching/detail/seam_finders.hpp \
    opencv-win/include/opencv2/stitching/detail/timelapsers.hpp \
    opencv-win/include/opencv2/stitching/detail/util.hpp \
    opencv-win/include/opencv2/stitching/detail/util_inl.hpp \
    opencv-win/include/opencv2/stitching/detail/warpers.hpp \
    opencv-win/include/opencv2/stitching/detail/warpers_inl.hpp \
    opencv-win/include/opencv2/stitching/warpers.hpp \
    opencv-win/include/opencv2/superres/optical_flow.hpp \
    opencv-win/include/opencv2/video/background_segm.hpp \
    opencv-win/include/opencv2/video/tracking.hpp \
    opencv-win/include/opencv2/video/tracking_c.h \
    opencv-win/include/opencv2/video/video.hpp \
    opencv-win/include/opencv2/videoio/cap_ios.h \
    opencv-win/include/opencv2/videoio/videoio.hpp \
    opencv-win/include/opencv2/videoio/videoio_c.h \
    opencv-win/include/opencv2/videostab/deblurring.hpp \
    opencv-win/include/opencv2/videostab/fast_marching.hpp \
    opencv-win/include/opencv2/videostab/fast_marching_inl.hpp \
    opencv-win/include/opencv2/videostab/frame_source.hpp \
    opencv-win/include/opencv2/videostab/global_motion.hpp \
    opencv-win/include/opencv2/videostab/inpainting.hpp \
    opencv-win/include/opencv2/videostab/log.hpp \
    opencv-win/include/opencv2/videostab/motion_core.hpp \
    opencv-win/include/opencv2/videostab/motion_stabilizing.hpp \
    opencv-win/include/opencv2/videostab/optical_flow.hpp \
    opencv-win/include/opencv2/videostab/outlier_rejection.hpp \
    opencv-win/include/opencv2/videostab/ring_buffer.hpp \
    opencv-win/include/opencv2/videostab/stabilizer.hpp \
    opencv-win/include/opencv2/videostab/wobble_suppression.hpp \
    opencv-win/include/opencv2/calib3d.hpp \
    opencv-win/include/opencv2/core.hpp \
    opencv-win/include/opencv2/cvconfig.h \
    opencv-win/include/opencv2/features2d.hpp \
    opencv-win/include/opencv2/flann.hpp \
    opencv-win/include/opencv2/highgui.hpp \
    opencv-win/include/opencv2/imgcodecs.hpp \
    opencv-win/include/opencv2/imgproc.hpp \
    opencv-win/include/opencv2/ml.hpp \
    opencv-win/include/opencv2/objdetect.hpp \
    opencv-win/include/opencv2/opencv.hpp \
    opencv-win/include/opencv2/opencv_modules.hpp \
    opencv-win/include/opencv2/photo.hpp \
    opencv-win/include/opencv2/shape.hpp \
    opencv-win/include/opencv2/stitching.hpp \
    opencv-win/include/opencv2/superres.hpp \
    opencv-win/include/opencv2/video.hpp \
    opencv-win/include/opencv2/videoio.hpp \
    opencv-win/include/opencv2/videostab.hpp \
    opencv-win/include/opencv2/world.hpp \
    set/setting.h \
    usb_device/device_enumerator.h \
    compare.h \
    cv_help.h \
    image_base64.h \
    image_buf.h \
    liveness.h \
    manager.h \
    test_face.h

FORMS += \
        wasterecycle.ui \
    pricesetdialog.ui \
    monthlystatics.ui

CONFIG += c++11

DISTFILES +=

RESOURCES += \
    imgs.qrc

LIBS += -L$$PWD/lib/win32/ -lOpenNetStream
LIBS += -L$$PWD/lib/x86/ -lBaiduFaceApi
LIBS += -L$$PWD/lib/x86/ -lopencv_world320
LIBS += -L$$PWD/lib/x86/ -ljson_vc71_libmt
LIBS += -L$$PWD/lib/x86/ -lhjimi_camera
LIBS += -L$$PWD/lib/x86/ -lorbe_camera
LIBS += -L$$PWD/lib/x86/ -lOle32
LIBS += -L$$PWD/lib/x86/ -lOleAut32

INCLUDEPATH += $$PWD/lib/win32
DEPENDPATH += $$PWD/lib/win32
