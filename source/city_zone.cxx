#include "city_zone.hpp"

#include <functional>
#include <vector>
#include <ranges>

CityZone::CityZone(std::string_view zone_name): m_zone_name {zone_name} {}

void CityZone::add_driver_to_zone(Driver* driver) {
    std::unique_lock<HierachialMutex> lock{m_mutex};
    m_drivers_in_zone.push_back(driver);
}

std::vector<Driver*> CityZone::get_drivers_in_zone() const {
    std::shared_lock<HierachialMutex> lock{m_mutex};
    return m_drivers_in_zone;
}

std::string_view CityZone::get_zone_name() const {
    std::shared_lock<HierachialMutex> lock{m_mutex};
    return m_zone_name;
}

bool CityZone::operator==(const CityZone& rhs) const {

    return this->m_zone_name == rhs.m_zone_name;

}

void transfer_driver_to_zone(CityZone& from, CityZone& to, Driver* driver) {
    if (from == to || driver == nullptr) {
        return;
    }

    std::unique_lock<HierachialMutex> from_lock{from.m_mutex, std::defer_lock};
    std::unique_lock<HierachialMutex> to_lock{to.m_mutex, std::defer_lock};

    if (std::greater<>{}(&from, &to)) {
        from_lock.lock();
        to_lock.lock();
    } else {
        to_lock.lock();
        from_lock.lock();
    }

    const std::vector<Driver*>::iterator found_driver {std::ranges::find(from.m_drivers_in_zone, driver)};

    // If we found the driver in the zone
    if (found_driver != std::end(from.m_drivers_in_zone)) {
        from.m_drivers_in_zone.erase(found_driver);
        to.m_drivers_in_zone.push_back(driver);
    } 

}
