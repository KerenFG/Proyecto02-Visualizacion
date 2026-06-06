#define PUGIXML_HEADER_ONLY
#include <pugixml/pugixml.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <functional>

#if defined(_WIN32) || defined(_WIN64)
  #define EXPORT __declspec(dllexport)
#else
  #define EXPORT
#endif

using json = nlohmann::json;

EXPORT json parse_data(const std::string& filename) {
    if (filename.empty()) return json::object();

    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file(filename.c_str());
    if (!result) {
        return json::object();
    }

    pugi::xml_node root = doc.document_element();
    if (!root) {
        return json::object();
    }

    std::function<json(const pugi::xml_node&)> convert = [&](const pugi::xml_node& node) -> json {
        json obj = json::object();

        for (const auto& attr : node.attributes()) {
            obj["@" + std::string(attr.name())] = attr.value();
        }

        bool has_children = false;
        for (const auto& child : node.children()) {
            if (child.type() == pugi::node_element) {
                has_children = true;
                obj[child.name()].push_back(convert(child));
            }
        }

        if (!has_children && node.text()) {
            obj["#text"] = node.text().get();
        }

        return obj;
    };

    json result_json;
    result_json[root.name()] = convert(root);
    return result_json;
}