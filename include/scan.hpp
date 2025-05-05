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

  std::tuple<std::expected<Ts, details::scan_error>...> parsed_values;

  auto parse_values = [&]<size_t... Ints>(std::index_sequence<Ints...>) {
    ((std::get<Ints>(parsed_values) = details::parse_value_with_format<Ts>(
          input_values[Ints], format_values[Ints])),
     ...);

    bool err_flag = (... || !std::get<Ints>(parsed_values).has_value());

    if (err_flag) {
      std::string error_text;
      ((!std::get<Ints>(parsed_values)
            ? (error_text +=
               std::get<Ints>(parsed_values).error().message + "; ")
            : ""),
       ...);

      // return std::unexpected(details::scan_error{std::move(error_text)});
    }

    return std::expected<details::scan_result<Ts...>, details::scan_error>(
        details::scan_result<Ts...>{
            std::tuple{std::get<Ints>(parsed_values).value()...}});
  };

  parse_values(std::make_index_sequence<sizeof...(Ts)>{});
}

} // namespace stdx
