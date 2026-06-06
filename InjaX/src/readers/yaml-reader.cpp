#include <fkYAML/node.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <functional>

using json = nlohmann::json;

#if defined(_WIN32) || defined(_WIN64)
  #define EXPORT __declspec(dllexport)
#elif defined(__APPLE__) || defined(__linux__)
  #define EXPORT __attribute__((visibility("default")))
#else
  #define EXPORT
#endif

extern "C" {
    EXPORT json parse_data(const std::string& filename) {
        if (filename.empty()) return json::object();

        try {
            std::ifstream file(filename);
            if (!file.is_open()) return json::object();        
            fkyaml::node yaml_root = fkyaml::node::deserialize(file);
            
            std::function<json(const fkyaml::node&)> convert = [&](const fkyaml::node& n) -> json {
                // Manejar valores nulos
                if (n.is_null()) {
                    return nullptr;
                }
                
                // Manejar booleanos
                if (n.is_boolean()) {
                    return n.as_bool();
                }
                
                // Manejar enteros (también captura floats internamente como int64_t o double)
                if (n.is_integer()) {
                    // Intentar obtener como int64_t primero
                    try {
                        return n.as_int();
                    } catch (...) {
                        // Si falla, intentar como double
                        try {
                            return n.as_float();
                        } catch (...) {
                            return n.as_int(); // reintentar
                        }
                    }
                }
                
                // Manejar strings (incluye números que se trataron como string)
                if (n.is_string()) {
                    std::string val = n.as_str();
                    // Manejar valores especiales en strings
                    if (val == "true") return true;
                    if (val == "false") return false;
                    if (val == "null") return nullptr;
                    
                    // Intentar convertir strings numéricos a números reales
                    try {
                        size_t pos;
                        double d = std::stod(val, &pos);
                        if (pos == val.length()) return d;
                    } catch (...) {}
                    
                    return val;
                }
                
                // Manejar secuencias (arrays)
                if (n.is_sequence()) {
                    json arr = json::array();
                    for (const auto& child : n.as_seq()) {
                        arr.push_back(convert(child));
                    }
                    return arr;
                }
                
                // Manejar mappings (objetos/diccionarios)
                if (n.is_mapping()) {
                    json obj = json::object();
                    
                    // Usar map_items() si está disponible (fkYAML >= 0.4.1) [citation:5]
                    #ifdef FKYAML_VERSION_MAJOR  // Detectar versión moderna
                    for (const auto& [key_node, value_node] : n.map_items()) {
                        std::string key_str;
                        if (key_node.is_string()) {
                            key_str = key_node.as_str();
                        } else if (key_node.is_integer()) {
                            key_str = std::to_string(key_node.as_int());
                        } else {
                            try {
                                key_str = key_node.as_str();
                            } catch (...) {
                                key_str = "unknown";
                            }
                        }
                        obj[key_str] = convert(value_node);
                    }
                    #else
                    // Fallback para versiones anteriores
                    for (auto it = n.begin(); it != n.end(); ++it) {
                        std::string key_str;
                        const auto& key_node = it.key();
                        if (key_node.is_string()) {
                            key_str = key_node.as_str();
                        } else if (key_node.is_integer()) {
                            key_str = std::to_string(key_node.as_int());
                        } else {
                            try {
                                key_str = key_node.as_str();
                            } catch (...) {
                                key_str = "unknown";
                            }
                        }
                        obj[key_str] = convert(it.value());
                    }
                    #endif
                    
                    return obj;
                }
                
                // Si no se pudo determinar el tipo
                return nullptr;
            };
            
            return convert(yaml_root);
            
        } catch (const fkyaml::exception& e) {
            std::cerr << "YAML parsing error: " << e.what() << std::endl;
            return json::object();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return json::object();
        }
    }
}