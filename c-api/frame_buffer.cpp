#include "frame_buffer.h"
#include "internal.h"

/* === FrameBufferAllocator === */

lc_frame_buffer_allocator_t* lc_frame_buffer_allocator_create(
    lc_camera_t* cam) {
  auto alloc = new lc_frame_buffer_allocator();
  alloc->alloc = std::make_unique<FrameBufferAllocator>(cam->camera);
  return alloc;
}

void lc_frame_buffer_allocator_destroy(lc_frame_buffer_allocator_t* alloc) {
  delete alloc;
}

int lc_frame_buffer_allocator_allocate(lc_frame_buffer_allocator_t* alloc,
                                       lc_stream_t* stream) {
  return alloc->alloc->allocate(stream->stream);
}

int lc_frame_buffer_allocator_free(lc_frame_buffer_allocator_t* alloc,
                                   lc_stream_t* stream) {
  return alloc->alloc->free(stream->stream);
}

size_t lc_frame_buffer_allocator_buffers_count(
    const lc_frame_buffer_allocator_t* alloc,
    lc_stream_t* stream) {
  const auto& buffers = alloc->alloc->buffers(stream->stream);
  return buffers.size();
}

lc_frame_buffer_t* lc_frame_buffer_allocator_get_buffer(
    const lc_frame_buffer_allocator_t* alloc,
    lc_stream_t* stream,
    size_t index) {
  const auto& buffers = alloc->alloc->buffers(stream->stream);
  if (index >= buffers.size()) {
    return nullptr;
  }
  auto fb = new lc_frame_buffer();
  fb->fb = buffers[index].get();
  return fb;
}

/* === FrameBuffer === */

void lc_frame_buffer_ref_destroy(lc_frame_buffer_t* fb) {
  delete fb;
}

size_t lc_frame_buffer_planes_count(const lc_frame_buffer_t* fb) {
  return fb->fb->planes().size();
}

lc_frame_buffer_plane_t lc_frame_buffer_get_plane(const lc_frame_buffer_t* fb,
                                                  size_t index) {
  lc_frame_buffer_plane_t plane;
  const auto& planes = fb->fb->planes();
  if (index >= planes.size()) {
    plane.fd = -1;
    plane.offset = 0;
    plane.length = 0;
    return plane;
  }
  plane.fd = planes[index].fd.get();
  plane.offset = planes[index].offset;
  plane.length = planes[index].length;
  return plane;
}

lc_frame_metadata_t lc_frame_buffer_metadata(const lc_frame_buffer_t* fb) {
  lc_frame_metadata_t meta;
  const auto& md = fb->fb->metadata();
  meta.status = static_cast<int>(md.status);
  meta.sequence = md.sequence;
  meta.timestamp = md.timestamp;
  return meta;
}
