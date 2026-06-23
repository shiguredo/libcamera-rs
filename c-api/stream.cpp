#include "stream.h"
#include "internal.h"

lc_stream_t* lc_stream_configuration_stream(
    const lc_stream_configuration_t* sc) {
  auto stream = sc->sc->stream();
  if (!stream) {
    return nullptr;
  }
  auto lc_st = new lc_stream();
  lc_st->stream = stream;
  return lc_st;
}

void lc_stream_ref_destroy(lc_stream_t* stream) {
  delete stream;
}
