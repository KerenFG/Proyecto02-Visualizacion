// chart-transform.hpp
#ifndef CHART_TRANSFORM_HPP
#define CHART_TRANSFORM_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <inja/inja.hpp>

using json = nlohmann::json;

inline json stack(const json& data, const std::vector<std::string>& keys) {
  if (!data.is_array()) {
    throw std::runtime_error("stack: data must be an array");
  }
  if (data.empty()) {
    return json::array();
  }
  if (keys.empty()) {
    throw std::runtime_error("stack: keys array cannot be empty");
  }
  
  for (const auto& row : data) {
    if (!row.is_object()) {
      throw std::runtime_error("stack: each data element must be an object");
    }
    for (const auto& key : keys) {
      if (!row.contains(key)) {
        throw std::runtime_error("stack: missing key '" + key + "' in data row");
      }
      if (!row[key].is_number()) {
        throw std::runtime_error("stack: key '" + key + "' must be numeric");
      }
    }
  }
  
  size_t numRows = data.size();
  std::vector<double> accumulators(numRows, 0.0);
  json result = json::array();
  
  for (const auto& key : keys) {
    json series;
    series["key"] = key;
    series["index"] = result.size();
    json seriesData = json::array();
    
    for (size_t i = 0; i < numRows; ++i) {
      double value = data[i][key].get<double>();
      double y0 = accumulators[i];
      double y1 = y0 + value;
      
      json point = json::array({y0, y1});
      point["data"] = data[i];
      point["value"] = value;
      point["index"] = i;
      
      seriesData.push_back(point);
      accumulators[i] = y1;
    }
    
    series["data"] = seriesData;
    result.push_back(series);
  }
  
  return result;
}

inline std::vector<std::string> stackKeys(const json& data, const std::string& excludeKey = "") {
  std::vector<std::string> keys;
  
  if (!data.is_array()) {
    throw std::runtime_error("stackKeys: data must be an array");
  }
  if (data.empty()) {
    return keys;
  }
  
  const auto& firstRow = data[0];
  if (!firstRow.is_object()) {
    throw std::runtime_error("stackKeys: each data element must be an object");
  }
  
  for (auto it = firstRow.begin(); it != firstRow.end(); ++it) {
    const std::string& key = it.key();
    if (key != excludeKey && it.value().is_number()) {
      keys.push_back(key);
    }
  }
  
  return keys;
}

inline json stackWithKeys(const json& data, const json& keysJson) {
  std::vector<std::string> keys;
  
  if (keysJson.is_array()) {
    for (const auto& k : keysJson) {
      if (!k.is_string()) {
        throw std::runtime_error("stackWithKeys: all keys must be strings");
      }
      keys.push_back(k.get<std::string>());
    }
  } else if (keysJson.is_string()) {
    keys = stackKeys(data, keysJson.get<std::string>());
  } else {
    throw std::runtime_error("stackWithKeys: keys must be an array or a string");
  }
  
  return stack(data, keys);
}

inline json stackOrderNone(const json& data, const std::vector<std::string>& keys) {
  return stack(data, keys);
}

inline json stackOrderReverse(const json& data, const std::vector<std::string>& keys) {
  std::vector<std::string> reversedKeys = keys;
  std::reverse(reversedKeys.begin(), reversedKeys.end());
  return stack(data, reversedKeys);
}

inline json stackOrderAscending(const json& data, const std::vector<std::string>& keys) {
  if (!data.is_array()) {
    throw std::runtime_error("stackOrderAscending: data must be an array");
  }
  
  std::vector<double> sums(keys.size(), 0.0);
  for (size_t i = 0; i < keys.size(); ++i) {
    double sum = 0.0;
    for (const auto& row : data) {
      if (row.contains(keys[i]) && row[keys[i]].is_number()) {
        sum += row[keys[i]].get<double>();
      }
    }
    sums[i] = sum;
  }
  
  std::vector<size_t> indices(keys.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(), 
            [&sums](size_t a, size_t b) { return sums[a] < sums[b]; });
  
  std::vector<std::string> orderedKeys(keys.size());
  for (size_t i = 0; i < keys.size(); ++i) {
    orderedKeys[i] = keys[indices[i]];
  }
  
  return stack(data, orderedKeys);
}

