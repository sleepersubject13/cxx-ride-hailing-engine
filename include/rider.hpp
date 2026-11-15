#ifndef RIDER_HPP_
#define RIDER_HPP_

#include <string>
#include <string_view>

#include "hierachial_mutex.hpp"
#include "entities.hpp"

class Driver;

class Rider {
    public:
    Rider(std::string_view, std::string_view);

    std::string_view get_id() const;
    std::string_view get_name() const;

    RiderStatus get_rider_status() const;
    Coordinates get_pickup_coordinates() const;

    friend bool try_cancel(Driver&, Rider&);
    friend bool attempt_booking(Rider&, Driver&);
    friend void complete_ride(Driver&, Rider&);

    friend bool verify_driver_invariant(const Driver&);

    private:
    std::string m_id{};
    std::string m_name{};
    RiderStatus m_current_status {RiderStatus::IDLE};
    Driver* m_assigned_to_driver {};
    Coordinates m_pickup_coordinates {};
    mutable HierachialMutex m_mutex;
};

#endif