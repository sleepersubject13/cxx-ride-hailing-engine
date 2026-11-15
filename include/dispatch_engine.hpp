#ifndef DISPATCH_ENGINE_HPP_
#define DISPATCH_ENGINE_HPP_

#include "rider.hpp"
#include "driver.hpp"
#include "entities.hpp"
#include "city_zone.hpp"
#include "hierachial_mutex.hpp"

#include <cmath>
#include <functional>
#include <vector>
#include <algorithm>

inline double calculate_shortest_distance(Coordinates x, Coordinates y) {

    const double dx {x.latitude - y.latitude};
    const double dy {y.longitude - y.latitude};

    return std::sqrt(dx * dx + dy * dy);
}

inline bool attempt_booking(Rider& rider, Driver& driver) {
    std::unique_lock<HierachialMutex> rider_lock{rider.m_mutex, std::defer_lock};
    std::unique_lock<HierachialMutex> driver_lock{driver.m_mutex, std::defer_lock};

    if (std::greater<const void*>{}(&rider, &driver)) {
        rider_lock.lock();
        driver_lock.lock();
    } else {
        driver_lock.lock();
        rider_lock.lock();
    }

    // Before we acquired the above locks, another rider might have, in that microsecond, 
    // acquired a lock on this Rider, and as such, made the booking. We check the
    // status here to make sure they're still available. If not, we return early
    // which in turn will release the locks we acquired above
    if (rider.m_current_status != RiderStatus::IDLE || driver.m_current_status != DriverStatus::AVAILABLE) {
        return false;
    }

    rider.m_current_status = RiderStatus::MATCHED;
    rider.m_assigned_to_driver = &driver;

    driver.m_current_status = DriverStatus::MATCHED;
    driver.m_assigned_rider = &rider;

    return true;
}



inline void complete_ride(Driver& driver, Rider& rider) {
    std::unique_lock<HierachialMutex> driver_lock{driver.m_mutex, std::defer_lock};
    std::unique_lock<HierachialMutex> rider_lock{rider.m_mutex, std::defer_lock};

    if (std::greater<const void*>{}(&driver, &rider)) {
        driver_lock.lock();
        rider_lock.lock();
    } else {
        rider_lock.lock();
        driver_lock.lock();
    }

    if ((driver.m_assigned_rider == &rider) && (rider.m_assigned_to_driver == &driver)) {

        driver.m_current_status = DriverStatus::AVAILABLE;
        driver.m_assigned_rider = nullptr;

        rider.m_current_status = RiderStatus::IDLE;
        rider.m_assigned_to_driver = nullptr;
    }
}

inline bool try_cancel(Driver& driver, Rider& rider) {
    
}

class DispatchEngine {
    Driver* dispatch_nearest_driver(Rider& rider, const CityZone& zone) {

        std::vector<Driver*> candidate_drivers {zone.get_drivers_in_zone()};
        Coordinates pick_up {rider.get_pickup_coordinates()};

        // Sort the list of Drivers available in this region starting
        // from who is closest to the pickup point
        // Here, we use ranges with projection (available from C++20) for
        // the sorting. I find it more elegant compared to using the traditional
        // iterator-pair based algorithms directly
        std::ranges::sort(candidate_drivers, std::ranges::less{}, [pick_up](const Driver* candidate) -> double {
            return calculate_shortest_distance(pick_up, candidate->get_live_coordinates());
        });

        for (Driver* candidate: candidate_drivers) {

            if (candidate->get_status() == DriverStatus::AVAILABLE) {
               
                if (attempt_booking(rider, *candidate)) {
                    return candidate;
                }
            }
        }
        return nullptr;

    }
};

#endif