#ifndef HIERACHIAL_MUTEX_HPP_
#define HIERACHIAL_MUTEX_HPP_

#include <shared_mutex>
#include <vector>

// This is our custom implementation of a 'hierachial mutex' class.
// Its purpose is to prevent deadlocks.
// We have one simple rule: a thread cannot acquire a lock on a mutex
// that is 'higher' than it. In said implementation, we will be using
// the in memory addresses where instances of this class exist.
//
// For example:
//
// If a certain thread A has a lock on an instance of our aptly named 'HierachialMutex'
// that's located, say, at address [0xabcd102c], it can only acquire another lock
// on a mutex instance located at a lower address, like [0xabcd102a], and not [0xabcd102d].
//
// Let us say we have a Driver (James) and Rider (Emma).
// James wishes to cancel a ride due to a flat tyre, (or his wife/gf has called him home due
// to an emorgency :))
// At the same time, Emma wishes to cancel the same ride as she feels she has waited for a long time.
//
// We have the following:
//
// 1. Thread A that handles James' situation by a function like cancel_by_driver(Driver&, Rider&)
//
// 2. Thread B that handles Emma's situation by a function like cancel_by_rider(Rider&, Driver&)
//
// Now, consider the following:
//
// Thread A acquires a lock on Driver, and thread B acquires a lock on Rider.
// Thread A will wait for thread B to release the lock on Rider so that it can complete its task,
// (cancelling the ride from the Driver's end) while thread B waits on thread A to release the
// lock on Driver so that, it too, can complete its task (cancelling our hypothetical ride 
// from the Rider's end).
//
// But neither of the aforementioned threads can continue, as neither will release the locks
// they hold! This is a classic example of a deadlock! Both threads end up waiting forever!
//
// Our 'HiearachialMutex' class helps us avoid this.
//
// Both threads will be forced to acquire a lock on the entity (Rider or Driver) instance that
// resides at a higher memory location. For example's sake, let us imagine it is an instance
// of Rider [0xabcd120b]
//
// The OS will sereialize these pararrel operations, such that either thread A or thread B
// first acquires the lock.
// 
// Again, for example's sake, let us assume thread B wins the race. It will lock Rider, 
// causing thread A to be put to sleep by the OS. Thread B will then be free to acquire
// a lock on Driver. Thread B now has an exclusive lock on both Driver and Rider, and
// will proceed on to complete its task (cancelling the ride from the Rider's side)
// After it finishes, thread A will wake up, find both locks have been released, acquire
// both of them, and complete its task.
class HierachialMutex {

    public:
    HierachialMutex() = default;

    // We do not want our mutex to be copied or moved.
    // As such, we 'delete' the copy constructor, and
    // copy assignment operator. Doing so will implicitly
    // 'delete' the move constructor and move assignment
    // operator as well
    HierachialMutex(const HierachialMutex&) = delete;
    HierachialMutex& operator=(const HierachialMutex&) = delete;

    void lock();
    void unlock();
    void lock_shared();
    void unlock_shared();


    private:
    void check_for_violations() const;

    static inline thread_local std::vector<const HierachialMutex*> t_this_acquired_locks {};
    std::shared_mutex m_internal_mutex;
};

#endif
