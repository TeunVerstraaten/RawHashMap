#ifndef __HIST_FUNCTIONS__H_
#define __HIST_FUNCTIONS__H_

#include "histogram.h"

Histogram hist_find_uint32_t_raw(size_t n);
Histogram hist_find_uint32_t_google(size_t n);
Histogram hist_find_string_view_raw(size_t n);
Histogram hist_find_string_view_google(size_t n);

Histogram hist_remove_uint32_t_raw(size_t n);
Histogram hist_remove_uint32_t_google(size_t n);
Histogram hist_remove_string_view_raw(size_t n);
Histogram hist_remove_string_view_google(size_t n);

Histogram hist_insert_uint32_t_google(size_t n);
Histogram hist_insert_uint32_t_raw(size_t n);
Histogram hist_insert_string_view_google(size_t n);
Histogram hist_insert_string_view_raw(size_t n);

#endif // !__HIST_FUNCTIONS__H_
