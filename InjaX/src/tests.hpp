// SPDX-License-Identifier: MIT
// Project:   InjaX
// File:      src/tests.hpp

#pragma once
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <set>
#include <regex>

namespace custom_tests {

using json = nlohmann::json;

inline std::string to_string_from_json(const json &j) {
  if (j.is_string()) return j.get<std::string>();
  if (j.is_null()) return "";
  if (j.is_boolean()) return j.get<bool>() ? "true" : "false";
  if (j.is_number()) {
    std::ostringstream oss;
    oss << j;
    return oss.str();
  }
  return j.dump();
}

inline bool is_escaped_html(const std::string &s) {
  return s.find("&lt;") != std::string::npos ||
         s.find("&gt;") != std::string::npos ||
         s.find("&amp;") != std::string::npos ||
         s.find("&quot;") != std::string::npos ||
         s.find("&#39;") != std::string::npos;
}

inline bool isString(const json &j) {
  return j.is_string();
}

inline bool isArray(const json &j) {
  return j.is_array();
}

inline bool isObject(const json &j) {
  return j.is_object();
}

inline bool isNumber(const json &j) {
  return j.is_number();
}

inline bool isInteger(const json &j) {
  return j.is_number_integer();
}

inline bool isFloat(const json &j) {
  return j.is_number_float();
}

inline bool isBoolean(const json &j) {
  return j.is_boolean();
}

inline bool isNull(const json &j) {
  return j.is_null();
}

inline bool isIterable(const json &j) {
  return j.is_array() || j.is_object();
}

inline bool isMapping(const json &j) {
  return j.is_object();
}

inline bool isSequence(const json &j) {
  return j.is_array();
}

inline bool isEven(const json &j) {
  if (!j.is_number_integer()) return false;
  int64_t val = j.get<int64_t>();
  return val % 2 == 0;
}

inline bool isOdd(const json &j) {
  if (!j.is_number_integer()) return false;
  int64_t val = j.get<int64_t>();
  return val % 2 != 0;
}

inline bool isDivisibleBy(const json &j, const json &divisor) {
  if (!j.is_number_integer() || !divisor.is_number_integer()) return false;
  int64_t dividend = j.get<int64_t>();
  int64_t div = divisor.get<int64_t>();
  return div != 0 && dividend % div == 0;
}

inline bool isLower(const json &j) {
  if (!j.is_string()) return false;
  std::string s = j.get<std::string>();
  return std::all_of(s.begin(), s.end(), [](unsigned char c) {
    return !std::isalpha(c) || std::islower(c);
  });
}

inline bool isUpper(const json &j) {
  if (!j.is_string()) return false;
  std::string s = j.get<std::string>();
  return std::all_of(s.begin(), s.end(), [](unsigned char c) {
    return !std::isalpha(c) || std::isupper(c);
  });
}

inline bool isEmpty(const json &j) {
  if (j.is_string()) return j.get<std::string>().empty();
  if (j.is_array()) return j.empty();
  if (j.is_object()) return j.empty();
  return false;
}

inline bool contains(const json &haystack, const json &needle) {
  if (haystack.is_string() && needle.is_string()) {
    return haystack.get<std::string>().find(needle.get<std::string>()) != std::string::npos;
  }
  if (haystack.is_array()) {
    for (const auto &item : haystack) {
      if (item == needle) return true;
    }
  }
  if (haystack.is_object() && needle.is_string()) {
    return haystack.contains(needle.get<std::string>());
  }
  return false;
}

inline bool startsWith(const json &j, const json &prefix) {
  if (!j.is_string() || !prefix.is_string()) return false;
  std::string str = j.get<std::string>();
  std::string pre = prefix.get<std::string>();
  return str.size() >= pre.size() && str.compare(0, pre.size(), pre) == 0;
}

inline bool endsWith(const json &j, const json &suffix) {
  if (!j.is_string() || !suffix.is_string()) return false;
  std::string str = j.get<std::string>();
  std::string suf = suffix.get<std::string>();
  return str.size() >= suf.size() &&
         str.compare(str.size() - suf.size(), suf.size(), suf) == 0;
}

inline bool matches(const json &j, const json &pattern) {
  if (!j.is_string() || !pattern.is_string()) return false;
  try {
    std::regex re(pattern.get<std::string>());
    return std::regex_search(j.get<std::string>(), re);
  } catch (const std::regex_error &) {
    return false;
  }
}

inline bool isDefined(const json &j) {
  return !j.is_null();
}

inline bool isUndefined(const json &j) {
  return j.is_null();
}

inline bool isNone(const json &j) {
  return j.is_null();
}

inline bool hasKey(const json &j, const json &key) {
  if (!j.is_object() || !key.is_string()) return false;
  return j.contains(key.get<std::string>());
}

inline bool isContained(const json &item, const json &collection) {
  if (!collection.is_array()) return false;
  for (const auto &col_item : collection) {
    if (col_item == item) return true;
  }
  return false;
}

inline bool isEscaped(const json &j) {
  if (!j.is_string()) return false;
  return is_escaped_html(j.get<std::string>());
}

inline void register_tests(inja::Environment &env) {
  env.add_callback("isString", 1, [](inja::Arguments args) -> json {
    return isString(*args[0]);
  });

  env.add_callback("isArray", 1, [](inja::Arguments args) -> json {
    return isArray(*args[0]);
  });

  env.add_callback("isObject", 1, [](inja::Arguments args) -> json {
    return isObject(*args[0]);
  });

  env.add_callback("isNumber", 1, [](inja::Arguments args) -> json {
    return isNumber(*args[0]);
  });

  env.add_callback("isInteger", 1, [](inja::Arguments args) -> json {
    return isInteger(*args[0]);
  });

  env.add_callback("isFloat", 1, [](inja::Arguments args) -> json {
    return isFloat(*args[0]);
  });

  env.add_callback("isBoolean", 1, [](inja::Arguments args) -> json {
    return isBoolean(*args[0]);
  });

  env.add_callback("isNull", 1, [](inja::Arguments args) -> json {
    return isNull(*args[0]);
  });

  env.add_callback("isIterable", 1, [](inja::Arguments args) -> json {
    return isIterable(*args[0]);
  });

  env.add_callback("isMapping", 1, [](inja::Arguments args) -> json {
    return isMapping(*args[0]);
  });

  env.add_callback("isSequence", 1, [](inja::Arguments args) -> json {
    return isSequence(*args[0]);
  });

  env.add_callback("isEven", 1, [](inja::Arguments args) -> json {
    return isEven(*args[0]);
  });

  env.add_callback("isOdd", 1, [](inja::Arguments args) -> json {
    return isOdd(*args[0]);
  });

  env.add_callback("isDivisibleBy", 2, [](inja::Arguments args) -> json {
    return isDivisibleBy(*args[0], *args[1]);
  });

  env.add_callback("isLower", 1, [](inja::Arguments args) -> json {
    return isLower(*args[0]);
  });

  env.add_callback("isUpper", 1, [](inja::Arguments args) -> json {
    return isUpper(*args[0]);
  });

  env.add_callback("isEmpty", 1, [](inja::Arguments args) -> json {
    return isEmpty(*args[0]);
  });

  env.add_callback("contains", 2, [](inja::Arguments args) -> json {
    return contains(*args[0], *args[1]);
  });

  env.add_callback("startsWith", 2, [](inja::Arguments args) -> json {
    return startsWith(*args[0], *args[1]);
  });

  env.add_callback("endsWith", 2, [](inja::Arguments args) -> json {
    return endsWith(*args[0], *args[1]);
  });

  env.add_callback("matches", 2, [](inja::Arguments args) -> json {
    return matches(*args[0], *args[1]);
  });

  env.add_callback("isDefined", 1, [](inja::Arguments args) -> json {
    return isDefined(*args[0]);
  });

  env.add_callback("isUndefined", 1, [](inja::Arguments args) -> json {
    return isUndefined(*args[0]);
  });

  env.add_callback("isNone", 1, [](inja::Arguments args) -> json {
    return isNone(*args[0]);
  });

  env.add_callback("hasKey", 2, [](inja::Arguments args) -> json {
    return hasKey(*args[0], *args[1]);
  });

  env.add_callback("isContained", 2, [](inja::Arguments args) -> json {
    return isContained(*args[0], *args[1]);
  });

  env.add_callback("isEscaped", 1, [](inja::Arguments args) -> json {
    return isEscaped(*args[0]);
  });
}

} // namespace custom_tests