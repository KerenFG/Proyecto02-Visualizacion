#include <inja/inja.hpp>
#include <nlohmann/json.hpp>
#include <jsoncons/json.hpp>
#include <jsoncons_ext/jmespath/jmespath.hpp>
#include <jsoncons_ext/cbor/cbor.hpp>
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>

using json = nlohmann::json;

struct QueryModule {
    static std::string as_string(const json& j) {
        if (j.is_string()) return j.get_ref<const std::string&>();
        if (j.is_number()) return std::to_string(j.get<double>());
        if (j.is_boolean()) return j.get<bool>() ? "true" : "false";
        return j.dump();
    }

json eval(const json& input, const std::string& expr) {
        try {
            std::vector<uint8_t> cbor_bytes = json::to_cbor(input);
            jsoncons::json jc = jsoncons::cbor::decode_cbor<jsoncons::json>(cbor_bytes);
            jsoncons::json result = jsoncons::jmespath::search(jc, expr);
            std::vector<uint8_t> result_bytes;
            jsoncons::cbor::encode_cbor(result, result_bytes);
            return json::from_cbor(result_bytes);

        } catch (...) {
            return json();
        }
    }
};

void register_query_module(inja::Environment& env) {
    static auto module = std::make_shared<QueryModule>();

    env.add_callback("query", 2, [](inja::Arguments& args) -> json {
        const json& input = *args[0];
        std::string expr = QueryModule::as_string(*args[1]);
        return module->eval(input, expr);
    });
}

#if defined(_WIN32) || defined(_WIN64)
  #define EXPORT extern "C" __declspec(dllexport)
#else
  #define EXPORT extern "C"
#endif

EXPORT void register_module(inja::Environment& env) {
    register_query_module(env);
}