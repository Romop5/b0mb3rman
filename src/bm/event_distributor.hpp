#pragma once

#include <chrono>
#include <queue>
#include <typeindex>
#include <variant>
#include <vector>

#include <bm/entity.hpp>
#include <utils/extended_priority_queue.hpp>

namespace bm {

/**
 * @brief Distributes (timed) events to registered handlers
 *
 * EventDistributor is a combination of discrete-time event planning with strong
 * typing and dynamically-registered event listeners.
 *
 * This event distributor has a few limitations:
 * - thread safety is ignored
 * - only static listener topology (after registration)
 * - indirections of event data due to type erasure
 * - deadlock if events are recurrently planned without stopping condition
 */
class EventDistributor
{
public:
  using Clock = std::chrono::high_resolution_clock;
  using Timestamp = std::chrono::time_point<Clock>;

public:
  EventDistributor() = default;

  /**
   * @brief Plan `E` event for `planned_time`
   *
   * @tparam E
   * @param event
   * @param planned_time
   */
  template<typename E>
  auto enqueue_event(E event, Timestamp planned_time = Clock::now()) -> void
  {
    event_queue_.emplace(Record{
      std::make_optional<EventModel<E>>(event), Clock::now(), planned_time });
  }

  /// @brief Process all ready events
  auto dispatch() -> void
  {
    const auto now = Clock::now();
    while (!event_queue_.empty() and event_queue_.top().planned_time_ <= now) {
      // Fetch top and pop (to allow enqueuing while processing current event)
      const auto record = event_queue_.top_and_pop();

      const auto& type_index = record.event_->type_;
      if (listeners_.count(type_index) == 0) {
        // TODO: trace ingored event in log
        continue;
      }
      for (const auto& listener : listeners_.at(type_index)) {
        listener(record);
      }
    }
  }

  template<typename E, typename T>
  auto registry_listener(T& listener)
  {
    auto& listeners = listeners_[typeid(E)];
    listeners.emplace_back([&listener](const Record& record) {
      // Note: type equivalence is verified in compile-time, therefore we
      // don't use dynamic cast here
      const auto& event = static_cast<EventModel<E>&>(*record.event_);
      listener.handle(event.data_);
    });
  }

private:
  struct EventConcept
  {
    virtual ~EventConcept() = default;
    std::type_index type_;
  };

  template<typename T>
  struct EventModel : public EventConcept
  {
    explicit EventModel(T data)
      : EventConcept{ std::type_index(typeid(type_)) }
      , data_{ data }
    {
    }
    T data_;
  };

  /**
   * @brief Record for discrete-time simulation planning
   *
   */
  struct Record
  {
    Record() = default;
    Record(const Record&) = delete;
    Record(Record&& other)
    {
      std::swap(event_, other.event_);
      std::swap(enqued_time_, other.enqued_time_);
      std::swap(planned_time_, other.planned_time_);
    }

    Record& operator=(Record&& other)
    {
      std::swap(event_, other.event_);
      std::swap(enqued_time_, other.enqued_time_);
      std::swap(planned_time_, other.planned_time_);
      return *this;
    }

    /// @brief Event data
    std::unique_ptr<EventConcept> event_;
    /// @brief Time of insertion to queue
    Timestamp enqued_time_;
    /// @brief Planned time of execution
    Timestamp planned_time_;

    struct ScheduleSorter
    {
      auto operator()(const Record& a, const Record& b) -> bool
      {
        return a.planned_time_ < b.planned_time_;
      }
    };
  };

  utils::
    extended_priority_queue<Record, std::vector<Record>, Record::ScheduleSorter>
      event_queue_;

  std::unordered_map<std::type_index,
                     std::vector<std::function<void(const Record&)>>>
    listeners_;
};

} // namespace bm