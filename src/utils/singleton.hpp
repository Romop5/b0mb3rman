#include <memory>
#include <mutex>

namespace utils {

/**
 * @brief A singleton pattern
 *
 * @tparam T must be default-constructible
 */
template<typename T>
class Singleton
{
public:
  static auto get_instance() -> T&
  {
    static std::mutex mutex;
    static std::unique_ptr<T> instance{ nullptr };
    std::unique_lock<std::mutex> lock{ mutex };
    if (!instance) {
      instance = std::unique_ptr<T>(new T());
    }
    return *instance;
  }

protected:
  Singleton() = default;
};

} // namespace utils