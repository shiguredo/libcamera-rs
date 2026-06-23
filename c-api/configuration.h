#ifndef LC_CONFIGURATION_H
#define LC_CONFIGURATION_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void lc_camera_configuration_destroy(lc_camera_configuration_t* config);
size_t lc_camera_configuration_size(const lc_camera_configuration_t* config);
lc_stream_configuration_t* lc_camera_configuration_at(
    lc_camera_configuration_t* config,
    unsigned int index);
lc_config_status_t lc_camera_configuration_validate(
    lc_camera_configuration_t* config);

void lc_stream_configuration_destroy(lc_stream_configuration_t* sc);
lc_pixel_format_t lc_stream_configuration_get_pixel_format(
    const lc_stream_configuration_t* sc);
void lc_stream_configuration_set_pixel_format(lc_stream_configuration_t* sc,
                                              lc_pixel_format_t fmt);
lc_size_t lc_stream_configuration_get_size(const lc_stream_configuration_t* sc);
void lc_stream_configuration_set_size(lc_stream_configuration_t* sc,
                                      lc_size_t size);
unsigned int lc_stream_configuration_get_stride(
    const lc_stream_configuration_t* sc);
unsigned int lc_stream_configuration_get_frame_size(
    const lc_stream_configuration_t* sc);
unsigned int lc_stream_configuration_get_buffer_count(
    const lc_stream_configuration_t* sc);
void lc_stream_configuration_set_buffer_count(lc_stream_configuration_t* sc,
                                              unsigned int count);

#ifdef __cplusplus
}
#endif

#endif /* LC_CONFIGURATION_H */
