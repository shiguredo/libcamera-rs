#include "camera.h"
#include "internal.h"

#include <cstring>
#include <vector>

const char* lc_camera_id(const lc_camera_t* cam) {
  return cam->camera->id().c_str();
}

int lc_camera_acquire(lc_camera_t* cam) {
  return cam->camera->acquire();
}

int lc_camera_release(lc_camera_t* cam) {
  return cam->camera->release();
}

void lc_camera_release_ref(lc_camera_t* cam) {
  delete cam;
}

lc_camera_configuration_t* lc_camera_generate_configuration(
    lc_camera_t* cam,
    const lc_stream_role_t* roles,
    size_t roles_count) {
  std::vector<StreamRole> cpp_roles;
  cpp_roles.reserve(roles_count);
  for (size_t i = 0; i < roles_count; i++) {
    cpp_roles.push_back(to_stream_role(roles[i]));
  }
  auto config = cam->camera->generateConfiguration(cpp_roles);
  if (!config) {
    return nullptr;
  }
  auto lc_config = new lc_camera_configuration();
  lc_config->config = config.release();
  return lc_config;
}

int lc_camera_configure(lc_camera_t* cam, lc_camera_configuration_t* config) {
  return cam->camera->configure(config->config);
}

lc_request_t* lc_camera_create_request(lc_camera_t* cam, uint64_t cookie) {
  auto request = cam->camera->createRequest(cookie);
  if (!request) {
    return nullptr;
  }
  auto lc_req = new lc_request();
  lc_req->request = request.release();
  return lc_req;
}

int lc_camera_queue_request(lc_camera_t* cam, lc_request_t* request) {
  return cam->camera->queueRequest(request->request);
}

int lc_camera_start(lc_camera_t* cam) {
  return cam->camera->start();
}

int lc_camera_stop(lc_camera_t* cam) {
  return cam->camera->stop();
}

void lc_camera_connect_request_completed(lc_camera_t* cam,
                                         lc_request_completed_cb cb,
                                         void* user_data) {
  cam->callback.cb = cb;
  cam->callback.user_data = user_data;

  cam->camera->requestCompleted.connect(cam, [cam](Request* request) {
    if (cam->callback.cb) {
      /* Request をラップして渡す (所有権は渡さない) */
      lc_request_t lc_req;
      lc_req.request = request;
      cam->callback.cb(cam->callback.user_data, &lc_req);
    }
  });
}

void lc_camera_disconnect_request_completed(lc_camera_t* cam) {
  cam->camera->requestCompleted.disconnect(cam);
  cam->callback.cb = nullptr;
  cam->callback.user_data = nullptr;
}
