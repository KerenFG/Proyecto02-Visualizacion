// SPDX-License-Identifier: MIT
// Project:   InjaX
// File:      src/html-filters.hpp

#pragma once
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <regex>
#include <algorithm>
#include <cctype>

namespace custom_html {

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

inline std::string html_escape(const std::string &s) {
  std::string out;
  out.reserve(s.size() * 1.2);
  for (unsigned char c : s) {
    switch (c) {
      case '&':  out.append("&amp;"); break;
      case '<':  out.append("&lt;"); break;
      case '>':  out.append("&gt;"); break;
      case '"':  out.append("&quot;"); break;
      case '\'': out.append("&#39;"); break;
      case '/':  out.append("&#x2F;"); break;
      default:   out.push_back(static_cast<char>(c)); break;
    }
  }
  return out;
}

inline std::string urlencode(const std::string &input) {
  std::string escaped;
  escaped.reserve(input.size() * 3);

  char hex_buf[4];
  for (unsigned char c : input) {
    if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      escaped.push_back(static_cast<char>(c));
    } else {
      snprintf(hex_buf, sizeof(hex_buf), "%%%02X", c);
      escaped.append(hex_buf);
    }
  }
  return escaped;
}

inline std::string striptags(const std::string &input) {
  static const std::regex tags(R"(<[^>]*>)", std::regex::ECMAScript);
  return std::regex_replace(input, tags, "");
}

inline std::string urlize(const std::string &input, bool trim_url_limit = true, int max_length = 50) {
  static const std::regex url_re(R"((https?:\/\/[^\s<>"']+|www\.[^\s<>"']+))", std::regex::icase);
  std::string result;
  std::sregex_iterator it(input.begin(), input.end(), url_re);
  std::sregex_iterator end;
  size_t last_pos = 0;

  for (; it != end; ++it) {
    std::smatch m = *it;
    size_t match_pos = m.position();
    size_t match_len = m.length();

    result.append(html_escape(input.substr(last_pos, match_pos - last_pos)));

    std::string url = m.str();
    std::string href = url;
    if (href.rfind("http://", 0) != 0 && href.rfind("https://", 0) != 0) {
      href = "http://" + href;
    }

    std::string text = url;
    if (trim_url_limit && static_cast<int>(text.size()) > max_length) {
      int safe_max = std::max(5, max_length);
      int keep = safe_max - 3;
      if (keep > 6) {
        int head = keep / 2;
        int tail = keep - head;
        text = text.substr(0, head) + "..." + text.substr(text.size() - tail);
      } else {
        text = text.substr(0, keep) + "...";
      }
    }

    result.append("<a href=\"");
    result.append(html_escape(href));
    result.append("\">");
    result.append(html_escape(text));
    result.append("</a>");

    last_pos = match_pos + match_len;
  }

  if (last_pos < input.size()) {
    result.append(html_escape(input.substr(last_pos)));
  }

  return result;
}

inline std::string xmlattr(const json &obj) {
  if (!obj.is_object()) return std::string();
  std::ostringstream oss;
  for (auto it = obj.begin(); it != obj.end(); ++it) {
    std::string key = it.key();
    std::string val = it.value().is_string() ? it.value().get<std::string>() : it.value().dump();
    oss << ' ' << key << "=\"" << html_escape(val) << '"';
  }
  return oss.str();
}

inline void register_filters(inja::Environment &env) {
  env.add_callback("escape", 1, [](inja::Arguments args) {
    return html_escape(to_string_from_json(*args[0]));
  });

  env.add_callback("forceescape", 1, [](inja::Arguments args) {
    return html_escape(to_string_from_json(*args[0]));
  });

  env.add_callback("safe", 1, [](inja::Arguments args) {
    return to_string_from_json(*args[0]);
  });

  env.add_callback("striptags", 1, [](inja::Arguments args) {
    return striptags(to_string_from_json(*args[0]));
  });

  env.add_callback("xmlattr", 1, [](inja::Arguments args) {
    return xmlattr(*args[0]);
  });

  env.add_callback("urlencode", 1, [](inja::Arguments args) {
    return urlencode(to_string_from_json(*args[0]));
  });

  env.add_callback("urlize", 1, [](inja::Arguments args) {
    return urlize(to_string_from_json(*args[0]));
  });

  env.add_callback("urlize", 2, [](inja::Arguments args) {
    return urlize(to_string_from_json(*args[0]), true, args[1]->get<int>());
  });
}

} // namespace custom_html