inline json stackOrderDescending(const json& data, const std::vector<std::string>& keys) {
  if (!data.is_array()) {
    throw std::runtime_error("stackOrderDescending: data must be an array");
  }
  
  std::vector<double> sums(keys.size(), 0.0);
  for (size_t i = 0; i < keys.size(); ++i) {
    double sum = 0.0;
    for (const auto& row : data) {
      if (row.contains(keys[i]) && row[keys[i]].is_number()) {
        sum += row[keys[i]].get<double>();
      }
    }
    sums[i] = sum;
  }
  
  std::vector<size_t> indices(keys.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(), 
            [&sums](size_t a, size_t b) { return sums[a] > sums[b]; });
  
  std::vector<std::string> orderedKeys(keys.size());
  for (size_t i = 0; i < keys.size(); ++i) {
    orderedKeys[i] = keys[indices[i]];
  }
  
  return stack(data, orderedKeys);
}

inline json stackOrder(const json& data, const std::vector<std::string>& keys, const std::string& order) {
  if (order == "reverse") {
    return stackOrderReverse(data, keys);
  } else if (order == "ascending") {
    return stackOrderAscending(data, keys);
  } else if (order == "descending") {
    return stackOrderDescending(data, keys);
  } else if (order == "none" || order.empty()) {
    return stackOrderNone(data, keys);
  } else {
    throw std::runtime_error("stackOrder: unknown order '" + order + "'. Use: none, reverse, ascending, descending");
  }
}

inline json stackOffsetNone(const json& data, const std::vector<std::string>& keys) {
  return stack(data, keys);
}

inline json stackOffsetExpand(const json& data, const std::vector<std::string>& keys) {
  json stacked = stack(data, keys);
  
  if (!stacked.is_array() || stacked.empty()) {
    return stacked;
  }
  
  size_t numRows = stacked[0]["data"].size();
  std::vector<double> rowTotals(numRows, 0.0);
  
  for (const auto& series : stacked) {
    const auto& seriesData = series["data"];
    for (size_t i = 0; i < seriesData.size(); ++i) {
      if (seriesData[i].is_array() && seriesData[i].size() >= 2) {
        rowTotals[i] += seriesData[i][1].get<double>();
      }
    }
  }
  
  for (auto& series : stacked) {
    auto& seriesData = series["data"];
    for (size_t i = 0; i < seriesData.size(); ++i) {
      if (rowTotals[i] > 1e-9 && seriesData[i].is_array() && seriesData[i].size() >= 2) {
        double y0 = seriesData[i][0].get<double>() / rowTotals[i];
        double y1 = seriesData[i][1].get<double>() / rowTotals[i];
        seriesData[i][0] = y0;
        seriesData[i][1] = y1;
        seriesData[i]["normalized"] = true;
      }
    }
  }
  
  return stacked;
}

inline json stackOffsetDiverging(const json& data, const std::vector<std::string>& keys) {
  json stacked = stack(data, keys);
  
  if (!stacked.is_array() || stacked.empty()) {
    return stacked;
  }
  
  size_t numRows = stacked[0]["data"].size();
  std::vector<double> rowPosSum(numRows, 0.0);
  std::vector<double> rowNegSum(numRows, 0.0);
  
  for (const auto& series : stacked) {
    const auto& seriesData = series["data"];
    for (size_t i = 0; i < seriesData.size(); ++i) {
      if (seriesData[i].is_array() && seriesData[i].size() >= 2) {
        double y0 = seriesData[i][0].get<double>();
        double y1 = seriesData[i][1].get<double>();
        double value = y1 - y0;
        if (value >= 0) {
          rowPosSum[i] += value;
        } else {
          rowNegSum[i] += value;
        }
      }
    }
  }
  
  for (auto& series : stacked) {
    auto& seriesData = series["data"];
    for (size_t i = 0; i < seriesData.size(); ++i) {
      if (seriesData[i].is_array() && seriesData[i].size() >= 2) {
        double y0 = seriesData[i][0].get<double>();
        double y1 = seriesData[i][1].get<double>();
        double value = y1 - y0;
        
        if (value >= 0) {
          double newY0 = y0 - rowNegSum[i];
          double newY1 = newY0 + value;
          seriesData[i][0] = newY0;
          seriesData[i][1] = newY1;
        } else {
          double newY1 = y1 + rowPosSum[i];
          double newY0 = newY1 + value;
          seriesData[i][0] = newY0;
          seriesData[i][1] = newY1;
        }
        seriesData[i]["diverging"] = true;
      }
    }
  }
  
  return stacked;
}

