#include <nlohmann/json.hpp>
#include <simpleini/SimpleIni.h>
#include <string>
#include <vector>
#include <map>

#if defined(_WIN32) || defined(_WIN64)
  #define EXPORT __declspec(dllexport)
#else
  #define EXPORT
#endif

using json = nlohmann::json;

EXPORT json parse_data(const std::string& filename) {
    if (filename.empty()) return json::object();

    try {
        CSimpleIniA ini;
        
        SI_Error rc = ini.LoadFile(filename.c_str());
        if (rc != SI_OK) {
            return json::object();
        }

        json result = json::object();
        
        CSimpleIniA::TNamesDepend sections;
        ini.GetAllSections(sections);
        
        for (const auto& section : sections) {
            std::string section_name = section.pItem;
            json section_obj = json::object();
            
            CSimpleIniA::TNamesDepend keys;
            ini.GetAllKeys(section.pItem, keys);
            
            for (const auto& key : keys) {
                std::string key_name = key.pItem;
                const char* value = ini.GetValue(section.pItem, key.pItem);
                
                if (value != nullptr) {
                    std::string str_value(value);
                    
                    bool is_number = true;
                    bool is_float = false;
                    
                    for (char c : str_value) {
                        if (c == '.') {
                            is_float = true;
                        } else if (!std::isdigit(c) && c != '-' && c != '.') {
                            is_number = false;
                            break;
                        }
                    }
                    
                    if (is_number && !str_value.empty()) {
                        try {
                            if (is_float) {
                                section_obj[key_name] = std::stod(str_value);
                            } else {
                                section_obj[key_name] = std::stoll(str_value);
                            }
                        } catch (...) {
                            section_obj[key_name] = str_value;
                        }
                    }
                    else if (str_value == "true" || str_value == "false" || 
                             str_value == "True" || str_value == "False" ||
                             str_value == "YES" || str_value == "NO" ||
                             str_value == "yes" || str_value == "no") {
                        bool bool_value = (str_value == "true" || str_value == "True" || 
                                         str_value == "YES" || str_value == "yes");
                        section_obj[key_name] = bool_value;
                    }
                    else {
                        section_obj[key_name] = str_value;
                    }
                } else {
                    section_obj[key_name] = json();
                }
            }
            
            result[section_name] = section_obj;
        }
        
        return result;
        
    } catch (const std::exception& e) {
        return json::object();
    } catch (...) {
        return json::object();
    }
}

EXPORT json get_value(const std::string& filename, 
                      const std::string& section, 
                      const std::string& key) {
    if (filename.empty()) return json();
    
    try {
        CSimpleIniA ini;
        SI_Error rc = ini.LoadFile(filename.c_str());
        if (rc != SI_OK) {
            return json();
        }
        
        const char* value = ini.GetValue(section.c_str(), key.c_str());
        if (value == nullptr) {
            return json();
        }
        
        std::string str_value(value);
        
        bool is_number = true;
        bool is_float = false;
        
        for (char c : str_value) {
            if (c == '.') {
                is_float = true;
            } else if (!std::isdigit(c) && c != '-' && c != '.') {
                is_number = false;
                break;
            }
        }
        
        if (is_number && !str_value.empty()) {
            try {
                if (is_float) {
                    return json(std::stod(str_value));
                } else {
                    return json(std::stoll(str_value));
                }
            } catch (...) {
                return json(str_value);
            }
        }
        else if (str_value == "true" || str_value == "false" || 
                 str_value == "True" || str_value == "False") {
            return json(str_value == "true" || str_value == "True");
        }
        else {
            return json(str_value);
        }
        
    } catch (...) {
        return json();
    }
}