// chart-render.hpp (refactorizado)
#ifndef CHART_RENDER_HPP
#define CHART_RENDER_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <inja/inja.hpp>
#include "chart-scale.hpp"

using json = nlohmann::json;

json scale(const json& scaleObj, const json& value);
std::string domain_to_string(const json& val);
json ticks(const json& scaleObj, int count);

inline std::string format_number(double value) {
    if (std::isnan(value) || std::isinf(value)) return "0";
    double intPart;
    if (std::modf(value, &intPart) == 0.0) return std::to_string(static_cast<long long>(intPart));
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    return oss.str();
}

static inline std::string escape_svg_string(const std::string& s) {
    std::string r; r.reserve(s.size() * 11 / 10);
    for (char c : s) {
        switch (c) {
            case '&': r += "&amp;"; break;
            case '<': r += "&lt;"; break;
            case '>': r += "&gt;"; break;
            case '"': r += "&quot;"; break;
            case '\'': r += "&apos;"; break;
            default: r += c; break;
        }
    }
    return r;
}

inline double read_double_tolerant(const json& j, double def = 0.0) {
    if (j.is_number()) return j.get<double>();
    if (j.is_string()) try { return std::stod(j.get<std::string>()); } catch(...) { return def; }
    return def;
}

inline bool is_valid_axis_structure(const json& axis) {
    if (!axis.contains("scale") || !axis["scale"].is_object()) return false;
    const auto& s = axis["scale"];
    if (!s.contains("range") || !s["range"].is_array() || s["range"].size() < 2) return false;
    if (!s.contains("domain") || !s["domain"].is_array() || s["domain"].size() < 2) return false;
    auto ok = [](const json& v){ return v.is_number() || v.is_string(); };
    return ok(s["range"][0]) && ok(s["range"][1]) && ok(s["domain"][0]) && ok(s["domain"][1]);
}

struct TickData { double coord; std::string label; };

inline std::string format_tick_label(double v, const std::string& fmt) {
    if (fmt == "percent") return format_number(v * 100.0) + "%";
    if (fmt == "currency") return "$" + format_number(v);
    if (fmt == "scientific") { std::ostringstream o; o << std::scientific << std::setprecision(1) << v; return o.str(); }
    return format_number(v);
}

inline std::vector<TickData> generate_ticks(const json& axis, const json& scaleObj) {
    std::vector<TickData> out;
    std::string fmt = (axis.contains("tickFormat") && axis["tickFormat"].contains("value")) ? axis["tickFormat"]["value"].get<std::string>() : "";
    json raw = json::array();

    if (axis.contains("tickValues") && axis["tickValues"].is_array() && !axis["tickValues"].empty()) raw = axis["tickValues"];
    else {
        int count = 10;
        if (axis.contains("ticks") && axis["ticks"].is_object()) count = axis["ticks"].value("count", 10);
        try { raw = ticks(scaleObj, count); } catch(...) { raw = json::array(); }
    }

    out.reserve(raw.size());
    for (const auto& v : raw) {
        if (!v.is_number() && !v.is_string() && !v.is_boolean()) continue;
        json cjson;
        try { cjson = scale(scaleObj, v); } catch(...) { continue; }
        double c = NAN;
        if (cjson.is_number()) c = cjson.get<double>();
        else c = read_double_tolerant(cjson, NAN);
        if (!std::isfinite(c)) continue;
        std::string label = v.is_number() ? format_tick_label(v.get<double>(), fmt) : domain_to_string(v);
        out.push_back({c, label});
    }
    return out;
}

inline void get_text_anchors(double angle, const std::string& orient, std::string& textAnchor, std::string& dominantBaseline) {
    textAnchor = "middle"; dominantBaseline = "middle";
    if (angle == 0.0) {
        if (orient == "bottom") { dominantBaseline = "hanging"; textAnchor = "middle"; }
        else if (orient == "top") { dominantBaseline = "auto"; textAnchor = "middle"; }
        else if (orient == "left") { textAnchor = "end"; dominantBaseline = "central"; }
        else if (orient == "right") { textAnchor = "start"; dominantBaseline = "central"; }
        return;
    }
    if (orient == "bottom") { textAnchor = (angle > 0 ? "start" : "end"); dominantBaseline = "central"; }
    else if (orient == "top") { textAnchor = (angle > 0 ? "end" : "start"); dominantBaseline = "central"; }
    else if (orient == "left") { textAnchor = "end"; dominantBaseline = (angle > 0 ? "hanging" : "auto"); }
    else if (orient == "right") { textAnchor = "start"; dominantBaseline = (angle > 0 ? "auto" : "hanging"); }
}

