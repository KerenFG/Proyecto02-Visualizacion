// chart-scale.hpp
#ifndef CHART_SCALE_HPP
#define CHART_SCALE_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>
#include <inja/inja.hpp>

using json = nlohmann::json;

inline double domain_to_double(const json& value) {
    if (value.is_number()) return value.get<double>();
    if (value.is_string()) {
        try {
            return std::stod(value.get_ref<const std::string&>());
        } catch (...) {
            return 0.0;
        }
    }
    return 0.0;
}

inline std::string domain_to_string(const json& value) {
    if (value.is_string()) return value.get_ref<const std::string&>();
    if (value.is_number()) return std::to_string(value.get<double>());
    return "";
}

inline json scaleLinear() {
    return json{
        {"type", "linear"}, {"domain", json::array()}, {"range", json::array()},
        {"clamped", false}, {"rounded", false}, {"niceCount", 0}
    };
}

inline json scaleBand() {
    return json{
        {"type", "band"}, {"domain", json::array()}, {"range", json::array()},
        {"paddingValue", 0.0}, {"paddingInnerValue", 0.0}, {"paddingOuterValue", 0.0},
        {"rounded", false}
    };
}

inline json scaleOrdinal() {
    return json{
        {"type", "ordinal"}, {"domain", json::array()}, {"range", json::array()},
        {"paddingValue", 0.0}
    };
}

inline json scaleLog() {
    return json{
        {"type", "log"}, {"domain", json::array()}, {"range", json::array()},
        {"clamped", false}, {"rounded", false}, {"niceCount", 0}
    };
}

inline json scalePower() {
    return json{
        {"type", "power"}, {"domain", json::array()}, {"range", json::array()},
        {"exponent", 1.0}, {"clamped", false}, {"rounded", false}, {"niceCount", 0}
    };
}

inline json scaleTime() {
    return json{
        {"type", "time"}, {"domain", json::array()}, {"range", json::array()},
        {"clamped", false}, {"rounded", false}, {"niceCount", 0}
    };
}

inline json domain(json scale, const json& domainArray) {
    if (domainArray.is_array()) scale["domain"] = domainArray;
    return scale;
}

inline json domain(json scale, const json& a, const json& b) {
    scale["domain"] = json::array({a, b});
    return scale;
}

inline json range(json scale, const json& rangeArray) {
    if (rangeArray.is_array()) scale["range"] = rangeArray;
    return scale;
}

inline json range(json scale, const json& a, const json& b) {
    scale["range"] = json::array({a, b});
    return scale;
}

inline json extent(const json& values, const std::string& field = "") {
    if (!values.is_array() || values.empty()) return json::array();

    double minVal = std::numeric_limits<double>::infinity();
    double maxVal = -std::numeric_limits<double>::infinity();
    bool hasValidData = false;

    for (const auto& v : values) {
        double val = 0.0;
        bool isValid = false;
        
        if (field.empty()) {
            if (v.is_number()) {
                val = v.get<double>();
                isValid = true;
            } else if (v.is_string()) {
                try {
                    val = std::stod(v.get_ref<const std::string&>());
                    isValid = true;
                } catch (...) {}
            }
        } else {
            if (v.is_object() && v.contains(field)) {
                const auto& fieldValue = v[field];
                if (fieldValue.is_number()) {
                    val = fieldValue.get<double>();
                    isValid = true;
                } else if (fieldValue.is_string()) {
                    try {
                        val = std::stod(fieldValue.get_ref<const std::string&>());
                        isValid = true;
                    } catch (...) {}
                }
            }
        }
        
        if (isValid) {
            if (val < minVal) minVal = val;
            if (val > maxVal) maxVal = val;
            hasValidData = true;
        }
    }
    
    if (!hasValidData) return json::array();
    return json::array({minVal, maxVal});
}

inline json padding(json scale, double value) {
    scale["paddingValue"] = value;
    return scale;
}

inline json paddingInner(json scale, double value) {
    scale["paddingInnerValue"] = value;
    return scale;
}

inline json paddingOuter(json scale, double value) {
    scale["paddingOuterValue"] = value;
    return scale;
}

inline json clamp(json scale, bool value) {
    scale["clamped"] = value;
    return scale;
}

inline json nice(json scale, int count) {
    scale["niceCount"] = count;
    return scale;
}

inline json round(json scale, bool value) {
    scale["rounded"] = value;
    return scale;
}

