#pragma once

#include <expected>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "types.hpp"

namespace stdx::details {

template <typename T> T parse_value(int input_value) {
  return static_cast<T>(input_value);
}

template <typename T> T parse_value(std::string &input_value) {
  T t;
  return t;
}

template <typename T> T parse_value(unsigned long input_value) {
  return static_cast<T>(input_value);
}

template <typename T> T parse_value(float input_value) {
  return static_cast<T>(input_value);
}

template <typename T>
T parse_value(float input_value)
  requires requires(T &t) { t.size(); }
{
  return static_cast<T>(input_value);
}

// Функция для парсинга значения с учетом спецификатора формата
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input,
                                                     std::string_view fmt) {

  std::cout << "input = " << input << '\n' << "fmt" << fmt << '\n';

  if constexpr (std::is_same<std::string, T>::value) {

  } else {
    if (fmt == "%d") {
      auto input_value = std::stoi(std::string{input});
      auto parsed_value = parse_value<T>(input_value);
      return parsed_value;
    } else if (fmt == "%s") {
      auto input_value = std::string{input};
      auto parsed_value = parse_value<T>(input_value);
      return parsed_value;
    } else if (fmt == "%u") {
      auto input_value = std::stoul(std::string{input});
      auto parsed_value = parse_value<T>(input_value);
      return parsed_value;
    } else if (fmt == "%f") {
      auto input_value = std::stof(std::string{input});
      auto parsed_value = parse_value<T>(input_value);
      return parsed_value;
    }
  }

  return std::unexpected(scan_error{"Not supported format specifier"});
}

// Функция для проверки корректности входных данных и выделения из обеих строк
// интересующих данных для парсинга
template <typename... Ts>
std::expected<
    std::pair<std::vector<std::string_view>, std::vector<std::string_view>>,
    scan_error>
parse_sources(std::string_view input, std::string_view format) {
  std::vector<std::string_view> format_parts; // Части формата между {}
  std::vector<std::string_view> input_parts;
  size_t start = 0;
  while (true) {
    size_t open = format.find('{', start);
    if (open == std::string_view::npos) {
      break;
    }
    size_t close = format.find('}', open);
    if (close == std::string_view::npos) {
      break;
    }

    // Если между предыдущей } и текущей { есть текст,
    // проверяем его наличие во входной строке
    if (open > start) {
      std::string_view between = format.substr(start, open - start);
      auto pos = input.find(between);
      if (input.size() < between.size() || pos == std::string_view::npos) {
        return std::unexpected(scan_error{
            "Unformatted text in input and format string are different"});
      }
      if (start != 0) {
        input_parts.emplace_back(input.substr(0, pos));
      }

      input = input.substr(pos + between.size());
    }

    // Сохраняем спецификатор формата (то, что между {})
    format_parts.push_back(format.substr(open + 1, close - open - 1));
    start = close + 1;
  }

  // Проверяем оставшийся текст после последней }
  if (start < format.size()) {
    std::string_view remaining_format = format.substr(start);
    auto pos = input.find(remaining_format);
    if (input.size() < remaining_format.size() ||
        pos == std::string_view::npos) {
      return std::unexpected(scan_error{
          "Unformatted text in input and format string are different"});
    }
    input_parts.emplace_back(input.substr(0, pos));
    input = input.substr(pos + remaining_format.size());
  } else {
    input_parts.emplace_back(input);
  }
  return std::pair{format_parts, input_parts};
}

} // namespace stdx::details