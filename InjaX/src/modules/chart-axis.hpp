// chart-axis.hpp
#ifndef CHART_AXIS_HPP
#define CHART_AXIS_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <inja/inja.hpp>
#include <cctype>

using json = nlohmann::json;

enum class TickFormat {
    DEFAULT,
    PERCENT,
    CURRENCY,
    SCIENTIFIC,
    DATE
};

// Optimización: Transformación directa al construir
inline std::string to_lower(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    std::transform(s.begin(), s.end(), std::back_inserter(out), [](unsigned char c){ 
        return std::tolower(c); 
    });
    return out;
}

inline TickFormat parse_tick_format(const std::string& fmt) {
    std::string f = to_lower(fmt);
    if (f == "percent" || f == "%" || f == "pct") return TickFormat::PERCENT;
    if (f == "currency" || f == "money" || f == "$") return TickFormat::CURRENCY;
    if (f == "scientific" || f == "sci") return TickFormat::SCIENTIFIC;
    if (f == "date" || f == "time") return TickFormat::DATE;
    return TickFormat::DEFAULT;
}

inline std::string tick_format_to_string(TickFormat tf) {
    switch (tf) {
        case TickFormat::PERCENT:    return "percent";
        case TickFormat::CURRENCY:   return "currency";
        case TickFormat::SCIENTIFIC: return "scientific";
        case TickFormat::DATE:       return "date";
        default:                     return "default";
    }
}

inline json axisBottom(const json& scale) {
    json axis;
    axis["type"] = "axis";
    axis["orientation"] = "bottom";
    axis["scale"] = scale;
    axis["ticks"] = json::object();
    axis["tickFormat"] = json::object();
    axis["tickValues"] = json::array();
    axis["tickSize"] = 6.0;
    axis["tickSizeInner"] = 6.0;
    axis["tickSizeOuter"] = 6.0;
    axis["tickPadding"] = 3.0;
    axis["tickArguments"] = json::array();
    axis["grid"] = false;
    axis["tickAngle"] = 0.0;
    return axis;
}

inline json axisTop(const json& scale) {
    json axis = axisBottom(scale);
    axis["orientation"] = "top";
    return axis;
}

inline json axisLeft(const json& scale) {
    json axis = axisBottom(scale);
    axis["orientation"] = "left";
    return axis;
}

inline json axisRight(const json& scale) {
    json axis = axisBottom(scale);
    axis["orientation"] = "right";
    return axis;
}

inline json tickCount(json axis, int count) {
    axis["ticks"]["count"] = count;
    axis["tickArguments"] = json::array({count});
    return axis;
}

inline json tickValues(json axis, const json& values) {
    if (values.is_array()) {
        axis["tickValues"] = values;
    }
    return axis;
}

inline json tickFormat(json axis, const std::string& format) {
    TickFormat tf = parse_tick_format(format);
    axis["tickFormat"]["type"] = "enum";
    axis["tickFormat"]["value"] = tick_format_to_string(tf);
    return axis;
}

inline json tickFormat(json axis, TickFormat format) {
    axis["tickFormat"]["type"] = "enum";
    axis["tickFormat"]["value"] = tick_format_to_string(format);
    return axis;
}

inline json tickSize(json axis, double size) {
    axis["tickSize"] = size;
    axis["tickSizeInner"] = size;
    axis["tickSizeOuter"] = size;
    return axis;
}

inline json tickSizeInner(json axis, double size) {
    axis["tickSizeInner"] = size;
    return axis;
}

inline json tickSizeOuter(json axis, double size) {
    axis["tickSizeOuter"] = size;
    return axis;
}

inline json tickPadding(json axis, double padding) {
    axis["tickPadding"] = padding;
    return axis;
}

inline json tickAngle(json axis, double angle) {
    axis["tickAngle"] = angle;
    return axis;
}

inline json grid(json axis, bool enable) {
    axis["grid"] = enable;
    return axis;
}

inline void register_chart_axis_functions(inja::Environment& env) {
    env.add_callback("axisBottom", 1, [](inja::Arguments& args) -> json { return axisBottom(*args[0]); });
    env.add_callback("axisTop", 1, [](inja::Arguments& args) -> json { return axisTop(*args[0]); });
    env.add_callback("axisLeft", 1, [](inja::Arguments& args) -> json { return axisLeft(*args[0]); });
    env.add_callback("axisRight", 1, [](inja::Arguments& args) -> json { return axisRight(*args[0]); });
    
env.add_callback("tickCount", 2, [](inja::Arguments& args) -> json {
    int count = args[1]->is_number() ? args[1]->get<int>() : 0;
    return tickCount(*args[0], count); // <-- correcto
});
    
    env.add_callback("tickValues", 2, [](inja::Arguments& args) -> json {
        return tickValues(*args[0], *args[1]);
    });
    
    env.add_callback("tickFormat", 2, [](inja::Arguments& args) -> json {
        std::string fmt = args[1]->is_string() ? args[1]->get<std::string>() : "";
        return tickFormat(*args[0], fmt);
    });
    
    env.add_callback("tickSize", 2, [](inja::Arguments& args) -> json {
        double size = args[1]->is_number() ? args[1]->get<double>() : 6.0;
        return tickSize(*args[0], size);
    });
    
    env.add_callback("tickPadding", 2, [](inja::Arguments& args) -> json {
        double pad = args[1]->is_number() ? args[1]->get<double>() : 3.0;
        return tickPadding(*args[0], pad);
    });
    
    env.add_callback("grid", 2, [](inja::Arguments& args) -> json {
        bool enable = args[1]->is_boolean() ? args[1]->get<bool>() : false;
        return grid(*args[0], enable);
    });

    env.add_callback("tickAngle", 2, [](inja::Arguments& args) -> json {
      double angle = args[1]->is_number() ? args[1]->get<double>() : 0.0;
      return tickAngle(*args[0], angle);
    });
}

#endif