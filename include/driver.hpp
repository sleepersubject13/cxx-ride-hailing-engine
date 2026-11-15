#ifndef DRIVER_HPP_
#define DRIVER_HPP_

#include <string_view>
#include <mutex>
#include <string>

#include "entities.hpp"
#include "rider.hpp"
#include "hierachial_mutex.hpp"

class Rider;

class Driver {

    public:
    Driver(std::string_view, std::string_view);

    void update_live_coordinates(Coordinates);
    Coordinates get_live_coordinates() const;
    std::string_view get_id() const;
    std::string_view get_name() const;
    DriverStatus get_status() const;

    friend bool try_cancel(Driver&, Rider&);
    friend bool attempt_booking(Rider&, Driver&);
    friend void complete_ride(Driver&, Rider&);
    

    private:
    std::string m_id {};
    std::string m_name{};
    DriverStatus m_current_status {DriverStatus::AVAILABLE};
    Coordinates m_live_coordinates{};
    Rider* m_assigned_rider {};
    mutable HierachialMutex m_mutex;
};

#endif