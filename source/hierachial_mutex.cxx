#include "hierachial_mutex.hpp"

#include <stdexcept>
#include <functional>

void HierachialMutex::check_for_violations() const {
    // If the current thread doesn't have any locks, obviously
    // it can acquire a lock
    if (t_this_acquired_locks.empty()) {
        return;
    }
    // We get the most recently acquired lock from our lock stack:
    const HierachialMutex* last_acquired {t_this_acquired_locks.back()};

    if (std::greater_equal<>{}(this, last_acquired)) {
        throw std::logic_error{""};
    }
}

void HierachialMutex::lock() {
    check_for_violations();
    m_internal_mutex.lock();
    t_this_acquired_locks.push_back(this);
}

void HierachialMutex::unlock() {
    if (!t_this_acquired_locks.empty()) {
        t_this_acquired_locks.pop_back();
    }
    m_internal_mutex.unlock();
}

void HierachialMutex::lock_shared() {
    check_for_violations();
    m_internal_mutex.lock_shared();
    t_this_acquired_locks.push_back(this);
}

void HierachialMutex::unlock_shared() {
    if (!t_this_acquired_locks.empty()) {
        t_this_acquired_locks.pop_back();
    }

    m_internal_mutex.unlock_shared();
}