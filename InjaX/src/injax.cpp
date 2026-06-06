// SPDX-License-Identifier: MIT
// Project:   InjaX
// File:      src/injax.cpp

#include <filesystem>
#include <fstream>
#include <inja/inja.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>
#include "filters.hpp"
#include "tests.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

#if defined(_WIN32)
#include <windows.h>
const std::string LIB_EXTENSION = ".dll";
using lib_handle_t = HMODULE;
#else
#include <dlfcn.h>
#ifdef __APPLE__
const std::string LIB_EXTENSION = ".dylib";
#else
const std::string LIB_EXTENSION = ".so";
#endif
using lib_handle_t = void*;
#endif

using parse_fn_t = nlohmann::json (*)(const std::string &);

struct DynamicLibrary {
  lib_handle_t handle = nullptr;

  DynamicLibrary(const std::string& path) {
#if defined(_WIN32)
    handle = LoadLibraryA(path.c_str());
    if (!handle) {
      throw std::runtime_error("LoadLibrary error (" + path + "): " + std::to_string(GetLastError()));
    }
#else
    handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle) {
      throw std::runtime_error("dlopen error (" + path + "): " + std::string(dlerror()));
    }
#endif
  }

  ~DynamicLibrary() {
    if (handle) {
#if defined(_WIN32)
      FreeLibrary(handle);
#else
      dlclose(handle);
#endif
    }
  }

  DynamicLibrary(const DynamicLibrary&) = delete;
  DynamicLibrary& operator=(const DynamicLibrary&) = delete;

  template <typename FuncType>
  FuncType get_symbol(const std::string& symbol_name) const {
#if defined(_WIN32)
    auto fn = (FuncType)GetProcAddress(handle, symbol_name.c_str());
#else
    auto fn = (FuncType)dlsym(handle, symbol_name.c_str());
#endif
    if (!fn) {
      throw std::runtime_error("Symbol '" + symbol_name + "' not found");
    }
    return fn;
  }
};

inline fs::path get_executable_directory(const char* argv0) {
  return fs::absolute(fs::path(argv0)).parent_path();
}

std::string read_file(const std::string &filename) {
  std::ifstream file(filename, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file for reading: " + filename);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: " << argv[0] << " <input_data> <template_file> <output_file>" << std::endl;
    return 1;
  }

  const std::string data_file = argv[1];
  const std::string template_file = argv[2];
  const std::string output_file = argv[3];

  std::string extension = fs::path(data_file).extension().string();
  if (!extension.empty() && extension[0] == '.') {
    extension = extension.substr(1);
  }

  std::vector<std::unique_ptr<DynamicLibrary>> loaded_modules;

  inja::Environment env;

  env.set_trim_blocks(true);
  env.set_lstrip_blocks(true);

  custom_tests::register_tests(env);
  register_all_filters(env);

  try {
    const fs::path exe_dir = get_executable_directory(argv[0]);
    const fs::path modules_path = exe_dir / "modules";

    std::cout << "Searching plugins from: " << modules_path << std::endl;

    if (fs::exists(modules_path) && fs::is_directory(modules_path)) {
      for (const auto &entry : fs::directory_iterator(modules_path)) {
        std::string filename = entry.path().filename().string();
        std::string ext = entry.path().extension().string();

        if (ext == LIB_EXTENSION && filename.rfind("lib", 0) == 0) {
          try {
            auto mod = std::make_unique<DynamicLibrary>(entry.path().string());
            auto register_fn = mod->get_symbol<void (*)(inja::Environment &)>("register_module");

            register_fn(env);
            loaded_modules.push_back(std::move(mod));
            std::cout << "Successfully registered extension: " << filename << std::endl;
          } catch (const std::exception &e) {
            std::cerr << "Warning -> Skipping module " << filename << ": " << e.what() << std::endl;
          }
        }
      }
    }

    json input_dataset;
    if (extension == "json") {
      std::ifstream infile(data_file);
      if (!infile.is_open()) {
        throw std::runtime_error("Target dataset file does not exist: " + data_file);
      }
      infile >> input_dataset;
    } else {
      std::string lib_name = "readers/lib" + extension + LIB_EXTENSION;
      fs::path parser_path = exe_dir / lib_name;

      std::cout << "Loading dynamic external format parser: " << lib_name << std::endl;
      DynamicLibrary data_parser_lib(parser_path.string());
      auto parser = data_parser_lib.get_symbol<parse_fn_t>("parse_data");

      input_dataset = parser(data_file);
    }

    std::string template_content = read_file(template_file);

    std::ofstream output(output_file, std::ios::out | std::ios::trunc);
    if (!output.is_open()) {
      throw std::runtime_error("Target output file location is write-protected: " + output_file);
    }

    env.render_to(output, template_content, input_dataset);
    std::cout << "Render sequence completed successfully." << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Fatal Core Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}