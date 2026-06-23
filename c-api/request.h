#ifndef LC_REQUEST_H
#define LC_REQUEST_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void lc_request_destroy(lc_request_t* request);
void lc_request_reuse(lc_request_t* request);
int lc_request_add_buffer(lc_request_t* request,
                          lc_stream_t* stream,
                          lc_frame_buffer_t* buffer);
lc_request_status_t lc_request_status(const lc_request_t* request);
uint64_t lc_request_cookie(const lc_request_t* request);
uint32_t lc_request_sequence(const lc_request_t* request);
lc_control_list_t* lc_request_controls(lc_request_t* request);
lc_control_list_t* lc_request_metadata(const lc_request_t* request);
lc_frame_buffer_t* lc_request_find_buffer(const lc_request_t* request,
                                          lc_stream_t* stream);

#ifdef __cplusplus
}
#endif

#endif /* LC_REQUEST_H */
