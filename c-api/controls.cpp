#include "controls.h"
#include "internal.h"

void lc_control_list_ref_destroy(lc_control_list_t* list) {
  delete list;
}

size_t lc_control_list_size(const lc_control_list_t* list) {
  return list->list->size();
}

bool lc_control_list_contains(const lc_control_list_t* list, unsigned int id) {
  return list->list->contains(id);
}

bool lc_control_list_get_bool(const lc_control_list_t* list,
                              unsigned int id,
                              bool* out) {
  if (!list->list->contains(id)) {
    return false;
  }
  const auto& val = list->list->get(id);
  if (val.type() != ControlTypeBool) {
    return false;
  }
  *out = val.get<bool>();
  return true;
}

bool lc_control_list_get_int32(const lc_control_list_t* list,
                               unsigned int id,
                               int32_t* out) {
  if (!list->list->contains(id)) {
    return false;
  }
  const auto& val = list->list->get(id);
  if (val.type() != ControlTypeInteger32) {
    return false;
  }
  *out = val.get<int32_t>();
  return true;
}

bool lc_control_list_get_int64(const lc_control_list_t* list,
                               unsigned int id,
                               int64_t* out) {
  if (!list->list->contains(id)) {
    return false;
  }
  const auto& val = list->list->get(id);
  if (val.type() != ControlTypeInteger64) {
    return false;
  }
  *out = val.get<int64_t>();
  return true;
}

bool lc_control_list_get_float(const lc_control_list_t* list,
                               unsigned int id,
                               float* out) {
  if (!list->list->contains(id)) {
    return false;
  }
  const auto& val = list->list->get(id);
  if (val.type() != ControlTypeFloat) {
    return false;
  }
  *out = val.get<float>();
  return true;
}

bool lc_control_list_get_rectangle(const lc_control_list_t* list,
                                   unsigned int id,
                                   lc_rectangle_t* out) {
  if (!list->list->contains(id)) {
    return false;
  }
  const auto& val = list->list->get(id);
  if (val.type() != ControlTypeRectangle) {
    return false;
  }
  auto rect = val.get<Rectangle>();
  out->x = rect.x;
  out->y = rect.y;
  out->width = rect.width;
  out->height = rect.height;
  return true;
}

bool lc_control_list_get_float_array(const lc_control_list_t* list,
                                     unsigned int id,
                                     const float** out,
                                     size_t* count) {
  if (!list->list->contains(id)) {
    return false;
  }
  const auto& val = list->list->get(id);
  if (val.type() != ControlTypeFloat || !val.isArray()) {
    return false;
  }
  auto data = val.data();
  *out = reinterpret_cast<const float*>(data.data());
  *count = val.numElements();
  return true;
}

bool lc_control_list_get_int32_array(const lc_control_list_t* list,
                                     unsigned int id,
                                     const int32_t** out,
                                     size_t* count) {
  if (!list->list->contains(id)) {
    return false;
  }
  const auto& val = list->list->get(id);
  if (val.type() != ControlTypeInteger32 || !val.isArray()) {
    return false;
  }
  auto data = val.data();
  *out = reinterpret_cast<const int32_t*>(data.data());
  *count = val.numElements();
  return true;
}

void lc_control_list_set_bool(lc_control_list_t* list,
                              unsigned int id,
                              bool value) {
  ControlValue cv;
  cv.set<bool>(value);
  list->list->set(id, cv);
}

void lc_control_list_set_int32(lc_control_list_t* list,
                               unsigned int id,
                               int32_t value) {
  ControlValue cv;
  cv.set<int32_t>(value);
  list->list->set(id, cv);
}

void lc_control_list_set_int64(lc_control_list_t* list,
                               unsigned int id,
                               int64_t value) {
  ControlValue cv;
  cv.set<int64_t>(value);
  list->list->set(id, cv);
}

void lc_control_list_set_float(lc_control_list_t* list,
                               unsigned int id,
                               float value) {
  ControlValue cv;
  cv.set<float>(value);
  list->list->set(id, cv);
}

void lc_control_list_set_rectangle(lc_control_list_t* list,
                                   unsigned int id,
                                   lc_rectangle_t value) {
  Rectangle rect(value.x, value.y, value.width, value.height);
  ControlValue cv;
  cv.set<Rectangle>(rect);
  list->list->set(id, cv);
}

void lc_control_list_set_float_array(lc_control_list_t* list,
                                     unsigned int id,
                                     const float* values,
                                     size_t count) {
  ControlValue cv(Span<const float>(values, count));
  list->list->set(id, cv);
}

void lc_control_list_set_int32_array(lc_control_list_t* list,
                                     unsigned int id,
                                     const int32_t* values,
                                     size_t count) {
  ControlValue cv(Span<const int32_t>(values, count));
  list->list->set(id, cv);
}
