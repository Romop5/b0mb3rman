#include <catch2/catch_test_macros.hpp>

#include <utils/service_locator.hpp>

namespace {
class AbstractA
{
public:
  virtual ~AbstractA() = default;
};

class AbstractB
{
public:
  virtual ~AbstractB() = default;
};

class A : public utils::Service<>
{};

class B : public utils::Service<>
{};

class C : public utils::Service<AbstractA, AbstractB>
{
public:
  explicit C(utils::ServiceLocator& locator)
  {
    REQUIRE(locator.has_interface<AbstractA>());
    REQUIRE(locator.has_interface<AbstractB>());
  }
};
} // namespace

TEST_CASE("utils::Service: : prerequisite: instanciation", "service_locator")
{
  A a{};
  REQUIRE(a.get_depedencies().size() == 0);

  B b{};
  REQUIRE(b.get_depedencies().size() == 0);
}

TEST_CASE("utils::Service: : prerequisite:deps", "service_locator")
{
  REQUIRE(A::get_depedencies().size() == 0);
  REQUIRE(B::get_depedencies().size() == 0);
  REQUIRE(C::DependencyList::as_type_index().size() == 2);
}

TEST_CASE("utils::ServiceLocator: : basic", "service_locator")
{
  utils::ServiceLocator locator;
  locator.register_interface<AbstractA, A>();
  locator.register_interface<AbstractB, B>();
  locator.register_interface<C, C>(locator);

  REQUIRE(locator.has_interface<C>() == false);
  locator.initialize();
  REQUIRE(locator.has_interface<C>() == true);

  locator.get<C>();
}

TEST_CASE("utils::ServiceLocator: : detect circle", "service_locator")
{
  utils::ServiceLocator locator;
  locator.register_interface<AbstractA, A>();
  locator.register_interface<AbstractB, B>();
  locator.register_interface<C, C>(locator);

  locator.get<C>();
}
