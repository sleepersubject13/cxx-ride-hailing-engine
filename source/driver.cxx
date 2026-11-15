#include "driver.hpp"
#include "rider.hpp"


Driver::Driver(std::string_view id, std::string_view name, Coordinates live_coordinates): m_id {id}, m_name{name}, m_live_coordinates{live_coordinates} {}

void Driver::update_live_coordinates(Coordinates new_coordinates) {
     std::unique_lock<HierachialMutex> lock {m_mutex};
     m_live_coordinates = new_coordinates;
}

std::string_view Driver::get_id() const {
    std::shared_lock<HierachialMutex> s_lock{m_mutex};
    return m_id;
}

std::string_view Driver::get_name() const {
    std::shared_lock<HierachialMutex> s_lock{m_mutex};
    return m_name;
}

Coordinates Driver::get_live_coordinates() const {
    std::shared_lock<HierachialMutex> s_lock{m_mutex};
    return m_live_coordinates;
}

DriverStatus Driver::get_status() const {
    std::shared_lock<HierachialMutex> lock{m_mutex};
    return m_current_status;
}