// SPDX-License-Identifier: MIT
// Project:   InjaX
// File:      src/number-filters.hpp

#pragma once
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <cmath>
#include <climits>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <algorithm>

namespace custom_numeric {

using json = nlohmann::json;

inline double to_double(const json &j) {
  if (j.is_number()) return j.get<double>();
  if (j.is_string()) {
    try { return std::stod(j.get<std::string>()); } catch(...) { return 0.0; }
  }
  if (j.is_boolean()) return j.get<bool>() ? 1.0 : 0.0;
  return 0.0;
}

inline json abs(const json &value) {
  if (value.is_array()) {
    json out = json::array();
    for (const auto &el : value) {
      out.push_back(abs(el));
    }
    return out;
  }
  if (value.is_number_integer()) {
    auto v = value.get<long long>();
    if (v == LLONG_MIN) return LLONG_MAX;
    return (v < 0) ? -v : v;
  }
  if (value.is_number_unsigned()) return value;
  return std::fabs(to_double(value));
}

inline std::mt19937 &rng() {
  static thread_local std::mt19937 gen(static_cast<unsigned int>(
    std::chrono::high_resolution_clock::now().time_since_epoch().count()));
  return gen;
}

inline json random(const json &arg) {
  if (arg.is_array()) {
    if (arg.empty()) return json();
    std::uniform_int_distribution<size_t> dist(0, arg.size() - 1);
    return arg[dist(rng())];
  }
  if (arg.is_string()) {
    std::string s = arg.get<std::string>();
    if (s.empty()) return "";
    std::uniform_int_distribution<size_t> dist(0, s.size() - 1);
    return std::string(1, s[dist(rng())]);
  }
  return arg;
}

inline std::string filesizeformat(const json &value, bool binary = false) {
  long long bytes = value.is_number_integer() ? value.get<long long>() : static_cast<long long>(to_double(value));
  if (bytes < 0) bytes = 0;
  const char *units_si[] = { "B", "KB", "MB", "GB", "TB", "PB" };
  const char *units_bin[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB" };
  double base = binary ? 1024.0 : 1000.0;
  const char **units = binary ? units_bin : units_si;
  int unit = 0;
  double val_bytes = static_cast<double>(bytes);
  while (val_bytes >= base && unit < 5) {
    val_bytes /= base;
    ++unit;
  }
  std::ostringstream oss;
  if (unit == 0) {
    oss << static_cast<long long>(val_bytes) << ' ' << units[unit];
  } else {
    oss << std::fixed << std::setprecision(1) << val_bytes << ' ' << units[unit];
  }
  return oss.str();
}

inline std::string indent(const json &value, int width = 4, bool indent_first = false, bool blank = false) {
  std::string text = value.is_string() ? value.get<std::string>() : value.dump();
  std::string pad(width, ' ');
  std::ostringstream oss;
  std::string line;
  std::istringstream stream(text);
  bool is_first = true;
  while (std::getline(stream, line, '\n')) {
    if (!is_first) oss << '\n';
    if (line.empty() && !blank) {
    } else if (!is_first || indent_first) {
      oss << pad;
    }
    oss << line;
    is_first = false;
  }
  if (!text.empty() && text.back() == '\n') oss << '\n';
  return oss.str();
}

inline void register_filters(inja::Environment &env) {
  env.add_callback("abs", 1, [](inja::Arguments args) {
    return abs(*args[0]);
  });
  env.add_callback("random", 1, [](inja::Arguments args) { return random(*args[0]); });
  env.add_callback("filesizeformat", 1, [](inja::Arguments args) { return filesizeformat(*args[0], false); });
  env.add_callback("filesizeformat", 2, [](inja::Arguments args) { return filesizeformat(*args[0], args[1]->get<bool>()); });
  env.add_callback("indent", 1, [](inja::Arguments args) { return indent(*args[0], 4, false, false); });
  env.add_callback("indent", 2, [](inja::Arguments args) { return indent(*args[0], args[1]->get<int>(), false, false); });
  env.add_callback("indent", 3, [](inja::Arguments args) { return indent(*args[0], args[1]->get<int>(), args[2]->get<bool>(), false); });
  env.add_callback("indent", 4, [](inja::Arguments args) { return indent(*args[0], args[1]->get<int>(), args[2]->get<bool>(), args[3]->get<bool>()); });
}

}