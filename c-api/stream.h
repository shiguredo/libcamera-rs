#ifndef LC_STREAM_H
#define LC_STREAM_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

lc_stream_t* lc_stream_configuration_stream(
    const lc_stream_configuration_t* sc);
void lc_stream_ref_destroy(lc_stream_t* stream);

#ifdef __cplusplus
}
#endif

#endif /* LC_STREAM_H */
