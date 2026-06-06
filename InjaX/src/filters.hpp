#include "string-filters.hpp"
#include "html-filters.hpp"
#include "array-filters.hpp"
#include "number-filters.hpp"

inline void register_all_filters(inja::Environment &env) {
    custom_string::register_filters(env);
    custom_html::register_filters(env);
    custom_array::register_filters(env);
    custom_numeric::register_filters(env);
}