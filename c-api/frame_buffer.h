#ifndef LC_FRAME_BUFFER_H
#define LC_FRAME_BUFFER_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

lc_frame_buffer_allocator_t* lc_frame_buffer_allocator_create(lc_camera_t* cam);
void lc_frame_buffer_allocator_destroy(lc_frame_buffer_allocator_t* alloc);
int lc_frame_buffer_allocator_allocate(lc_frame_buffer_allocator_t* alloc,
                                       lc_stream_t* stream);
int lc_frame_buffer_allocator_free(lc_frame_buffer_allocator_t* alloc,
                                   lc_stream_t* stream);
size_t lc_frame_buffer_allocator_buffers_count(
    const lc_frame_buffer_allocator_t* alloc,
    lc_stream_t* stream);
lc_frame_buffer_t* lc_frame_buffer_allocator_get_buffer(
    const lc_frame_buffer_allocator_t* alloc,
    lc_stream_t* stream,
    size_t index);

void lc_frame_buffer_ref_destroy(lc_frame_buffer_t* fb);
size_t lc_frame_buffer_planes_count(const lc_frame_buffer_t* fb);
lc_frame_buffer_plane_t lc_frame_buffer_get_plane(const lc_frame_buffer_t* fb,
                                                  size_t index);
lc_frame_metadata_t lc_frame_buffer_metadata(const lc_frame_buffer_t* fb);

#ifdef __cplusplus
}
#endif

#endif /* LC_FRAME_BUFFER_H */
