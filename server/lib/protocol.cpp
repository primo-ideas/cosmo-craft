#include <nlohmann/json.hpp>

#include "cosmo.hpp"


namespace cosmo {

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ClientAuth, nickname)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AuthResponse, result, message)

template <class T> std::expected<T, SerdeError> deserialize(std::string const &json_str) {
    auto parse_result = nlohmann::json::parse(json_str, nullptr, false);
    if (parse_result.is_discarded()) {
        return std::unexpected(SerdeError::InvalidJson);
    }
    try {
        auto value = parse_result.get<T>();
        return value;
    } catch (...) {
        return std::unexpected(SerdeError::InvalidInput);
    }
}

template <class T> std::string serialize(T const &value) {
    nlohmann::json json = value;
    return json.dump();
}

template std::expected<ClientAuth, SerdeError> deserialize<ClientAuth>(std::string const &json_str);
template std::expected<AuthResponse, SerdeError>
deserialize<AuthResponse>(std::string const &json_str);
template std::string serialize<ClientAuth>(ClientAuth const &value);
template std::string serialize<AuthResponse>(AuthResponse const &value);

} // namespace cosmo
