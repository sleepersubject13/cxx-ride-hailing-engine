#ifndef DISPATCH_ENGINE_HPP_
#define DISPATCH_ENGINE_HPP_

#include "rider.hpp"
#include "driver.hpp"

#include <cmath>

inline double calculate_shortest_distance(Coordinates x, Coordinates y) {

    const double dx {x.latitude - y.latitude};
    const double dy {y.longitude - y.latitude};

    return std::sqrt(dx * dx + dy * dy);
}

inline bool try_cancel(Driver& driver, Rider& rider) {

}

#endif