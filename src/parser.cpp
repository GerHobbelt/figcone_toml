#include <toml.hpp>
#include <figcone_toml/parser.h>
#include <figcone_tree/errors.h>
#include <figcone_tree/tree.h>
#include <sstream>
#include <string>
#include <vector>

namespace figcone::toml {

namespace {

std::string str(const toml::value& val)
{
    if (val.is_string())
        return val.as_string();

    auto stream = std::stringstream{};
    stream << val;
    return stream.str();
}

void parseToml(const toml::value& toml, TreeNode& node)
{
    for (auto& [key, value] : toml.as_table()) {
        if (value.is_table()) {
            auto& newNode = node.asItem().addNode(key);
            parseToml(value, newNode);
        }
        else if (value.is_array()) {
            if (!value.as_array().empty() && value.as_array().front().is_table()) {
                auto& newNode = node.asItem().addNodeList(key);
                ;
                for (auto& item : value.as_array())
                    parseToml(item, newNode.asList().addNode());
            }
            else {
                auto valuesList = std::vector<std::string>{};
                for (auto& item : value.as_array()) {
                    if (item.is_array())
                        throw figcone::ConfigError{"Array '" + key + "': figcone_toml doesn't support nested arrays"};
                    valuesList.emplace_back(str(item));
                }

                node.asItem().addParamList(key, valuesList);
            }
        }
        else
            node.asItem().addParam(key, str(value));
    }
}
} //namespace

TreeNode Parser::parse(std::istream& stream)
{
    const auto toml = [&]
    {
        try {
            return toml::parse(stream);
        }
        catch (const toml::exception& e) {
            throw figcone::ConfigError{e.what(), {e.location().line(), e.location().column()}};
        }
    }();

    auto tree = figcone::makeTreeRoot();
    parseToml(toml, tree);
    return tree;
}

} //namespace figcone::toml