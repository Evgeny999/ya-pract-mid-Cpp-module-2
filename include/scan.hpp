#pragma once

#include <concepts>

#include "parse.hpp"
#include "types.hpp"

namespace stdx {

template <typename T>
concept one_of = (std::same_as<std::remove_cv_t<T>, int8_t> ||
                  std::same_as<std::remove_cv_t<T>, int16_t> ||
                  std::same_as<std::remove_cv_t<T>, int32_t> ||
                  std::same_as<std::remove_cv_t<T>, int64_t> ||
                  std::same_as<std::remove_cv_t<T>, uint8_t> ||
                  std::same_as<std::remove_cv_t<T>, uint16_t> ||
                  std::same_as<std::remove_cv_t<T>, uint32_t> ||
                  std::same_as<std::remove_cv_t<T>, uint64_t> ||
                  std::same_as<std::remove_cv_t<T>, float> ||
                  std::same_as<std::remove_cv_t<T>, double> ||
                  std::same_as<std::remove_cv_t<T>, std::string_view> ||
                  std::same_as<std::remove_cv_t<T>, std::string>);

template <typename... Ts>
  requires(one_of<Ts> && ...)
std::expected<details::scan_result<Ts...>, details::scan_error>
scan(std::string_view input, std::string_view format) {
  auto sources_vectors = details::parse_sources(input, format);
  if (!sources_vectors) {
    return std::unexpected(sources_vectors.error());
  }

  auto format_values = sources_vectors.value().first;
  auto input_values = sources_vectors.value().second;

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

  auto parse_values = [&]<size_t... Ints>(std::index_sequence<Ints...>)
      -> std::expected<details::scan_result<Ts...>, details::scan_error> {
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

      return std::unexpected(details::scan_error{std::move(error_text)});
    }
    return std::expected<details::scan_result<Ts...>, details::scan_error>(
        details::scan_result<Ts...>{
            std::tuple{std::get<Ints>(parsed_values).value()...}});
  };

  return parse_values(std::make_index_sequence<sizeof...(Ts)>{});
}

} // namespace stdx
