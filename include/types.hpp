#pragma once

namespace stdx::details {

// Класс для хранения ошибки неуспешного сканирования

struct scan_error {
  std::string message;
};

// Шаблонный класс для хранения результатов успешного сканирования

template <typename... Ts> struct scan_result {
public:
  scan_result(std::tuple<Ts...> &&scan_values) noexcept
      : scan_values_(std::exchange(scan_values, {})) {}

  std::tuple<Ts...> values() { return scan_values_; }

private:
  std::tuple<Ts...> scan_values_;
};

} // namespace stdx::details
