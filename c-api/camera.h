#ifndef LC_CAMERA_H
#define LC_CAMERA_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* lc_camera_id(const lc_camera_t* cam);
int lc_camera_acquire(lc_camera_t* cam);
int lc_camera_release(lc_camera_t* cam);
void lc_camera_release_ref(lc_camera_t* cam);
lc_camera_configuration_t* lc_camera_generate_configuration(
    lc_camera_t* cam,
    const lc_stream_role_t* roles,
    size_t roles_count);
int lc_camera_configure(lc_camera_t* cam, lc_camera_configuration_t* config);
lc_request_t* lc_camera_create_request(lc_camera_t* cam, uint64_t cookie);
int lc_camera_queue_request(lc_camera_t* cam, lc_request_t* request);
int lc_camera_start(lc_camera_t* cam);
int lc_camera_stop(lc_camera_t* cam);
void lc_camera_connect_request_completed(lc_camera_t* cam,
                                         lc_request_completed_cb cb,
                                         void* user_data);
void lc_camera_disconnect_request_completed(lc_camera_t* cam);

#ifdef __cplusplus
}
#endif

#endif /* LC_CAMERA_H */
