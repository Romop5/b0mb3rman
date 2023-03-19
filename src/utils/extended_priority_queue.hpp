#pragma once

#include <algorithm>
#include <queue>

namespace utils {
/**
 * @brief std::priority_queue with move-able elements
 *
 * @note credits: https://stackoverflow.com/a/51368807
 * @tparam T
 * @tparam Container
 * @tparam std::less<typename Container::value_type>
 */
template<class T,
         class Container = std::vector<T>,
         class Compare = std::less<typename Container::value_type>>
class extended_priority_queue
  : public std::priority_queue<T, Container, Compare>
{
public:
  T top_and_pop()
  {
    std::pop_heap(c.begin(), c.end(), comp);
    T value = std::move(c.back());
    c.pop_back();
    return value;
  }

protected:
  using std::priority_queue<T, Container, Compare>::c;
  using std::priority_queue<T, Container, Compare>::comp;
};

} // namespace utils