#include <figcone_toml/datetime.h>
#include <toml.hpp>
#include <chrono>
#include <sstream>

namespace figcone{

std::optional<toml::DateTimePoint> StringConverter<toml::DateTimePoint>::fromString(const std::string& data)
{
    auto dateStr = "date = " + data;
    auto stream = std::stringstream{dateStr};
    const auto tomlData = toml::parse(stream);
    const auto date = toml::get<std::chrono::system_clock::time_point>(tomlData.at("date"));
    return toml::DateTimePoint{date};
}

std::optional<toml::TimeDuration> StringConverter<toml::TimeDuration>::fromString(const std::string& data)
{
    auto timeStr = "time = " + data;
    auto stream = std::stringstream{timeStr};
    const auto tomlData = toml::parse(stream);
    const auto time = toml::get<std::chrono::seconds>(tomlData.at("time"));
    return toml::TimeDuration{time};
}

}
