// chart-module.cpp
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

#include "chart-scale.hpp"
#include "chart-axis.hpp"
#include "chart-render.hpp"
#include "chart-shapes.hpp"
#include "chart-transform.hpp"
#include "chart-time.hpp"

using json = nlohmann::json;

void register_chart_scale_module(inja::Environment& env) {
  register_chart_scale_functions(env);
  register_chart_axis_functions(env);
  register_chart_render_functions(env);
  register_chart_shapes_functions(env);
  register_chart_transform_functions(env);
  register_chart_time_functions(env);
}

#if defined(_WIN32) || defined(_WIN64)
  #define EXPORT extern "C" __declspec(dllexport)
#else
  #define EXPORT extern "C"
#endif

EXPORT void register_module(inja::Environment& env) {
  register_chart_scale_module(env);
}