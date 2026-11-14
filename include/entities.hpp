#ifndef ENTITIES_HPP_
#define ENTITIES_HPP_

struct Coordinates {
    double latitude {};
    double longitude {};
};

enum class DriverStatus {AVAILABLE, EN_ROUTE, MATCHED};

enum class RiderStatus {IDLE, MATCHED};


#endif