#include "request.h"
#include "internal.h"

void lc_request_destroy(lc_request_t* request) {
  delete request->request;
  delete request;
}

void lc_request_reuse(lc_request_t* request) {
  request->request->reuse(Request::ReuseBuffers);
}

int lc_request_add_buffer(lc_request_t* request,
                          lc_stream_t* stream,
                          lc_frame_buffer_t* buffer) {
  return request->request->addBuffer(stream->stream, buffer->fb);
}

lc_request_status_t lc_request_status(const lc_request_t* request) {
  switch (request->request->status()) {
    case Request::RequestPending:
      return LC_REQUEST_STATUS_PENDING;
    case Request::RequestComplete:
      return LC_REQUEST_STATUS_COMPLETE;
    case Request::RequestCancelled:
      return LC_REQUEST_STATUS_CANCELLED;
    default:
      return LC_REQUEST_STATUS_CANCELLED;
  }
}

uint64_t lc_request_cookie(const lc_request_t* request) {
  return request->request->cookie();
}

uint32_t lc_request_sequence(const lc_request_t* request) {
  return request->request->sequence();
}

lc_control_list_t* lc_request_controls(lc_request_t* request) {
  auto cl = new lc_control_list();
  cl->list = &request->request->controls();
  cl->owned = false;
  return cl;
}

lc_control_list_t* lc_request_metadata(const lc_request_t* request) {
  auto cl = new lc_control_list();
  cl->list = const_cast<ControlList*>(&request->request->metadata());
  cl->owned = false;
  return cl;
}

lc_frame_buffer_t* lc_request_find_buffer(const lc_request_t* request,
                                          lc_stream_t* stream) {
  auto* fb = request->request->findBuffer(stream->stream);
  if (!fb) {
    return nullptr;
  }
  auto lc_fb = new lc_frame_buffer();
  lc_fb->fb = fb;
  return lc_fb;
}
