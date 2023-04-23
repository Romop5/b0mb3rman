#pragma once

#include <array>
#include <numeric>
#include <tuple>
#include <vector>

namespace utils {
template<typename T>
auto
product_op(T a, T b) -> T
{
  return a * b;
}

template<typename Index, typename Dims>
constexpr auto
compute_index(Index index, Dims dims) -> typename Index::value_type
{
  static_assert(index.size() == dims.size());

  typename Index::value_type dim_accumulation = 1;
  typename Index::value_type result = 0;
  for (size_t i = 0; i < index.size(); i++) {
    result += index.at(i) * dim_accumulation;
    dim_accumulation *= dims.at(i);
  }
  return result;
}

namespace detail {
} // utils::detail

/**
 * @brief Vector with _Dims dimensional indexing
 *
 * @tparam _Dims
 * @tparam T
 */
template<unsigned _Dims, typename T = bool>
class OccupancyMap
{
public:
  using type = T;
  using Index = std::array<unsigned, _Dims>;
  using Dims = Index;

  OccupancyMap() = default;

  OccupancyMap(Dims dims, T initial_state = {})
    : dims_{ dims }
    , size_{ get_storage_size() }
    , cells_{ std::vector<T>(size_, initial_state) }
  {
  }

  auto at(Index index) const { return cells_.at(compute_index(index, dims_)); }
  auto at(Index index) { return cells_.at(compute_index(index, dims_)); }

  auto size() const { return size_; }

  auto begin() { return cells_.begin(); }
  auto end() { return cells_.end(); }

private:
  auto get_storage_size() const
  {
    return std::reduce(
      dims_.begin(), dims_.end(), 1, utils::product_op<unsigned>);
  }

  unsigned size_{ 0 };
  Dims dims_;
  std::vector<T> cells_;
};

template<typename... Args>
using OccupancyMap2D = OccupancyMap<2, Args...>;

} // namespace utils