inline json exponent(json scale, double value) {
    if (scale.value("type", "") == "power") {
        scale["exponent"] = value;
    }
    return scale;
}

inline bool is_valid_scale_bounds(const json& scaleObj) {
    return scaleObj.contains("domain") && scaleObj["domain"].is_array() && scaleObj["domain"].size() >= 2 &&
           scaleObj.contains("range") && scaleObj["range"].is_array() && scaleObj["range"].size() >= 2;
}

inline json eval_linear(const json& scaleObj, const json& value) {
    if (!is_valid_scale_bounds(scaleObj)) return 0.0;

    double val = domain_to_double(value);
    const auto& domainArray = scaleObj["domain"];
    const auto& rangeArray = scaleObj["range"];
    
    double domainMin = domain_to_double(domainArray[0]);
    double domainMax = domain_to_double(domainArray[1]);
    double rangeMin = domain_to_double(rangeArray[0]);
    double rangeMax = domain_to_double(rangeArray[1]);
    
    double denom = domainMax - domainMin;
    if (std::abs(denom) < 1e-9) return rangeMin;
    
    double t = (val - domainMin) / denom;
    if (scaleObj.value("clamped", false)) t = std::clamp(t, 0.0, 1.0);
    
    double result = rangeMin + t * (rangeMax - rangeMin);
    if (scaleObj.value("rounded", false)) result = std::round(result);
    
    return result;
}

inline json eval_band(const json& scaleObj, const json& value) {
    if (!scaleObj.contains("domain") || !scaleObj["domain"].is_array() || scaleObj["domain"].empty() ||
        !scaleObj.contains("range") || !scaleObj["range"].is_array() || scaleObj["range"].size() < 2) {
        return 0.0;
    }

    const auto& domainArray = scaleObj["domain"];
    const auto& rangeArray = scaleObj["range"];

    int index = -1;
    for (size_t i = 0; i < domainArray.size(); ++i) {
        if (domainArray[i] == value) {
            index = static_cast<int>(i);
            break;
        }
    }
    if (index == -1) return 0.0;

    double r0 = rangeArray[0].is_number() ? rangeArray[0].get<double>() : 0.0;
    double r1 = rangeArray[1].is_number() ? rangeArray[1].get<double>() : 0.0;

    double step = (r1 - r0) / static_cast<double>(domainArray.size());
    double paddingVal = scaleObj.value("paddingValue", 0.0);
    double bandWidth = step * (1.0 - paddingVal);
    double offset = (step - bandWidth) / 2.0;

    double result = r0 + (index * step) + offset;
    if (scaleObj.value("rounded", false)) result = std::round(result);
    
    return result;
}

inline json eval_ordinal(const json& scaleObj, const json& value) {
    if (!scaleObj.contains("domain") || !scaleObj["domain"].is_array() || scaleObj["domain"].empty() ||
        !scaleObj.contains("range") || !scaleObj["range"].is_array() || scaleObj["range"].empty()) {
        return json(nullptr);
    }

    std::string val = domain_to_string(value);
    const auto& domainArray = scaleObj["domain"];
    const auto& rangeArray = scaleObj["range"];
    
    int index = -1;
    for (size_t i = 0; i < domainArray.size(); ++i) {
        if (domain_to_string(domainArray[i]) == val) {
            index = static_cast<int>(i);
            break;
        }
    }
    if (index == -1) return json(nullptr);
    
    size_t r_size = rangeArray.size();
    size_t target_idx = (index < static_cast<int>(r_size)) ? static_cast<size_t>(index) : (r_size - 1);
    return rangeArray[target_idx];
}

inline json eval_log(const json& scaleObj, const json& value) {
    if (!is_valid_scale_bounds(scaleObj)) return 0.0;

    double val = domain_to_double(value);
    const auto& domainArray = scaleObj["domain"];
    const auto& rangeArray = scaleObj["range"];
    
    double domainMin = domain_to_double(domainArray[0]);
    double domainMax = domain_to_double(domainArray[1]);
    double rangeMin = domain_to_double(rangeArray[0]);
    double rangeMax = domain_to_double(rangeArray[1]);
    
    if (val <= 0.0 || domainMin <= 0.0 || domainMax <= 0.0 || std::abs(domainMax - domainMin) < 1e-9) {
        return rangeMin;
    }
    
    double t = std::log10(val / domainMin) / std::log10(domainMax / domainMin);
    if (scaleObj.value("clamped", false)) t = std::clamp(t, 0.0, 1.0);
    
    double result = rangeMin + t * (rangeMax - rangeMin);
    if (scaleObj.value("rounded", false)) result = std::round(result);
    
    return result;
}

