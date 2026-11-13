#include "camera_manager.h"
#include "internal.h"

lc_camera_manager_t* lc_camera_manager_create(void) {
  auto mgr = new lc_camera_manager();
  mgr->cm = std::make_unique<CameraManager>();
  return mgr;
}

void lc_camera_manager_destroy(lc_camera_manager_t* mgr) {
  delete mgr;
}

int lc_camera_manager_start(lc_camera_manager_t* mgr) {
  return mgr->cm->start();
}

void lc_camera_manager_stop(lc_camera_manager_t* mgr) {
  mgr->cm->stop();
}

size_t lc_camera_manager_cameras_count(const lc_camera_manager_t* mgr) {
  return mgr->cm->cameras().size();
}

lc_camera_t* lc_camera_manager_get_camera(lc_camera_manager_t* mgr,
                                          size_t index) {
  auto cameras = mgr->cm->cameras();
  if (index >= cameras.size()) {
    return nullptr;
  }
  auto cam = new lc_camera();
  cam->camera = cameras[index];
  return cam;
}

lc_camera_t* lc_camera_manager_get_camera_by_id(lc_camera_manager_t* mgr,
                                                const char* id) {
  // CameraManager::get() は std::string_view を受け取る。
  // std::string_view の const char* 構築は char_traits::length を呼ぶため、
  // nullptr を渡すとその内部の strlen がヌル参照して未定義動作になる。
  // ここで null を弾くことで、プロセスごと落ちるのを防ぐ。
  // なお、nullptr 以外の不正なポインタまでは守れない。
  if (id == nullptr) {
    return nullptr;
  }
  auto camera = mgr->cm->get(id);
  if (!camera) {
    return nullptr;
  }
  auto cam = new lc_camera();
  cam->camera = camera;
  return cam;
}
