#include "rider.hpp"

Rider::Rider(std::string_view id, std::string_view name, Coordinates pickup_coordinates) : m_id {id}, m_name{name}, m_pickup_coordinates{pickup_coordinates} {}

std::string_view Rider::get_id() const {
    std::shared_lock<HierachialMutex> lock{m_mutex};
    return m_id;
}

std::string_view Rider::get_name() const {
    std::shared_lock<HierachialMutex> lock{m_mutex};
    return m_name;
}

RiderStatus Rider::get_rider_status() const {
    std::shared_lock<HierachialMutex> lock{m_mutex};
    return m_current_status;
}

Coordinates Rider::get_pickup_coordinates() const {
    std::shared_lock<HierachialMutex> lock{m_mutex};
    return m_pickup_coordinates;
}