#ifndef LC_CONTROLS_H
#define LC_CONTROLS_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void lc_control_list_ref_destroy(lc_control_list_t* list);
size_t lc_control_list_size(const lc_control_list_t* list);
bool lc_control_list_contains(const lc_control_list_t* list, unsigned int id);

bool lc_control_list_get_bool(const lc_control_list_t* list,
                              unsigned int id,
                              bool* out);
bool lc_control_list_get_int32(const lc_control_list_t* list,
                               unsigned int id,
                               int32_t* out);
bool lc_control_list_get_int64(const lc_control_list_t* list,
                               unsigned int id,
                               int64_t* out);
bool lc_control_list_get_float(const lc_control_list_t* list,
                               unsigned int id,
                               float* out);
bool lc_control_list_get_rectangle(const lc_control_list_t* list,
                                   unsigned int id,
                                   lc_rectangle_t* out);
bool lc_control_list_get_float_array(const lc_control_list_t* list,
                                     unsigned int id,
                                     const float** out,
                                     size_t* count);
bool lc_control_list_get_int32_array(const lc_control_list_t* list,
                                     unsigned int id,
                                     const int32_t** out,
                                     size_t* count);
bool lc_control_list_get_int64_array(const lc_control_list_t* list,
                                     unsigned int id,
                                     const int64_t** out,
                                     size_t* count);
bool lc_control_list_get_byte(const lc_control_list_t* list,
                              unsigned int id,
                              uint8_t* out);
bool lc_control_list_get_byte_array(const lc_control_list_t* list,
                                    unsigned int id,
                                    const uint8_t** out,
                                    size_t* count);
bool lc_control_list_get_string(const lc_control_list_t* list,
                               unsigned int id,
                               const char** out,
                               size_t* len);
bool lc_control_list_get_size(const lc_control_list_t* list,
                              unsigned int id,
                              lc_size_t* out);
bool lc_control_list_get_rectangle_array(const lc_control_list_t* list,
                                         unsigned int id,
                                         const lc_rectangle_t** out,
                                         size_t* count);
bool lc_control_list_get_point_array(const lc_control_list_t* list,
                                     unsigned int id,
                                     const lc_point_t** out,
                                     size_t* count);

void lc_control_list_set_bool(lc_control_list_t* list,
                              unsigned int id,
                              bool value);
void lc_control_list_set_int32(lc_control_list_t* list,
                               unsigned int id,
                               int32_t value);
void lc_control_list_set_int64(lc_control_list_t* list,
                               unsigned int id,
                               int64_t value);
void lc_control_list_set_float(lc_control_list_t* list,
                               unsigned int id,
                               float value);
void lc_control_list_set_rectangle(lc_control_list_t* list,
                                   unsigned int id,
                                   lc_rectangle_t value);
void lc_control_list_set_float_array(lc_control_list_t* list,
                                     unsigned int id,
                                     const float* values,
                                     size_t count);
void lc_control_list_set_int32_array(lc_control_list_t* list,
                                     unsigned int id,
                                     const int32_t* values,
                                     size_t count);
void lc_control_list_set_int64_array(lc_control_list_t* list,
                                     unsigned int id,
                                     const int64_t* values,
                                     size_t count);
void lc_control_list_set_byte(lc_control_list_t* list,
                              unsigned int id,
                              uint8_t value);
void lc_control_list_set_byte_array(lc_control_list_t* list,
                                    unsigned int id,
                                    const uint8_t* values,
                                    size_t count);
void lc_control_list_set_rectangle_array(lc_control_list_t* list,
                                         unsigned int id,
                                         const lc_rectangle_t* values,
                                         size_t count);
void lc_control_list_set_point_array(lc_control_list_t* list,
                                     unsigned int id,
                                     const lc_point_t* values,
                                     size_t count);

#ifdef __cplusplus
}
#endif

#endif /* LC_CONTROLS_H */
