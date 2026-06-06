// chart-time.hpp
#ifndef CHART_TIME_HPP
#define CHART_TIME_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <regex>
#include <map>
#include <functional>

using json = nlohmann::json;

namespace chart_time {

// Forward declarations
std::string format_time(const std::time_t& time, const std::string& specifier, bool utc = false);
std::time_t parse_time(const std::string& date_str, const std::string& specifier, bool utc = false);

// Helper function to convert specifier to strftime/strptime format
inline std::string convert_d3_specifier(const std::string& d3_specifier) {
    std::string result = d3_specifier;
    
    // D3 specifier to strftime/strptime mapping
    std::map<std::string, std::string> mappings = {
        {"%Y", "%Y"}, // 4-digit year
        {"%y", "%y"}, // 2-digit year
        {"%m", "%m"}, // month (01-12)
        {"%d", "%d"}, // day (01-31)
        {"%H", "%H"}, // hour (00-23)
        {"%I", "%I"}, // hour (01-12)
        {"%M", "%M"}, // minute (00-59)
        {"%S", "%S"}, // second (00-59)
        {"%L", "%3"}  // milliseconds (not standard, handled specially)
    };
    
    for (const auto& [d3, strf] : mappings) {
        size_t pos = 0;
        while ((pos = result.find(d3, pos)) != std::string::npos) {
            result.replace(pos, d3.length(), strf);
            pos += strf.length();
        }
    }
    
    return result;
}

// Format a date using D3 specifiers
inline std::string format_time(const std::time_t& time, const std::string& specifier, bool utc) {
    std::tm tm;
    if (utc) {
        tm = *std::gmtime(&time);
    } else {
        tm = *std::localtime(&time);
    }
    
    // Handle milliseconds specially
    std::string processed_specifier = specifier;
    bool has_milliseconds = false;
    
    size_t ms_pos = processed_specifier.find("%L");
    if (ms_pos != std::string::npos) {
        has_milliseconds = true;
        processed_specifier.replace(ms_pos, 2, "%S"); // Temporary replacement
    }
    
    // Convert D3 specifier to strftime format
    std::string strf_format = convert_d3_specifier(processed_specifier);
    
    // Format the time
    std::stringstream ss;
    ss << std::put_time(&tm, strf_format.c_str());
    std::string result = ss.str();
    
    // Add milliseconds if requested
    if (has_milliseconds) {
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::from_time_t(time).time_since_epoch()
        ).count() % 1000;
        
        std::stringstream ms_ss;
        ms_ss << std::setfill('0') << std::setw(3) << ms;
        
        // Insert milliseconds at the original %L position
        std::string final_result;
        size_t original_pos = specifier.find("%L");
        if (original_pos != std::string::npos && original_pos <= result.length()) {
            final_result = result.substr(0, original_pos) + 
                          ms_ss.str() + 
                          result.substr(original_pos);
        } else {
            final_result = result;
        }
        return final_result;
    }
    
    return result;
}

// Parse a date string using D3 specifiers
inline std::time_t parse_time(const std::string& date_str, const std::string& specifier, bool utc) {
    std::tm tm = {};
    std::string processed_specifier = specifier;
    
    // Handle milliseconds by removing %L and parsing separately if needed
    bool has_milliseconds = specifier.find("%L") != std::string::npos;
    if (has_milliseconds) {
        // Remove %L for basic parsing
        processed_specifier = std::regex_replace(specifier, std::regex("%L"), "");
    }
    
    std::string strptime_format = convert_d3_specifier(processed_specifier);
    
    // Parse using strptime
    std::istringstream ss(date_str);
    ss >> std::get_time(&tm, strptime_format.c_str());
    
    if (ss.fail()) {
        return 0; // Return epoch on failure
    }
    
    // Convert to time_t
    std::time_t result;
    if (utc) {
        // For UTC, we need to handle timezone carefully
        // Simplified approach: assume tm is UTC and convert
        result = std::mktime(&tm);
        // Adjust for local timezone offset
        std::time_t local = std::mktime(std::localtime(&result));
        std::time_t gmt = std::mktime(std::gmtime(&result));
        result += (local - gmt);
    } else {
        result = std::mktime(&tm);
    }
    
    return result;
}

// Main API functions (exactly like D3.js)

/**
 * Creates a function that parses strings to Date objects (local time)
 * Usage: auto parser = timeParse("%Y-%m-%d");
 *        auto date = parser("2024-12-25");
 */
inline std::function<std::time_t(const std::string&)> timeParse(const std::string& specifier) {
    return [specifier](const std::string& date_str) -> std::time_t {
        return parse_time(date_str, specifier, false);
    };
}

