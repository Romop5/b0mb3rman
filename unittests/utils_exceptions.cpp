#include <catch2/catch_test_macros.hpp>

#include <utils/exceptions.hpp>

TEST_CASE("utils::Exceptions: : basic", "exceptions")
{
  REQUIRE_THROWS(utils::throw_runtime_on_false(false, "expected throw"));
  REQUIRE_NOTHROW(utils::throw_runtime_on_false(true, "expected throw"));
}

TEST_CASE("utils::Exceptions: : not_implemented", "exceptions")
{
  const auto msg = "custom message";
  try {
    throw utils::not_implemented(msg);
  } catch (std::exception& e) {
    REQUIRE(std::string(e.what()).compare(msg) == 0);
  }
}