inline json eval_power(const json& scaleObj, const json& value) {
    if (!is_valid_scale_bounds(scaleObj)) return 0.0;

    double val = domain_to_double(value);
    const auto& domainArray = scaleObj["domain"];
    const auto& rangeArray = scaleObj["range"];
    
    double domainMin = domain_to_double(domainArray[0]);
    double domainMax = domain_to_double(domainArray[1]);
    double rangeMin = domain_to_double(rangeArray[0]);
    double rangeMax = domain_to_double(rangeArray[1]);
    double exp = scaleObj.value("exponent", 1.0);
    
    double denom = domainMax - domainMin;
    if (std::abs(denom) < 1e-9) return rangeMin;
    
    double base = (val - domainMin) / denom;
    if (base < 0.0 && std::floor(exp) != exp) base = 0.0;

    double t = std::pow(base, exp);
    if (scaleObj.value("clamped", false)) t = std::clamp(t, 0.0, 1.0);
    
    double result = rangeMin + t * (rangeMax - rangeMin);
    if (scaleObj.value("rounded", false)) result = std::round(result);
    
    return result;
}

inline json scale(const json& scaleObj, const json& value) {
    std::string type = scaleObj.value("type", "");
    if (type == "linear" || type == "time") return eval_linear(scaleObj, value);
    if (type == "band") return eval_band(scaleObj, value);
    if (type == "ordinal") return eval_ordinal(scaleObj, value);
    if (type == "log") return eval_log(scaleObj, value);
    if (type == "power") return eval_power(scaleObj, value);
    return json(nullptr);
}

inline json scale(const json& scaleObj, const std::string& value) { return scale(scaleObj, json(value)); }
inline json scale(const json& scaleObj, const char* value) { return scale(scaleObj, json(std::string(value))); }
inline json scale(const json& scaleObj, double value) { return scale(scaleObj, json(value)); }

inline double bandwidth(const json& scaleObj) {
    if (!scaleObj.is_object() || scaleObj.value("type", "") != "band") return 0.0;
    if (!scaleObj.contains("domain") || !scaleObj["domain"].is_array() || scaleObj["domain"].empty() ||
        !scaleObj.contains("range") || !scaleObj["range"].is_array() || scaleObj["range"].size() < 2) {
        return 0.0;
    }
    
    const auto& domainArray = scaleObj["domain"];
    const auto& rangeArray = scaleObj["range"];
    double r0 = rangeArray[0].is_number() ? rangeArray[0].get<double>() : 0.0;
    double r1 = rangeArray[1].is_number() ? rangeArray[1].get<double>() : 0.0;
    
    double step = (r1 - r0) / static_cast<double>(domainArray.size());
    double paddingVal = scaleObj.value("paddingValue", 0.0);
    double bandWidth = step * (1.0 - paddingVal);
    
    return scaleObj.value("rounded", false) ? std::round(bandWidth) : bandWidth;
}

inline double d3_tick_step(double start, double stop, int count) {
    if (count <= 0) return 0.0;
    double step = (stop - start) / count;
    double power = std::floor(std::log10(std::abs(step)));
    double error = std::abs(step) / std::pow(10.0, power);
    
    double factor = 1.0;
    if (error >= 8.5) factor = 10.0;
    else if (error >= 4.25) factor = 5.0;
    else if (error >= 1.7) factor = 2.0;
    
    double ideal_step = factor * std::pow(10.0, power);
    return step < 0 ? -ideal_step : ideal_step;
}

inline json ticks(const json& scaleObj, int count = 10) {
    json result = json::array();
    std::string type = scaleObj.value("type", "");

    if (type == "band" || type == "ordinal") {
        if (scaleObj.contains("domain") && scaleObj["domain"].is_array()) {
            return scaleObj["domain"];
        }
        return result;
    }

    if (!scaleObj.contains("domain") || !scaleObj["domain"].is_array() || scaleObj["domain"].size() < 2) {
        return result;
    }

    double start = domain_to_double(scaleObj["domain"][0]);
    double stop = domain_to_double(scaleObj["domain"][1]);

    if (start == stop || count <= 0) {
        result.push_back(start);
        return result;
    }

    bool inverse = start > stop;
    if (inverse) std::swap(start, stop);

    double step = d3_tick_step(start, stop, count);
    if (step == 0.0 || !std::isfinite(step)) return result;

    double tick_start = std::ceil(start / step) * step;
    double tick_stop = std::floor(stop / step) * step + step / 2.0;

    for (double v = tick_start; v <= tick_stop; v += step) {
        double rounded_v = v;
        double power = std::floor(std::log10(std::abs(step)));
        if (power < 0) {
            double precision = std::pow(10.0, -power + 2);
            rounded_v = std::round(v * precision) / precision;
        }
        result.push_back(rounded_v);
    }

    if (inverse) {
        std::reverse(result.begin(), result.end());
    }

    return result;
}

