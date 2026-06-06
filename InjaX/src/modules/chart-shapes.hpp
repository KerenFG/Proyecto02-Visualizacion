#ifndef CHART_SHAPES_HPP
#define CHART_SHAPES_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <inja/inja.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using json = nlohmann::json;

// --- Utilidades de Resolución ---

/**
 * Resuelve un valor dinámico:
 * - Si es número: lo devuelve.
 * - Si es string: busca esa clave en el objeto 'row'.
 */
inline double resolve_val(const json& config_val, const json& row, double def = 0.0) {
    if (config_val.is_number()) return config_val.get<double>();
    if (config_val.is_string() && row.is_object()) {
        std::string key = config_val.get<std::string>();
        if (row.contains(key) && row[key].is_number()) return row[key].get<double>();
    }
    return def;
}

inline std::string as_string(const json& j, const std::string& def = "") { 
    return j.is_string() ? j.get<std::string>() : def; 
}

inline json make_shape_defaults(const std::string& type) {
    if (type == "symbol") return json{{"type","symbol"},{"size",64.0},{"shape","circle"}};
    if (type == "line")   return json{{"type","line"},{"x","x"},{"y","y"},{"curve","linear"}};
    if (type == "area")   return json{{"type","area"},{"x","x"},{"y0",0.0},{"y1","y1"},{"curve","linear"}};
    if (type == "pie")    return json{{"type","pie"},{"value","value"},{"startAngle",0.0},{"endAngle",2.0*M_PI},{"sort",true}};
    if (type == "arc")    return json{{"type","arc"},{"innerRadius",0.0},{"outerRadius",100.0}};
    if (type == "radialLine") return json{{"type","radialLine"},{"angle","angle"},{"radius","radius"}};
    return json::object();
}

// --- Generador de Símbolos ---

inline std::string symbol_path(const json& gen) {
    if (!gen.is_object()) return "";
    // El tamaño y forma suelen ser fijos por llamada de símbolo
    double area = gen.value("size", 64.0);
    std::string type = as_string(gen.value("shape", "circle"));
    std::ostringstream path;

    if (type == "circle") {
        double r = std::sqrt(area / M_PI);
        path << "M " << -r << " 0 a " << r << " " << r << " 0 1 0 " << (2*r) << " 0 a " << r << " " << r << " 0 1 0 " << -(2*r) << " 0 Z";
    } else if (type == "square") {
        double side = std::sqrt(area); double h = side/2.0;
        path << "M " << -h << " " << -h << " L " << h << " " << -h << " L " << h << " " << h << " L " << -h << " " << h << " Z";
    } else if (type == "triangle") {
        double side = std::sqrt((4.0 * area) / std::sqrt(3.0));
        double h = side * std::sqrt(3.0) / 2.0;
        path << "M 0 " << -h/2.0 << " L " << side/2.0 << " " << h/2.0 << " L " << -side/2.0 << " " << h/2.0 << " Z";
    } else if (type == "cross") {
        double s = std::sqrt(area) / 2.0;
        path << "M " << -s << " " << -s/3.0 << " H " << -s/3.0 << " V " << -s << " H " << s/3.0
             << " V " << -s/3.0 << " H " << s << " V " << s/3.0 << " H " << s/3.0
             << " V " << s << " H " << -s/3.0 << " V " << s/3.0 << " H " << -s << " Z";
    }
    return path.str();
}

// --- Generador de Líneas (Soporta Curvas) ---