inline json stackOffsetWiggle(const json& data, const std::vector<std::string>& keys) {
  json stacked = stack(data, keys);
  
  if (!stacked.is_array() || stacked.empty()) {
    return stacked;
  }
  
  size_t numSeries = stacked.size();
  size_t numRows = stacked[0]["data"].size();
  
  std::vector<std::vector<double>> seriesValues(numSeries, std::vector<double>(numRows, 0.0));
  
  for (size_t s = 0; s < numSeries; ++s) {
    const auto& seriesData = stacked[s]["data"];
    for (size_t i = 0; i < numRows; ++i) {
      if (seriesData[i].is_array() && seriesData[i].size() >= 2) {
        seriesValues[s][i] = seriesData[i][1].get<double>() - seriesData[i][0].get<double>();
      }
    }
  }
  
  std::vector<double> offsets(numRows, 0.0);
  std::vector<double> cumSum(numRows, 0.0);
  
  for (size_t i = 0; i < numRows; ++i) {
    double total = 0.0;
    for (size_t s = 0; s < numSeries; ++s) {
      total += seriesValues[s][i];
    }
    if (i > 0) {
      offsets[i] = -cumSum[i - 1] / 2.0;
    }
    cumSum[i] = total;
  }
  
  for (size_t s = 0; s < numSeries; ++s) {
    auto& seriesData = stacked[s]["data"];
    for (size_t i = 0; i < numRows; ++i) {
      if (seriesData[i].is_array() && seriesData[i].size() >= 2) {
        double y0 = seriesData[i][0].get<double>() + offsets[i];
        double y1 = seriesData[i][1].get<double>() + offsets[i];
        seriesData[i][0] = y0;
        seriesData[i][1] = y1;
      }
    }
  }
  
  return stacked;
}

inline json stackOffsetSilhouette(const json& data, const std::vector<std::string>& keys) {
  json stacked = stack(data, keys);
  
  if (!stacked.is_array() || stacked.empty()) {
    return stacked;
  }
  
  size_t numRows = stacked[0]["data"].size();
  std::vector<double> rowTotals(numRows, 0.0);
  
  for (const auto& series : stacked) {
    const auto& seriesData = series["data"];
    for (size_t i = 0; i < seriesData.size(); ++i) {
      if (seriesData[i].is_array() && seriesData[i].size() >= 2) {
        rowTotals[i] += seriesData[i][1].get<double>() - seriesData[i][0].get<double>();
      }
    }
  }
  
  for (auto& series : stacked) {
    auto& seriesData = series["data"];
    for (size_t i = 0; i < seriesData.size(); ++i) {
      if (seriesData[i].is_array() && seriesData[i].size() >= 2 && rowTotals[i] > 0) {
        double offset = -rowTotals[i] / 2.0;
        double y0 = seriesData[i][0].get<double>() + offset;
        double y1 = seriesData[i][1].get<double>() + offset;
        seriesData[i][0] = y0;
        seriesData[i][1] = y1;
        seriesData[i]["silhouette"] = true;
      }
    }
  }
  
  return stacked;
}

