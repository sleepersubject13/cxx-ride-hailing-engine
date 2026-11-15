#ifndef CITY_ZONE_HPP_
#define CITY_ZONE_HPP_

#include <vector>
#include <string_view>
#include <string>

#include "hierachial_mutex.hpp"
#include "driver.hpp"

class CityZone {
    public:
    explicit CityZone(std::string_view);
    bool operator==(const CityZone&) const;

    void add_driver_to_zone(Driver*);
    std::vector<Driver*> get_drivers_in_zone() const;
    std::string_view get_zone_name() const;
    
    friend void transfer_driver_to_zone(CityZone&, CityZone&, Driver*);
  

    private:
    std::string m_zone_name{};
    std::vector<Driver*> m_drivers_in_zone;
    mutable HierachialMutex m_mutex;
};
#endif