inline std::string build_line_path(const json& gen, const json& data) {
    if (!gen.is_object() || !data.is_array() || data.empty()) return "";
    
    json xConf = gen.value("x", "x");
    json yConf = gen.value("y", "y");
    std::string curve = as_string(gen.value("curve", "linear"));

    std::ostringstream path;
    bool first = true;
    double px=0, py=0, ppx=0, ppy=0;
    bool hasPP = false;

    for (size_t i=0; i < data.size(); ++i) {
        double x = resolve_val(xConf, data[i]);
        double y = resolve_val(yConf, data[i]);

        if (first) {
            path << "M " << x << " " << y;
            first = false;
        } else {
            if (curve == "step") {
                path << " L " << x << " " << py << " L " << x << " " << y;
            } else if (curve == "smooth") {
                if (hasPP) {
                    double t = 0.5;
                    double cp1x = px + (x - ppx) * t / 6.0;
                    double cp2x = x - (x - px) * t / 6.0;
                    path << " C " << cp1x << " " << py << " " << cp2x << " " << y << " " << x << " " << y;
                } else {
                    path << " Q " << (px + x) / 2.0 << " " << py << " " << x << " " << y;
                }
            } else if (curve == "basis") {
                path << " S " << x << " " << y << " " << x << " " << y;
            } else { // linear
                path << " L " << x << " " << y;
            }
        }
        ppx = px; ppy = py;
        px = x; py = y;
        if (i >= 1) hasPP = true;
    }
    return path.str();
}

// --- Generador de Áreas ---

inline std::string build_area_path(const json& gen, const json& data) {
    if (!gen.is_object() || !data.is_array() || data.empty()) return "";
    json xConf = gen.value("x", "x");
    json y0Conf = gen.value("y0", 0.0);
    json y1Conf = gen.value("y1", "y1");

    std::ostringstream path;
    bool first = true;
    // Línea superior (Y1)
    for (const auto& row : data) {
        double x = resolve_val(xConf, row);
        double y1 = resolve_val(y1Conf, row);
        if (first) { path << "M " << x << " " << y1; first = false; }
        else path << " L " << x << " " << y1;
    }
    // Línea base (Y0) - recorrido inverso
    for (auto it = data.rbegin(); it != data.rend(); ++it) {
        double x = resolve_val(xConf, *it);
        double y0 = resolve_val(y0Conf, *it);
        path << " L " << x << " " << y0;
    }
    path << " Z";
    return path.str();
}

// --- Generador de Pie & Arcs ---

inline json build_pie(const json& gen, const json& data) {
    if (!gen.is_object() || !data.is_array() || data.empty()) return json::array();
    json valConf = gen.value("value", "value");
    double startAngle = gen.value("startAngle", 0.0);
    double endAngle = gen.value("endAngle", 2.0*M_PI);
    bool shouldSort = gen.value("sort", true);

    struct Item { json original; double val; size_t idx; };
    std::vector<Item> items;
    double total = 0.0;

    for (size_t i=0; i<data.size(); ++i) {
        double v = std::max(0.0, resolve_val(valConf, data[i]));
        total += v;
        items.push_back({data[i], v, i});
    }

    if (total < 1e-12) return json::array();
    if (shouldSort) std::stable_sort(items.begin(), items.end(), [](const Item& a, const Item& b){ return b.val < a.val; });

    json result = json::array();
    double current = startAngle;
    double range = endAngle - startAngle;
    for (const auto& it : items) {
        double angle = (it.val / total) * range;
        result.push_back({
            {"data", it.original}, {"value", it.val}, {"index", it.idx},
            {"startAngle", current}, {"endAngle", current + angle}
        });
        current += angle;
    }
    return result;
}

inline std::string arc_path(const json& gen, const json& slice) {
    if (!slice.is_object() || !slice.contains("startAngle")) return "";
    
    double innerR = gen.value("innerRadius", 0.0);
    double outerR = gen.value("outerRadius", 100.0);
    double startA = slice.value("startAngle", 0.0) - M_PI/2.0;
    double endA   = slice.value("endAngle", 0.0) - M_PI/2.0;

    auto pX = [](double r, double a) { return r * std::cos(a); };
    auto pY = [](double r, double a) { return r * std::sin(a); };
    
    double diff = slice.value("endAngle", 0.0) - slice.value("startAngle", 0.0);
    int largeArc = (diff > M_PI) ? 1 : 0;

    std::ostringstream path;
    path << "M " << pX(outerR, startA) << " " << pY(outerR, startA)
         << " A " << outerR << " " << outerR << " 0 " << largeArc << " 1 " << pX(outerR, endA) << " " << pY(outerR, endA);
    
    if (innerR > 0.001) {
        path << " L " << pX(innerR, endA) << " " << pY(innerR, endA)
             << " A " << innerR << " " << innerR << " 0 " << largeArc << " 0 " << pX(innerR, startA) << " " << pY(innerR, startA)
             << " Z";
    } else {
        path << " L 0 0 Z";
    }
    return path.str();
}

