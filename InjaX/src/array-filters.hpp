// SPDX-License-Identifier: MIT
// Project:   InjaX
// File:      src/array-filters.hpp

#pragma once
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <regex>
#include <cctype>

namespace custom_array {

using json = nlohmann::json;

inline std::vector<std::string> split_path(const std::string &path) {
  std::vector<std::string> parts;
  size_t i = 0;
  while (i < path.size()) {
    size_t j = path.find('.', i);
    if (j == std::string::npos) j = path.size();
    parts.push_back(path.substr(i, j - i));
    i = j + 1;
  }
  return parts;
}

inline bool looks_like_index(const std::string &s) {
  if (s.empty()) return false;
  size_t i = 0;
  if (s[0] == '-' || s[0] == '+') {
    if (s.size() == 1) return false;
    i = 1;
  }
  for (; i < s.size(); ++i) {
    if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
  }
  return true;
}

inline std::function<json(const json&)> make_attrgetter(const std::string &path) {
  auto parts = split_path(path);
  return [parts](const json &obj) -> json {
    const json *cur = &obj;
    for (const auto &p : parts) {
      if (cur == nullptr || cur->is_null()) return json();
      if (cur->is_object()) {
        if (cur->contains(p)) {
          cur = &((*cur)[p]);
          continue;
        }
      }
      if (cur->is_array() && looks_like_index(p)) {
        try {
          int idx = std::stoi(p);
          int n = static_cast<int>(cur->size());
          if (idx < 0) idx += n;
          if (idx >= 0 && idx < n) {
            cur = &((*cur)[idx]);
            continue;
          }
        } catch (...) {}
      }
      return json();
    }
    return *cur;
  };
}

inline bool is_truthy(const json &v) {
  if (v.is_null()) return false;
  if (v.is_boolean()) return v.get<bool>();
  if (v.is_number()) return v.get<double>() != 0.0;
  if (v.is_string()) return !v.get<std::string>().empty();
  if (v.is_array() || v.is_object()) return !v.empty();
  return false;
}

inline bool apply_test(const json &val, const std::string &test, const json &arg) {
  if (test.empty()) return is_truthy(val);
  if (test == "equalto") return val == arg;
  if (test == "defined") return !val.is_null();
  if (test == "none") return val.is_null();
  if (test == "in") {
    if (arg.is_array()) {
      return std::find(arg.begin(), arg.end(), val) != arg.end();
    } else if (arg.is_string() && val.is_string()) {
      return arg.get<std::string>().find(val.get<std::string>()) != std::string::npos;
    }
    return false;
  }
  if (test == "match" && val.is_string() && arg.is_string()) {
    try {
      std::regex re(arg.get<std::string>());
      return std::regex_search(val.get<std::string>(), re);
    } catch (...) {
      return false;
    }
  }
  return is_truthy(val);
}

inline json unique(const json &arr) {
  if (!arr.is_array()) return json::array();
  std::set<json> seen;
  json out = json::array();
  for (const auto &el : arr) {
    if (seen.insert(el).second) out.push_back(el);
  }
  return out;
}

inline double sum(const json &arr) {
  if (!arr.is_array()) return 0.0;
  double total = 0.0;
  for (const auto &el : arr) {
    if (el.is_number()) {
      total += el.get<double>();
    } else if (el.is_string()) {
      try { total += std::stod(el.get<std::string>()); } catch(...) {}
    }
  }
  return total;
}

inline json map(const json &seq, const json &key_or_path) {
  json out = json::array();
  if (!seq.is_array()) return out;

  if (key_or_path.is_string()) {
    auto getter = make_attrgetter(key_or_path.get<std::string>());
    for (const auto &el : seq) out.push_back(getter(el));
    return out;
  }

  if (key_or_path.is_number()) {
    int idx = static_cast<int>(key_or_path.get<double>());
    for (const auto &el : seq) {
      if (el.is_array()) {
        int n = static_cast<int>(el.size());
        int i = idx < 0 ? idx + n : idx;
        out.push_back((i >= 0 && i < n) ? el[i] : json(nullptr));
      } else {
        out.push_back(json(nullptr));
      }
    }
    return out;
  }
  return out;
}

inline json slice(const json &seq, int start, int stop, int step = 1) {
  json out = json::array();
  if (!seq.is_array() || step == 0) return out;

  int n = static_cast<int>(seq.size());
  if (n == 0) return out;

  if (start < 0) start += n;
  if (stop < 0) stop += n;

  start = std::clamp(start, 0, n);
  stop = std::clamp(stop, 0, n);

  if (step > 0) {
    for (int i = start; i < stop; i += step) {
      if (i >= 0 && i < n) out.push_back(seq[i]);
    }
  } else {
    for (int i = start; i > stop; i += step) {
      if (i >= 0 && i < n) out.push_back(seq[i]);
    }
  }
  return out;
}

inline json select(const json &seq, const json &value) {
  json out = json::array();
  if (!seq.is_array()) return out;
  for (const auto &el : seq) {
    if (el == value) out.push_back(el);
  }
  return out;
}

inline json reject(const json &seq, const json &value) {
  json out = json::array();
  if (!seq.is_array()) return out;
  for (const auto &el : seq) {
    if (el != value) out.push_back(el);
  }
  return out;
}

inline json selectattr(const json &seq, const std::string &attr, const std::string &test = "", const json &test_arg = json()) {
  json out = json::array();
  if (!seq.is_array()) return out;
  auto getter = make_attrgetter(attr);
  for (const auto &el : seq) {
    if (apply_test(getter(el), test, test_arg)) out.push_back(el);
  }
  return out;
}

inline json rejectattr(const json &seq, const std::string &attr, const std::string &test = "", const json &test_arg = json()) {
  json out = json::array();
  if (!seq.is_array()) return out;
  auto getter = make_attrgetter(attr);
  for (const auto &el : seq) {
    if (!apply_test(getter(el), test, test_arg)) out.push_back(el);
  }
  return out;
}

inline json batch(const json &seq, int size, const json &fill_with = json()) {
  json out = json::array();
  if (!seq.is_array() || size <= 0) return out;
  json current = json::array();
  for (const auto &el : seq) {
    current.push_back(el);
    if (static_cast<int>(current.size()) == size) {
      out.push_back(current);
      current = json::array();
    }
  }
  if (!current.empty()) {
    if (!fill_with.is_null()) {
      while (static_cast<int>(current.size()) < size) {
        current.push_back(fill_with);
      }
    }
    out.push_back(current);
  }
  return out;
}

inline json regroup(const json &seq, const std::string &attr) {
  json out = json::array();
  if (!seq.is_array()) return out;
  auto getter = make_attrgetter(attr);

  std::map<json, json> groups;
  for (const auto &el : seq) {
    json key = getter(el);
    groups[key].push_back(el);
  }
  for (const auto &p : groups) {
    json obj;
    obj["grouper"] = p.first;
    obj["list"] = p.second;
    out.push_back(obj);
  }
  return out;
}

inline json dictsort(const json &obj, const std::string &by = "key") {
  json out = json::array();
  if (!obj.is_object()) return out;

  struct SortItem {
    std::string key;
    json value;
  };
  std::vector<SortItem> items_vec;
  for (auto it = obj.begin(); it != obj.end(); ++it) {
    items_vec.push_back({it.key(), it.value()});
  }

  if (by == "value") {
    std::stable_sort(items_vec.begin(), items_vec.end(), [](const SortItem &a, const SortItem &b){
      if (a.value.is_number() && b.value.is_number()) {
        return a.value.template get<double>() < b.value.template get<double>();
      }
      if (a.value.is_string() && b.value.is_string()) {
        return a.value.template get<std::string>() < b.value.template get<std::string>();
      }
      return a.value.dump() < b.value.dump();
    });
  } else {
    std::stable_sort(items_vec.begin(), items_vec.end(), [](const SortItem &a, const SortItem &b){
      return a.key < b.key;
    });
  }

  for (const auto &item : items_vec) {
    json res_item;
    res_item["key"] = item.key;
    res_item["value"] = item.value;
    out.push_back(res_item);
  }
  return out;
}

inline json items(const json &obj) {
  json out = json::array();
  if (!obj.is_object()) return out;

  for (auto it = obj.begin(); it != obj.end(); ++it) {
    json item;
    item["key"] = it.key();
    item["value"] = it.value();
    out.push_back(item);
  }
  return out;
}

inline json make_list(const json &value) {
  if (value.is_array()) return value;
  if (value.is_object()) {
    json out = json::array();
    for (auto it = value.begin(); it != value.end(); ++it) {
      out.push_back(it.value());
    }
    return out;
  }
  return json::array({ value });
}

inline json sort_by(const json &seq, const std::string &attr) {
  if (!seq.is_array()) return json::array();
  json copy = seq;
  auto getter = make_attrgetter(attr);

  std::stable_sort(copy.begin(), copy.end(), [&](const json &a, const json &b){
    json va = getter(a);
    json vb = getter(b);
    if (va.is_null()) return false;
    if (vb.is_null()) return true;
    return va < vb;
  });
  return copy;
}

inline json make_object(const inja::Arguments& args) {
  json obj = json::object();
  for (size_t i = 0; i + 1 < args.size(); i += 2) {
    if (args[i]->is_string()) {
      obj[args[i]->get<std::string>()] = *args[i + 1];
    }
  }
  return obj;
}

inline json make_array(const inja::Arguments& args) {
  json arr = json::array();
  for (size_t i = 0; i < args.size(); ++i) {
    arr.push_back(*args[i]);
  }
  return arr;
}

inline json zip_fixed(const std::vector<json> &arrays) {
  json out = json::array();
  if (arrays.empty()) return out;

  for (const auto &arr : arrays) {
    if (!arr.is_array()) return out;
  }

  size_t min_size = arrays[0].size();
  for (const auto &arr : arrays) {
    min_size = std::min(min_size, arr.size());
  }

  for (size_t i = 0; i < min_size; ++i) {
    json tuple_data = json::array();
    for (const auto &arr : arrays) {
      tuple_data.push_back(arr[i]);
    }
    out.push_back(tuple_data);
  }
  return out;
}

inline json zip_array(const json &arrays) {
  if (!arrays.is_array()) return json::array();

  std::vector<json> vec;
  for (const auto &arr : arrays) {
    if (!arr.is_array()) return json::array();
    vec.push_back(arr);
  }
  return zip_fixed(vec);
}

inline json zip_obj(const json &keys, const std::vector<json> &arrays) {
  json out = json::array();

  if (!keys.is_array() || keys.empty()) return out;
  if (arrays.empty()) return out;

  for (const auto &arr : arrays) {
    if (!arr.is_array()) return out;
  }

  size_t min_size = arrays[0].size();
  for (const auto &arr : arrays) {
    min_size = std::min(min_size, arr.size());
  }

  size_t num_keys = std::min(keys.size(), arrays.size());

  for (size_t i = 0; i < min_size; ++i) {
    json obj = json::object();
    for (size_t j = 0; j < num_keys; ++j) {
      std::string key = keys[j].get<std::string>();
      obj[key] = arrays[j][i];
    }
    out.push_back(obj);
  }
  return out;
}

inline json append(const json &arr, const json &item) {
  if (!arr.is_array()) return json::array({item});
  json result = arr;
  result.push_back(item);
  return result;
}

inline void register_filters(inja::Environment &env) {
  env.add_callback("obj", -1, [](inja::Arguments args) { return make_object(args); });
  env.add_callback("array", -1, [](inja::Arguments args) { return make_array(args); });

  env.add_callback("unique", 1, [](inja::Arguments args) { return unique(*args[0]); });
  env.add_callback("sum", 1, [](inja::Arguments args) { return sum(*args[0]); });

  env.add_callback("slice", 3, [](inja::Arguments args) {
    return slice(*args[0], args[1]->get<int>(), args[2]->get<int>());
  });
  env.add_callback("slice", 4, [](inja::Arguments args) {
    return slice(*args[0], args[1]->get<int>(), args[2]->get<int>(), args[3]->get<int>());
  });

  env.add_callback("map", 2, [](inja::Arguments args) { return map(*args[0], *args[1]); });

  env.add_callback("select", 2, [](inja::Arguments args) { return select(*args[0], *args[1]); });
  env.add_callback("reject", 2, [](inja::Arguments args) { return reject(*args[0], *args[1]); });

  env.add_callback("selectattr", 2, [](inja::Arguments args) { return selectattr(*args[0], args[1]->get<std::string>()); });
  env.add_callback("selectattr", 3, [](inja::Arguments args) { return selectattr(*args[0], args[1]->get<std::string>(), args[2]->get<std::string>()); });
  env.add_callback("selectattr", 4, [](inja::Arguments args) { return selectattr(*args[0], args[1]->get<std::string>(), args[2]->get<std::string>(), *args[3]); });

  env.add_callback("rejectattr", 2, [](inja::Arguments args) { return rejectattr(*args[0], args[1]->get<std::string>()); });
  env.add_callback("rejectattr", 3, [](inja::Arguments args) { return rejectattr(*args[0], args[1]->get<std::string>(), args[2]->get<std::string>()); });
  env.add_callback("rejectattr", 4, [](inja::Arguments args) { return rejectattr(*args[0], args[1]->get<std::string>(), args[2]->get<std::string>(), *args[3]); });

  env.add_callback("batch", 2, [](inja::Arguments args) { return batch(*args[0], args[1]->get<int>()); });
  env.add_callback("batch", 3, [](inja::Arguments args) { return batch(*args[0], args[1]->get<int>(), *args[2]); });

  env.add_callback("regroup", 2, [](inja::Arguments args) { return regroup(*args[0], args[1]->get<std::string>()); });

  env.add_callback("dictsort", 1, [](inja::Arguments args) { return dictsort(*args[0], "key"); });
  env.add_callback("dictsort", 2, [](inja::Arguments args) { return dictsort(*args[0], args[1]->get<std::string>()); });

  env.add_callback("items", 1, [](inja::Arguments args) { return items(*args[0]); });
  env.add_callback("make_list", 1, [](inja::Arguments args) { return make_list(*args[0]); });

  env.add_callback("sort_by", 2, [](inja::Arguments args) { return sort_by(*args[0], args[1]->get<std::string>()); });

  env.add_callback("append", 2, [](inja::Arguments args) { return append(*args[0], *args[1]); });

  env.add_callback("tojson", 1, [](inja::Arguments args) -> std::string { return args[0]->dump(); });

  env.add_callback("zip", -1, [](inja::Arguments args) -> json {
    if (args.size() < 2) return json::array();
    std::vector<json> arrays;
    arrays.reserve(args.size());
    for (const auto &arg : args) {
      arrays.push_back(*arg);
    }
    return zip_fixed(arrays);
  });

  env.add_callback("zip_array", 1, [](inja::Arguments args) -> json {
    return zip_array(*args[0]);
  });

  env.add_callback("zip_obj", -1, [](inja::Arguments args) -> json {
    if (args.size() < 2) return json::array();
    const json &keys = *args[0];
    std::vector<json> arrays;
    arrays.reserve(args.size() - 1);
    for (size_t i = 1; i < args.size(); ++i) {
      arrays.push_back(*args[i]);
    }
    return zip_obj(keys, arrays);
  });
}

} // namespace custom_array