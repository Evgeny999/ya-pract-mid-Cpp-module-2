#include <gtest/gtest.h>
#include <print>

#include "scan.hpp"

TEST(ScanTest, SimpleTest) {
  auto result = stdx::scan<std::string>("number", "{}");
  ASSERT_TRUE(result);
}

TEST(ScanTest, TestWrongFormatSpecifier) {
  auto result = stdx::scan<std::string>("number", "{%k}");
  ASSERT_FALSE(result);
}

TEST(ScanTest, TestBadFormat) {
  auto result = stdx::scan<std::string>("number", "{");
  ASSERT_FALSE(result);
}

TEST(ScanTest, TestTooFewFormat) {
  auto result = stdx::scan<std::string>("number", "{} {}");
  ASSERT_FALSE(result);
}

TEST(ScanTest, TestTextDifferent) {
  auto result = stdx::scan<std::string>("number", "{} error");
  ASSERT_FALSE(result);
}

TEST(ScanTest, TestFloatFloatToIntFloat) {
  auto result = stdx::scan<int, float>("15.5 number 16.2", "{%f} number {%f}");
  if (result) {
    auto result_tuple = result.value().values();
    ASSERT_TRUE((std::get<0>(result_tuple) == 15) &&
                (std::get<1>(result_tuple) == 16.2f));
  } else {
    FAIL();
  }
}

TEST(ScanTest, TestUnsignedToString) {
  UINT_MAX;
  auto result = stdx::scan<std::string>("-15 number", "{%u} number");
  if (result) {
    auto result_tuple = result.value().values();
    // UINT_MAX - 15 + 1
    ASSERT_TRUE((std::get<0>(result_tuple) == "4294967281"));
  } else {
    FAIL();
  }
}

TEST(ScanTest, TestStringToInt) {
  UINT_MAX;
  auto result = stdx::scan<int>("-15 number", "{%s} number");
  if (result) {
    auto result_tuple = result.value().values();
    ASSERT_TRUE((std::get<0>(result_tuple) == -15));
  } else {
    FAIL();
  }
}

TEST(ScanTest, TestStringToFloat) {
  UINT_MAX;
  auto result = stdx::scan<float>("-15.7 number", "{%s} number");
  if (result) {
    auto result_tuple = result.value().values();
    ASSERT_TRUE((std::get<0>(result_tuple) == -15.7f));
  } else {
    FAIL();
  }
}