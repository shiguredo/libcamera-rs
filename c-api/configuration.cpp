#include "configuration.h"
#include "internal.h"

/* === CameraConfiguration === */

void lc_camera_configuration_destroy(lc_camera_configuration_t* config) {
  delete config->config;
  delete config;
}

size_t lc_camera_configuration_size(const lc_camera_configuration_t* config) {
  return config->config->size();
}

lc_stream_configuration_t* lc_camera_configuration_at(
    lc_camera_configuration_t* config,
    unsigned int index) {
  if (index >= config->config->size()) {
    return nullptr;
  }
  auto sc = new lc_stream_configuration();
  sc->sc = &config->config->at(index);
  return sc;
}

lc_config_status_t lc_camera_configuration_validate(
    lc_camera_configuration_t* config) {
  auto status = config->config->validate();
  switch (status) {
    case CameraConfiguration::Valid:
      return LC_CONFIG_STATUS_VALID;
    case CameraConfiguration::Adjusted:
      return LC_CONFIG_STATUS_ADJUSTED;
    case CameraConfiguration::Invalid:
      return LC_CONFIG_STATUS_INVALID;
    default:
      return LC_CONFIG_STATUS_INVALID;
  }
}

/* === StreamConfiguration === */

void lc_stream_configuration_destroy(lc_stream_configuration_t* sc) {
  delete sc;
}

lc_pixel_format_t lc_stream_configuration_get_pixel_format(
    const lc_stream_configuration_t* sc) {
  lc_pixel_format_t fmt;
  fmt.fourcc = sc->sc->pixelFormat.fourcc();
  fmt.modifier = sc->sc->pixelFormat.modifier();
  return fmt;
}

void lc_stream_configuration_set_pixel_format(lc_stream_configuration_t* sc,
                                              lc_pixel_format_t fmt) {
  sc->sc->pixelFormat = PixelFormat(fmt.fourcc, fmt.modifier);
}

lc_size_t lc_stream_configuration_get_size(
    const lc_stream_configuration_t* sc) {
  lc_size_t size;
  size.width = sc->sc->size.width;
  size.height = sc->sc->size.height;
  return size;
}

void lc_stream_configuration_set_size(lc_stream_configuration_t* sc,
                                      lc_size_t size) {
  sc->sc->size.width = size.width;
  sc->sc->size.height = size.height;
}

unsigned int lc_stream_configuration_get_stride(
    const lc_stream_configuration_t* sc) {
  return sc->sc->stride;
}

unsigned int lc_stream_configuration_get_frame_size(
    const lc_stream_configuration_t* sc) {
  return sc->sc->frameSize;
}

unsigned int lc_stream_configuration_get_buffer_count(
    const lc_stream_configuration_t* sc) {
  return sc->sc->bufferCount;
}

void lc_stream_configuration_set_buffer_count(lc_stream_configuration_t* sc,
                                              unsigned int count) {
  sc->sc->bufferCount = count;
}
