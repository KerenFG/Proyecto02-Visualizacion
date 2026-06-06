#include <csv2/reader.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
  #define EXPORT extern "C" __declspec(dllexport)
#else
  #define EXPORT extern "C"
#endif

using json = nlohmann::json;

json parse_value(const std::string& value) {
    if (value.empty()) return nullptr;
    
    try {
        size_t pos;
        if (value.find('.') != std::string::npos) {
            double d = std::stod(value, &pos);
            if (pos == value.size()) return d;
        } else {
            long long i = std::stoll(value, &pos);
            if (pos == value.size()) return i;
        }
    } catch (...) {
    }
    return value;
}

EXPORT json parse_data(const std::string& filename) {
    if (filename.empty()) return json::object();

    csv2::Reader<csv2::delimiter<','>, 
                 csv2::quote_character<'"'>, 
                 csv2::first_row_is_header<true>> csv;

    if (!csv.mmap(filename)) {
        return json::object();
    }

    const auto header = csv.header();
    json rows_array = json::array();

    for (const auto row : csv) {
        json row_obj = json::object();
        auto header_it = header.begin();
        
        for (const auto cell : row) {
            std::string value;
            cell.read_value(value);
            
            std::string column_name;
            (*header_it).read_value(column_name);

            row_obj[column_name] = parse_value(value);
            
            if (header_it != header.end()) {
                ++header_it;
            }
        }
        rows_array.push_back(row_obj);
    }

    json result;
    result["data"] = rows_array;
    return result;
}