#ifndef ENTITIES_HPP_
#define ENTITIES_HPP_

struct Coordinates {
    explicit Coordinates(double lat = 0.0, double lng = 0.0) : latitude{lat}, longitude{lng} {}
    double latitude {};
    double longitude {};
};

enum class DriverStatus {AVAILABLE, EN_ROUTE, MATCHED};

enum class RiderStatus {IDLE, MATCHED};


#endif