inline json stackOffset(const json& data, const std::vector<std::string>& keys, const std::string& offset) {
  if (offset == "expand") {
    return stackOffsetExpand(data, keys);
  } else if (offset == "diverging") {
    return stackOffsetDiverging(data, keys);
  } else if (offset == "wiggle") {
    return stackOffsetWiggle(data, keys);
  } else if (offset == "silhouette") {
    return stackOffsetSilhouette(data, keys);
  } else if (offset == "none" || offset.empty()) {
    return stackOffsetNone(data, keys);
  } else {
    throw std::runtime_error("stackOffset: unknown offset '" + offset + "'. Use: none, expand, diverging, wiggle, silhouette");
  }
}

inline void register_chart_transform_functions(inja::Environment& env) {
  env.add_callback("stack", 2, [](inja::Arguments& args) -> json {
    const json& data = *args[0];
    const json& keysJson = *args[1];
    return stackWithKeys(data, keysJson);
  });
  
  env.add_callback("stackKeys", 1, [](inja::Arguments& args) -> json {
    const json& data = *args[0];
    std::vector<std::string> keys = stackKeys(data);
    json result = json::array();
    for (const auto& key : keys) {
      result.push_back(key);
    }
    return result;
  });
  
  env.add_callback("stackKeysExclude", 2, [](inja::Arguments& args) -> json {
    const json& data = *args[0];
    std::string exclude = args[1]->get<std::string>();
    std::vector<std::string> keys = stackKeys(data, exclude);
    json result = json::array();
    for (const auto& key : keys) {
      result.push_back(key);
    }
    return result;
  });
  
  env.add_callback("stackOrder", 3, [](inja::Arguments& args) -> json {
    const json& data = *args[0];
    const json& keysJson = *args[1];
    std::string order = args[2]->get<std::string>();
    
    std::vector<std::string> keys;
    if (keysJson.is_array()) {
      for (const auto& k : keysJson) {
        if (!k.is_string()) {
          throw std::runtime_error("stackOrder: all keys must be strings");
        }
        keys.push_back(k.get<std::string>());
      }
    } else if (keysJson.is_string()) {
      keys = stackKeys(data, keysJson.get<std::string>());
    } else {
      throw std::runtime_error("stackOrder: keys must be an array or a string");
    }
    
    return stackOrder(data, keys, order);
  });
  
  env.add_callback("stackOffset", 3, [](inja::Arguments& args) -> json {
    const json& data = *args[0];
    const json& keysJson = *args[1];
    std::string offset = args[2]->get<std::string>();
    
    std::vector<std::string> keys;
    if (keysJson.is_array()) {
      for (const auto& k : keysJson) {
        if (!k.is_string()) {
          throw std::runtime_error("stackOffset: all keys must be strings");
        }
        keys.push_back(k.get<std::string>());
      }
    } else if (keysJson.is_string()) {
      keys = stackKeys(data, keysJson.get<std::string>());
    } else {
      throw std::runtime_error("stackOffset: keys must be an array or a string");
    }
    
    return stackOffset(data, keys, offset);
  });
  
  env.add_callback("stackValue", 3, [](inja::Arguments& args) -> json {
    const json& data = *args[0];
    const json& keysJson = *args[1];
    std::string valueField = args[2]->get<std::string>();
    
    std::vector<std::string> keys;
    if (keysJson.is_array()) {
      for (const auto& k : keysJson) {
        if (!k.is_string()) {
          throw std::runtime_error("stackValue: all keys must be strings");
        }
        keys.push_back(k.get<std::string>());
      }
    } else if (keysJson.is_string()) {
      keys = stackKeys(data, keysJson.get<std::string>());
    } else {
      throw std::runtime_error("stackValue: keys must be an array or a string");
    }
    
    json result = stack(data, keys);
    
    for (auto& series : result) {
      auto& seriesData = series["data"];
      for (auto& point : seriesData) {
        if (point.is_object() && point.contains("data")) {
          const auto& row = point["data"];
          if (row.contains(valueField) && row[valueField].is_number()) {
            point["value"] = row[valueField];
          }
        }
      }
    }
    
    return result;
  });
}

#endif