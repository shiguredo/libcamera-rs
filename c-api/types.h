#ifndef LC_TYPES_H
#define LC_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 不透明ポインタ型 */
typedef struct lc_camera_manager lc_camera_manager_t;
typedef struct lc_camera lc_camera_t;
typedef struct lc_camera_configuration lc_camera_configuration_t;
typedef struct lc_stream_configuration lc_stream_configuration_t;
typedef struct lc_request lc_request_t;
typedef struct lc_frame_buffer lc_frame_buffer_t;
typedef struct lc_frame_buffer_allocator lc_frame_buffer_allocator_t;
typedef struct lc_stream lc_stream_t;
typedef struct lc_control_list lc_control_list_t;

/* POD 型 */
typedef struct {
  int x;
  int y;
  unsigned int width;
  unsigned int height;
} lc_rectangle_t;

typedef struct {
  unsigned int width;
  unsigned int height;
} lc_size_t;

typedef struct {
  int x;
  int y;
} lc_point_t;

typedef struct {
  uint32_t fourcc;
  uint64_t modifier;
} lc_pixel_format_t;

typedef struct {
  int fd;
  unsigned int offset;
  unsigned int length;
} lc_frame_buffer_plane_t;

typedef struct {
  int status;
  unsigned int sequence;
  uint64_t timestamp;
} lc_frame_metadata_t;

/* 列挙型 */
typedef enum {
  LC_STREAM_ROLE_RAW = 0,
  LC_STREAM_ROLE_STILL_CAPTURE = 1,
  LC_STREAM_ROLE_VIDEO_RECORDING = 2,
  LC_STREAM_ROLE_VIEWFINDER = 3,
} lc_stream_role_t;

typedef enum {
  LC_CONFIG_STATUS_VALID = 0,
  LC_CONFIG_STATUS_ADJUSTED = 1,
  LC_CONFIG_STATUS_INVALID = 2,
} lc_config_status_t;

typedef enum {
  LC_REQUEST_STATUS_PENDING = 0,
  LC_REQUEST_STATUS_COMPLETE = 1,
  LC_REQUEST_STATUS_CANCELLED = 2,
} lc_request_status_t;

typedef enum {
  LC_FRAME_STATUS_SUCCESS = 0,
  LC_FRAME_STATUS_ERROR = 1,
  LC_FRAME_STATUS_CANCELLED = 2,
  LC_FRAME_STATUS_STARTUP = 3,
} lc_frame_status_t;

typedef enum {
  LC_CONTROL_TYPE_NONE = 0,
  LC_CONTROL_TYPE_BOOL = 1,
  LC_CONTROL_TYPE_BYTE = 2,
  LC_CONTROL_TYPE_UNSIGNED16 = 3,
  LC_CONTROL_TYPE_UNSIGNED32 = 4,
  LC_CONTROL_TYPE_INT32 = 5,
  LC_CONTROL_TYPE_INT64 = 6,
  LC_CONTROL_TYPE_FLOAT = 7,
  LC_CONTROL_TYPE_STRING = 8,
  LC_CONTROL_TYPE_RECTANGLE = 9,
  LC_CONTROL_TYPE_SIZE = 10,
  LC_CONTROL_TYPE_POINT = 11,
} lc_control_type_t;

/* コールバック */
typedef void (*lc_request_completed_cb)(void* user_data, lc_request_t* request);

#ifdef __cplusplus
}
#endif

#endif /* LC_TYPES_H */