// --- Generador Radial ---

inline std::string radial_line_path(const json& gen, const json& data) {
    if (!gen.is_object() || !data.is_array() || data.empty()) return "";
    json aConf = gen.value("angle", "angle");
    json rConf = gen.value("radius", "radius");
    std::ostringstream path;
    bool first = true;
    double fx=0, fy=0;

    for (const auto& row : data) {
        double a = resolve_val(aConf, row) - M_PI/2.0;
        double r = resolve_val(rConf, row);
        double x = r * std::cos(a), y = r * std::sin(a);
        if (first) { path << "M " << x << " " << y; fx=x; fy=y; first=false; }
        else path << " L " << x << " " << y;
    }
    if (!first) path << " L " << fx << " " << fy;
    return path.str();
}

// --- Registro en Inja ---

inline void register_chart_shapes_functions(inja::Environment& env) {
    
    // Helper para setters que aceptan tanto String (campo) como Number (fijo)
    auto add_setter = [&](const std::string& name, const std::string& key) {
        env.add_callback(name, 2, [key](inja::Arguments& args) -> json {
            json g = *args[0];
            // Inja pasa punteros a json. Copiamos el valor directamente (sea string o num)
            g[key] = *args[1]; 
            return g;
        });
    };

    // Constructores de objetos de configuración
    env.add_callback("shapeSymbol", 0, [](inja::Arguments&) { return make_shape_defaults("symbol"); });
    env.add_callback("shapeLine", 0, [](inja::Arguments&) { return make_shape_defaults("line"); });
    env.add_callback("shapeArea", 0, [](inja::Arguments&) { return make_shape_defaults("area"); });
    env.add_callback("shapePie", 0, [](inja::Arguments&) { return make_shape_defaults("pie"); });
    env.add_callback("shapeArc", 0, [](inja::Arguments&) { return make_shape_defaults("arc"); });
    env.add_callback("shapeRadialLine", 0, [](inja::Arguments&) { return make_shape_defaults("radialLine"); });

    // Setters Polimórficos
    add_setter("x", "x");
    add_setter("y", "y");
    add_setter("y0", "y0");
    add_setter("y1", "y1");
    add_setter("value", "value");
    add_setter("size", "size");
    add_setter("shape", "shape");
    add_setter("curve", "curve");
    add_setter("innerRadius", "innerRadius");
    add_setter("outerRadius", "outerRadius");
    add_setter("angle", "angle");
    add_setter("radius", "radius");

    // Especial para booleanos
    env.add_callback("sortValues", 2, [](inja::Arguments& args) -> json {
        json g = *args[0]; g["sort"] = args[1]->get<bool>(); return g;
    });

    // Funciones de Generación de Rutas (Finales)
    env.add_callback("symbol", 1, [](inja::Arguments& args) { return symbol_path(*args[0]); });
    env.add_callback("line", 2, [](inja::Arguments& args) { return build_line_path(*args[0], *args[1]); });
    env.add_callback("area", 2, [](inja::Arguments& args) { return build_area_path(*args[0], *args[1]); });
    env.add_callback("pie", 2, [](inja::Arguments& args) { return build_pie(*args[0], *args[1]); });
    env.add_callback("arc", 2, [](inja::Arguments& args) { return arc_path(*args[0], *args[1]); });
    env.add_callback("radialLine", 2, [](inja::Arguments& args) { return radial_line_path(*args[0], *args[1]); });
}

#endif