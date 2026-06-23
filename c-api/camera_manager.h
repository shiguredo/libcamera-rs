#ifndef LC_CAMERA_MANAGER_H
#define LC_CAMERA_MANAGER_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

lc_camera_manager_t* lc_camera_manager_create(void);
void lc_camera_manager_destroy(lc_camera_manager_t* mgr);
int lc_camera_manager_start(lc_camera_manager_t* mgr);
void lc_camera_manager_stop(lc_camera_manager_t* mgr);
size_t lc_camera_manager_cameras_count(const lc_camera_manager_t* mgr);
lc_camera_t* lc_camera_manager_get_camera(lc_camera_manager_t* mgr,
                                          size_t index);
lc_camera_t* lc_camera_manager_get_camera_by_id(lc_camera_manager_t* mgr,
                                                const char* id);

#ifdef __cplusplus
}
#endif

#endif /* LC_CAMERA_MANAGER_H */