inline std::string render_axis_generic(const json& axis, const std::vector<TickData>& ticks, double rMin, double rMax, const std::string& orient) {
    std::string svg; svg.reserve(2048);
    double tickSize = axis.value("tickSize", 6.0);
    double tickPadding = axis.value("tickPadding", 3.0);
    bool hasGrid = axis.value("grid", false);
    double tickAngle = axis.value("tickAngle", 0.0);

    svg += "  <g class=\"axis axis-" + orient + "\" transform=\"translate(0, 0)\">\n";

    if (orient == "bottom" || orient == "top") {
        svg += "    <line class=\"axis-line\" x1=\"" + format_number(rMin) + "\" y1=\"0\" x2=\"" + format_number(rMax) + "\" y2=\"0\" stroke=\"currentColor\" stroke-width=\"1\"/>\n";
    } else {
        svg += "    <line class=\"axis-line\" x1=\"0\" y1=\"" + format_number(rMin) + "\" x2=\"0\" y2=\"" + format_number(rMax) + "\" stroke=\"currentColor\" stroke-width=\"1\"/>\n";
    }

    for (const auto& t : ticks) {
        if (orient == "bottom" || orient == "top") {
            double sign = (orient == "bottom") ? 1.0 : -1.0;
            svg += "    <g class=\"tick\" transform=\"translate(" + format_number(t.coord) + ", 0)\">\n";
            svg += "      <line class=\"tick-line\" x1=\"0\" y1=\"0\" x2=\"0\" y2=\"" + format_number(sign * tickSize) + "\" stroke=\"currentColor\" stroke-width=\"1\"/>\n";

            std::string ta, db; get_text_anchors(tickAngle, orient, ta, db);
            double labelY = sign * (tickSize + tickPadding);
            svg += "      <text class=\"tick-label\" x=\"0\" y=\"" + format_number(labelY) + "\" text-anchor=\"" + ta + "\" dominant-baseline=\"" + db + "\" ";
            if (tickAngle != 0.0) svg += "transform=\"rotate(" + format_number(tickAngle) + ", 0, " + format_number(labelY) + ")\" ";
            else svg += "dy=\"0.15em\" ";
            svg += ">" + escape_svg_string(t.label) + "</text>\n";

            if (hasGrid) svg += "      <line class=\"grid-line\" x1=\"0\" y1=\"0\" x2=\"0\" y2=\"" + format_number(-sign * 500.0) + "\" stroke=\"#e0e0e0\" stroke-width=\"1\" stroke-dasharray=\"4,4\"/>\n";
            svg += "    </g>\n";
        } else { // left or right
            double sign = (orient == "left") ? -1.0 : 1.0;
            svg += "    <g class=\"tick\" transform=\"translate(0, " + format_number(t.coord) + ")\">\n";
            svg += "      <line class=\"tick-line\" x1=\"" + format_number(sign * tickSize) + "\" y1=\"0\" x2=\"0\" y2=\"0\" stroke=\"currentColor\" stroke-width=\"1\"/>\n";

            std::string ta, db; get_text_anchors(tickAngle, orient, ta, db);
            double labelX = sign * (tickSize + tickPadding);
            svg += "      <text class=\"tick-label\" x=\"" + format_number(labelX) + "\" y=\"0\" text-anchor=\"" + ta + "\" dominant-baseline=\"" + db + "\" ";
            if (tickAngle != 0.0) svg += "transform=\"rotate(" + format_number(tickAngle) + ", " + format_number(labelX) + ", 0)\" ";
            svg += ">" + escape_svg_string(t.label) + "</text>\n";

            if (hasGrid) svg += "      <line class=\"grid-line\" x1=\"0\" y1=\"0\" x2=\"" + format_number(500.0) + "\" y2=\"0\" stroke=\"#e0e0e0\" stroke-width=\"1\" stroke-dasharray=\"4,4\"/>\n";
            svg += "    </g>\n";
        }
    }

    svg += "  </g>\n";
    return svg;
}

inline std::string call(const json& axis) {
    if (!is_valid_axis_structure(axis)) return "";
    std::string orient = axis.value("orientation", "");
    if (orient.empty()) return "";

    const auto& scaleObj = axis["scale"];
    const auto& range = scaleObj["range"];
    double rMin = read_double_tolerant(range[0], 0.0);
    double rMax = read_double_tolerant(range[1], 0.0);

    auto ticksData = generate_ticks(axis, scaleObj);

    if (ticksData.empty() && scaleObj.contains("domain") && scaleObj["domain"].is_array() && scaleObj["domain"].size() >= 2) {
        double d0 = read_double_tolerant(scaleObj["domain"][0], 0.0);
        double d1 = read_double_tolerant(scaleObj["domain"][1], 1.0);
        json raw = json::array({d0, (d0 + d1) / 2.0, d1});
        for (const auto& v : raw) {
            json cjson;
            try { cjson = scale(scaleObj, v); } catch(...) { continue; }
            double c = cjson.is_number() ? cjson.get<double>() : read_double_tolerant(cjson, NAN);
            if (!std::isfinite(c)) continue;
            std::string label = v.is_number() ? format_tick_label(v.get<double>(), "") : domain_to_string(v);
            ticksData.push_back({c, label});
        }
    }

    std::ostringstream dbg;
    dbg << "<!-- axis orientation=" << orient << " range=[" << format_number(rMin) << "," << format_number(rMax) << "] ticks=" << ticksData.size() << " -->\n";

    return dbg.str() + render_axis_generic(axis, ticksData, rMin, rMax, orient);
}

inline void register_chart_render_functions(inja::Environment& env) {
    env.add_callback("call", 1, [](inja::Arguments& args) -> std::string {
        return call(*args[0]);
    });
}

#endif