inline void register_chart_scale_functions(inja::Environment& env) {
    env.add_callback("scaleLinear", 0, [](inja::Arguments&) -> json { return scaleLinear(); });
    env.add_callback("scaleBand", 0, [](inja::Arguments&) -> json { return scaleBand(); });
    env.add_callback("scaleOrdinal", 0, [](inja::Arguments&) -> json { return scaleOrdinal(); });
    env.add_callback("scaleLog", 0, [](inja::Arguments&) -> json { return scaleLog(); });
    env.add_callback("scalePower", 0, [](inja::Arguments&) -> json { return scalePower(); });
    env.add_callback("scaleTime", 0, [](inja::Arguments&) -> json { return scaleTime(); });
    
    env.add_callback("domain", [](inja::Arguments& args) -> json {
        if (args.size() < 2 || args.size() > 3) throw std::runtime_error("domain expects 2 or 3 arguments");
        if (args.size() == 2) return domain(*args[0], *args[1]);
        return domain(*args[0], *args[1], *args[2]);
    });

    env.add_callback("range", [](inja::Arguments& args) -> json {
        if (args.size() < 2 || args.size() > 3) throw std::runtime_error("range expects 2 or 3 arguments");    
        if (args.size() == 2) return range(*args[0], *args[1]);
        return range(*args[0], *args[1], *args[2]);
    });
    
env.add_callback("extent", [](inja::Arguments& args) -> json {
    if (args.empty()) throw std::runtime_error("extent expects at least 1 argument");
    if (args.size() == 1) return extent(*args[0]);
    if (args.size() == 2 && args[1]->is_string()) return extent(*args[0], args[1]->get<std::string>());
    throw std::runtime_error("extent: invalid arguments");
});

    env.add_callback("padding", 2, [](inja::Arguments& args) -> json {
        double val = args[1]->is_number() ? args[1]->get<double>() : 0.0;
        return padding(*args[0], val);
    });
    
    env.add_callback("paddingInner", 2, [](inja::Arguments& args) -> json {
        double val = args[1]->is_number() ? args[1]->get<double>() : 0.0;
        return paddingInner(*args[0], val);
    });
    
    env.add_callback("paddingOuter", 2, [](inja::Arguments& args) -> json {
        double val = args[1]->is_number() ? args[1]->get<double>() : 0.0;
        return paddingOuter(*args[0], val);
    });
    
    env.add_callback("clamp", 2, [](inja::Arguments& args) -> json {
        bool val = args[1]->is_boolean() ? args[1]->get<bool>() : false;
        return clamp(*args[0], val);
    });
    
    env.add_callback("nice", 2, [](inja::Arguments& args) -> json {
        int count = args[1]->is_number() ? args[1]->get<int>() : 10;
        return nice(*args[0], count);
    });

    env.add_callback("round", 2, [](inja::Arguments& args) -> json {
        bool value = args[1]->is_boolean() ? args[1]->get<bool>() : false;
        return round(*args[0], value);
    });

    env.add_callback("exponent", 2, [](inja::Arguments& args) -> json {
        double value = args[1]->is_number() ? args[1]->get<double>() : 1.0;
        return exponent(*args[0], value);
    });

    env.add_callback("scale", 2, [](inja::Arguments& args) -> json { return scale(*args[0], *args[1]); });
    env.add_callback("bandwidth", 1, [](inja::Arguments& args) -> json { return bandwidth(*args[0]); });

env.add_callback("ticks", [](inja::Arguments& args) -> json {
    if (args.empty()) throw std::runtime_error("ticks expects at least 1 argument (scale)");
    int count = 10;
    if (args.size() == 2 && args[1]->is_number()) {
        count = args[1]->get<int>();
    }
    return ticks(*args[0], count);
});
}

#endif