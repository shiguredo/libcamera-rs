#ifndef LC_INTERNAL_H
#define LC_INTERNAL_H

#include "types.h"

#include <libcamera/camera.h>
#include <libcamera/camera_manager.h>
#include <libcamera/controls.h>
#include <libcamera/framebuffer.h>
#include <libcamera/framebuffer_allocator.h>
#include <libcamera/request.h>
#include <libcamera/stream.h>

#include <memory>

using namespace libcamera;

/* 内部構造体 */

struct RequestCallbackWrapper {
  lc_request_completed_cb cb = nullptr;
  void* user_data = nullptr;
};

struct lc_camera_manager {
  std::unique_ptr<CameraManager> cm;
};

struct lc_camera {
  std::shared_ptr<Camera> camera;
  RequestCallbackWrapper callback;
};

/* CameraConfiguration は unique_ptr::release() で raw ポインタとして返す */
struct lc_camera_configuration {
  CameraConfiguration* config;
};

/* StreamConfiguration は CameraConfiguration 内の参照 */
struct lc_stream_configuration {
  StreamConfiguration* sc;
};

/* Request は unique_ptr::release() で返す */
struct lc_request {
  Request* request;
};

/* FrameBuffer は Allocator が所有するバッファへのポインタ */
struct lc_frame_buffer {
  FrameBuffer* fb;
};

struct lc_frame_buffer_allocator {
  std::unique_ptr<FrameBufferAllocator> alloc;
};

/* Stream は Camera 内部データへの不透明参照 */
struct lc_stream {
  Stream* stream;
};

/* ControlList は owned フラグ付き */
struct lc_control_list {
  ControlList* list;
  bool owned;
};

/* ヘルパー: StreamRole 変換 */
inline StreamRole to_stream_role(lc_stream_role_t role) {
  switch (role) {
    case LC_STREAM_ROLE_RAW:
      return StreamRole::Raw;
    case LC_STREAM_ROLE_STILL_CAPTURE:
      return StreamRole::StillCapture;
    case LC_STREAM_ROLE_VIDEO_RECORDING:
      return StreamRole::VideoRecording;
    case LC_STREAM_ROLE_VIEWFINDER:
      return StreamRole::Viewfinder;
    default:
      return StreamRole::Viewfinder;
  }
}

#endif /* LC_INTERNAL_H */
