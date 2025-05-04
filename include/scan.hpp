#pragma once

#include "parse.hpp"
#include "types.hpp"

namespace stdx {

template <typename... Ts>
std::expected<details::scan_result<Ts...>, details::scan_error>
scan(std::string_view input, std::string_view format) {
  auto sources_vectors = details::parse_sources(input, format);
  if (!sources_vectors.has_value()) {
    return std::unexpected(sources_vectors.error());
  }

  auto input_values = sources_vectors.value().first;
  auto format_values = sources_vectors.value().second;

  if (input_values.size() != format_values.size()) {
    return std::unexpected(
        details::scan_error{"Incorrect parsing: size of input and format "
                            "values vectors don't match"});
  }

  if (input_values.size() != sizeof...(Ts)) {
    return std::unexpected(
        details::scan_error{"Size of template parameter pack of scan function "
                            "doesn't match parsed parameters vector size"});
  }

  size_t i = 0;

  ((details::parse_value_with_format<Ts>(input_values[i], format_values[i++])),
   ...);
  /*for (size_t i = 0; i < input_values.size(); ++i) {
    auto value_format = details::parse_value_with_format<std::string>(
        input_values[i], format_values[i]);
    if (!value_format.has_value()) {
      return std::unexpected(sources_vectors.error());
    }
  }*/

  return std::unexpected(details::scan_error{"Dumb implementation"});
}

} // namespace stdx
