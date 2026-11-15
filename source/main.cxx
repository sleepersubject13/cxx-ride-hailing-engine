#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <random>
#include <atomic>

#include "hierachial_mutex.hpp"
#include "entities.hpp"
#include "city_zone.hpp"
#include "dispatch_engine.hpp"

int main() {
    std::cout << "<<<<<<<<<< Starting Hyper-Threaded Ride-Hailing Engine >>>>>>>>>>\n\n";

    // We create some city neighbourhoods here
    CityZone the_bronx{"The Bronx"};
    CityZone staten_island{"Staten Island"};

    // We spawn some Drivers on the heap using unique_ptr so their RAM addresses never move
    std::vector<std::unique_ptr<Driver>> drivers;
    drivers.push_back(std::make_unique<Driver>("1", "Nandor The Relentless", Coordinates{1.0, 2.0}));
    drivers.push_back(std::make_unique<Driver>("2", "Nadja", Coordinates{3.0, 4.0}));
    drivers.push_back(std::make_unique<Driver>("3", "Lazlo", Coordinates{5.0, 1.0}));
    drivers.push_back(std::make_unique<Driver>("4", "Collin Robinson", Coordinates{2.0, 6.0}));

    // Assign drivers to initial zones using .get() to pass raw non-owning pointers
    the_bronx.add_driver_to_zone(drivers[0].get());
    the_bronx.add_driver_to_zone(drivers[1].get());
    staten_island.add_driver_to_zone(drivers[2].get());
    staten_island.add_driver_to_zone(drivers[3].get());

    // Next, we spawn Riders on the heap
    std::vector<std::unique_ptr<Rider>> riders;
    riders.push_back(std::make_unique<Rider>("101", "Count Rapula", Coordinates{1.5, 2.5}));
    riders.push_back(std::make_unique<Rider>("102", "Jim the Vampire", Coordinates{4.5, 1.5}));
    riders.push_back(std::make_unique<Rider>("103", "The Baron", Coordinates{2.5, 5.5}));

    DispatchEngine engine;

    // Atomic counters shared safely across all threads without needing a mutex
    std::atomic<int> completed_trips{0};
    std::atomic<int> audit_checks{0};

    // Scoped Simulation Block: All std::jthreads automatically stop & join at the closing '}'
    {
        // THREAD 1: Simulates drivers moving and crossing between zones
        std::jthread gps_worker([&](std::stop_token stoken) {
            std::mt19937 rng{42};
            std::uniform_real_distribution<double> move_dist{-0.5, 0.5};
            int tick = 0;

            while (!stoken.stop_requested()) {
                for (const auto& d : drivers) {
                    Coordinates loc {d->get_live_coordinates()};
                    d->update_live_coordinates(Coordinates{loc.latitude + move_dist(rng), loc.longitude + move_dist(rng)});
                }

                // Periodically move Nandor the Relentless (drivers[0]) across neighborhood borders
                if (tick % 5 == 0) {
                    transfer_driver_to_zone(the_bronx, staten_island, drivers[0].get());
                } else if (tick % 5 == 2) {
                    transfer_driver_to_zone(staten_island, the_bronx, drivers[0].get());
                }

                ++tick;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });

        // REUSABLE WORKER LAMBDA: Simulates a rider repeatedly booking & completing rides
        auto rider_simulation {[&](std::stop_token stoken, Rider& rider, CityZone& primary_zone, CityZone& backup_zone) {
            while (!stoken.stop_requested()) {
                Driver* matched {engine.dispatch_nearest_driver(rider, primary_zone)};
                if (matched == nullptr) {
                    matched = engine.dispatch_nearest_driver(rider, backup_zone);
                }

                if (matched != nullptr) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(15));
                    complete_ride(*matched, rider);
                    completed_trips.fetch_add(1);
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            }
        }};

        // THREADS 2, 3, & 4: Concurrent Rider Threads fighting for available drivers
        std::jthread rider_thread_1 {rider_simulation, std::ref(*riders[0]), std::ref(the_bronx), std::ref(staten_island)};
        std::jthread rider_thread_2 {rider_simulation, std::ref(*riders[1]), std::ref(staten_island), std::ref(the_bronx)};
        std::jthread rider_thread_3 {rider_simulation, std::ref(*riders[2]), std::ref(the_bronx), std::ref(staten_island)};

        // THREAD 5: Live Invariant Auditor checking data integrity continuously
        std::jthread auditor_thread{[&](std::stop_token stoken) {
            while (!stoken.stop_requested()) {
                for (const auto& d : drivers) {
                    if (!verify_driver_invariant(*d)) {
                        std::cerr << "[CRITICAL ERROR] Invariant broken for Driver " << d->get_id() << "!\n";
                        std::abort();
                    }
                }
                audit_checks.fetch_add(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }};

        // Let all 5 threads hammer the engine concurrently for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "Stopping simulation workers via std::jthread RAII destructors...\n";
    }
    // Every std::jthread destructor (std::~jthread()) fires after the above closing brace, requesting 
    // stop and waiting for clean exit. If we had used std::thread instead, we would have had to
    // manually invoke 'join()' to join this thread with the spawned threads. If we forget to
    // invoke it, our app crashes. Also, we do not have to write clumsy code by wrapping our code
    // in a try {} catch(...) {} block, and have to call join() in both the try {} and catch(...) {}
    // sections

    std::cout << "\n<<<<<<<<<< Simulation Complete >>>>>>>>>>>\n";
    std::cout << "Total Atomic Trips Completed: " << completed_trips.load() << "\n";
    std::cout << "Live Invariant Audits Passed: " << audit_checks.load() << " (0 violations)\n";

    return EXIT_SUCCESS;
}