/**
 * Creates a function that formats Date objects to strings (local time)
 * Usage: auto formatter = timeFormat("%Y-%m-%d");
 *        std::string str = formatter(std::time(nullptr));
 */
inline std::function<std::string(const std::time_t&)> timeFormat(const std::string& specifier) {
    return [specifier](const std::time_t& time) -> std::string {
        return format_time(time, specifier, false);
    };
}

/**
 * Creates a function that parses strings to Date objects (UTC time)
 * Usage: auto parser = utcParse("%Y-%m-%d");
 *        auto date = parser("2024-12-25");
 */
inline std::function<std::time_t(const std::string&)> utcParse(const std::string& specifier) {
    return [specifier](const std::string& date_str) -> std::time_t {
        return parse_time(date_str, specifier, true);
    };
}

/**
 * Creates a function that formats Date objects to strings (UTC time)
 * Usage: auto formatter = utcFormat("%Y-%m-%d");
 *        std::string str = formatter(std::time(nullptr));
 */
inline std::function<std::string(const std::time_t&)> utcFormat(const std::string& specifier) {
    return [specifier](const std::time_t& time) -> std::string {
        return format_time(time, specifier, true);
    };
}

/**
 * ISO 8601 parser (UTC)
 * Usage: auto date = isoParse("2024-12-25T14:30:00.000Z");
 */
inline std::time_t isoParse(const std::string& iso_string) {
    // Parse ISO 8601 format: YYYY-MM-DDThh:mm:ss.sssZ
    std::regex iso_regex(R"((\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})\.?(\d{3})?Z?)");
    std::smatch match;
    
    if (std::regex_match(iso_string, match, iso_regex)) {
        std::tm tm = {};
        tm.tm_year = std::stoi(match[1]) - 1900;
        tm.tm_mon = std::stoi(match[2]) - 1;
        tm.tm_mday = std::stoi(match[3]);
        tm.tm_hour = std::stoi(match[4]);
        tm.tm_min = std::stoi(match[5]);
        tm.tm_sec = std::stoi(match[6]);
        
        std::time_t result = std::mktime(&tm);
        
        // Adjust to UTC (assuming the input is UTC)
        std::time_t local = std::mktime(std::localtime(&result));
        std::time_t gmt = std::mktime(std::gmtime(&result));
        result += (local - gmt);
        
        return result;
    }
    
    return 0; // Return epoch on failure
}

/**
 * ISO 8601 formatter (UTC)
 * Usage: auto iso_str = isoFormat(std::time(nullptr));
 */
inline std::string isoFormat(const std::time_t& time) {
    return format_time(time, "%Y-%m-%dT%H:%M:%S.%LZ", true);
}

// Convenience functions for JSON integration (like chart-axis.hpp)
inline json timeParseJson(const std::string& specifier) {
    json result;
    result["type"] = "timeParse";
    result["specifier"] = specifier;
    return result;
}

inline json timeFormatJson(const std::string& specifier) {
    json result;
    result["type"] = "timeFormat";
    result["specifier"] = specifier;
    return result;
}

} // namespace chart_time

// Inja environment registration (matching the pattern from chart-axis.hpp)
inline void register_chart_time_functions(inja::Environment& env) {
    using namespace chart_time;
    
    // Register parser creators
    env.add_callback("timeParse", 1, [](inja::Arguments& args) -> json {
        std::string specifier = args[0]->is_string() ? args[0]->get<std::string>() : "";
        return timeParseJson(specifier);
    });
    
    env.add_callback("timeFormat", 1, [](inja::Arguments& args) -> json {
        std::string specifier = args[0]->is_string() ? args[0]->get<std::string>() : "";
        return timeFormatJson(specifier);
    });
    
    env.add_callback("utcParse", 1, [](inja::Arguments& args) -> json {
        std::string specifier = args[0]->is_string() ? args[0]->get<std::string>() : "";
        json result;
        result["type"] = "utcParse";
        result["specifier"] = specifier;
        return result;
    });
    
    env.add_callback("utcFormat", 1, [](inja::Arguments& args) -> json {
        std::string specifier = args[0]->is_string() ? args[0]->get<std::string>() : "";
        json result;
        result["type"] = "utcFormat";
        result["specifier"] = specifier;
        return result;
    });
    
    env.add_callback("isoParse", 0, [](inja::Arguments& args) -> json {
        json result;
        result["type"] = "isoParse";
        return result;
    });
    
    env.add_callback("isoFormat", 0, [](inja::Arguments& args) -> json {
        json result;
        result["type"] = "isoFormat";
        return result;
    });
    
    // Add convenience functions for date manipulation
    env.add_callback("now", 0, [](inja::Arguments& args) -> json {
        return json(std::time(nullptr));
    });
}

#endif // CHART_TIME_HPP