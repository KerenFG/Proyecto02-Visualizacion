// SPDX-License-Identifier: MIT
// Project:   InjaX
// File:      src/string-filters.hpp

#pragma once
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <regex>
#include <vector>
#include <string>

namespace custom_string {

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

inline std::string center(const std::string &input, int width, const std::string &fill = " ") {
  if (width <= 0 || static_cast<int>(input.size()) >= width) return input;

  std::string safe_fill = fill.empty() ? " " : fill;

  int total_pad = width - static_cast<int>(input.size());
  int left_pad = total_pad / 2;
  int right_pad = total_pad - left_pad;

  std::string left, right;
  while (static_cast<int>(left.size()) < left_pad) left += safe_fill;
  left = left.substr(0, left_pad);

  while (static_cast<int>(right.size()) < right_pad) right += safe_fill;
  right = right.substr(0, right_pad);

  return left + input + right;
}

inline std::string reverse(const std::string &input) {
  std::string out = input;
  std::reverse(out.begin(), out.end());
  return out;
}

inline std::string trim(const std::string &input) {
  const std::string ws = " \t\n\r\f\v";
  size_t start = input.find_first_not_of(ws);
  if (start == std::string::npos) return "";
  size_t end = input.find_last_not_of(ws);
  return input.substr(start, end - start + 1);
}

inline std::string truncate(const std::string &input, int length, const std::string &end = "...") {
  if (length <= 0) return "";
  if (static_cast<int>(input.size()) <= length) return input;
  if (static_cast<int>(end.size()) >= length) return input.substr(0, length);
  return input.substr(0, length - static_cast<int>(end.size())) + end;
}

inline std::string title(const std::string &input) {
  std::string out = input;
  bool cap = true;
  for (size_t i = 0; i < out.size(); ++i) {
    unsigned char c = static_cast<unsigned char>(out[i]);
    if (std::isspace(c) || std::ispunct(c)) {
      cap = true;
    } else if (cap) {
      out[i] = static_cast<char>(std::toupper(c));
      cap = false;
    } else {
      out[i] = static_cast<char>(std::tolower(c));
    }
  }
  return out;
}

inline int wordcount(const std::string &input) {
  std::istringstream iss(input);
  int count = 0;
  std::string token;
  while (iss >> token) ++count;
  return count;
}

inline std::string wordwrap(const std::string &input, int width, const std::string &break_str = "\n", bool break_long_words = false) {
  if (width <= 0) return input;

  std::istringstream iss(input);
  std::string word;
  std::ostringstream oss;
  int current_length = 0;
  bool is_first_word = true;

  while (iss >> word) {
    int word_len = static_cast<int>(word.size());

    if (!is_first_word && current_length + 1 + word_len <= width) {
      oss << " " << word;
      current_length += 1 + word_len;
    } else {
      if (!is_first_word && word_len <= width) {
        oss << break_str;
      }

      if (word_len > width && break_long_words) {
        int pos = 0;
        while (pos < word_len) {
          if (pos > 0 || !is_first_word) {
            oss << break_str;
          }
          int chunk = std::min(width, word_len - pos);
          oss << word.substr(pos, chunk);
          pos += chunk;
          current_length = chunk;
        }
      } else {
        oss << word;
        current_length = word_len;
      }
    }
    is_first_word = false;
  }
  return oss.str();
}

inline void register_filters(inja::Environment &env) {
  env.add_callback("center", 2, [](inja::Arguments args) {
    return center(to_string_from_json(*args[0]), args[1]->get<int>());
  });
  env.add_callback("center", 3, [](inja::Arguments args) {
    return center(to_string_from_json(*args[0]), args[1]->get<int>(), to_string_from_json(*args[2]));
  });
  env.add_callback("reverse", 1, [](inja::Arguments args) {
    return reverse(to_string_from_json(*args[0]));
  });
  env.add_callback("trim", 1, [](inja::Arguments args) {
    return trim(to_string_from_json(*args[0]));
  });
  env.add_callback("truncate", 2, [](inja::Arguments args) {
    return truncate(to_string_from_json(*args[0]), args[1]->get<int>());
  });
  env.add_callback("truncate", 3, [](inja::Arguments args) {
    return truncate(to_string_from_json(*args[0]), args[1]->get<int>(), to_string_from_json(*args[2]));
  });
  env.add_callback("title", 1, [](inja::Arguments args) {
    return title(to_string_from_json(*args[0]));
  });
  env.add_callback("wordcount", 1, [](inja::Arguments args) {
    return wordcount(to_string_from_json(*args[0]));
  });
  env.add_callback("wordwrap", 2, [](inja::Arguments args) {
    return wordwrap(to_string_from_json(*args[0]), args[1]->get<int>());
  });
  env.add_callback("wordwrap", 3, [](inja::Arguments args) {
    return wordwrap(to_string_from_json(*args[0]), args[1]->get<int>(), to_string_from_json(*args[2]));
  });
  env.add_callback("wordwrap", 4, [](inja::Arguments args) {
    return wordwrap(to_string_from_json(*args[0]), args[1]->get<int>(), to_string_from_json(*args[2]), args[3]->get<bool>());
  });
  env.add_callback("string", 1, [](inja::Arguments args) {
    return to_string_from_json(*args[0]);
  });
}

} // namespace custom_string