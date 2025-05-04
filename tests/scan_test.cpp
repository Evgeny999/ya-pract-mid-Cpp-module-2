#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
  auto result = stdx::scan<std::string, int>("number my new", "{} my {}");
  ASSERT_FALSE(result);
}