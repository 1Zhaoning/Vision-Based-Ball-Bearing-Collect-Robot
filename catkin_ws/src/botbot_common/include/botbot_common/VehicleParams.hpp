#pragma once

#include "VectorXY.hpp"

namespace mms::VehicleParams {

/* Assume SI base units unless specified otherwise */

struct Dimensions {
    constexpr static const double trackwidth      = 1.44;
    constexpr static const double COG_nose        = 1.85;
    constexpr static const double COG_jacking_bar = 1.0;
    constexpr static const double COG_front       = 0.8672;
    constexpr static const double COG_rear        = 0.6183;
    constexpr static const double wheelbase       = COG_front + COG_rear;
    constexpr static const double mass            = 280;
    constexpr static const double yaw_inertia     = 230;

    constexpr static const double front_weight_ratio = COG_rear / (COG_front + COG_rear);
    constexpr static const double rear_weight_ratio  = COG_front / (COG_front + COG_rear);
};

struct Aero {
    constexpr static const double CDA = 1.56; // M18E
    constexpr static const double CLA = 3.1;  // M18E
};

struct Sensors {
    struct Lidar {
        constexpr static const double COG_offset = 1.82;
        constexpr static const double rate       = 10;
    };
    inline static const mms::VectorXY Lidar = mms::VectorXY(Lidar::COG_offset, 0.0);

    struct LidarRollhoop {
        constexpr static const double COG_offset = -0.9;
        constexpr static const double rate       = 10;
    };
    inline static const mms::VectorXY LidarRollhoop = mms::VectorXY(LidarRollhoop::COG_offset, 0.0);

    struct GPS {
        constexpr static const double COG_offset = 0.92;
        constexpr static const double rate       = 10;
    };
    inline static const mms::VectorXY GPS = mms::VectorXY(GPS::COG_offset, 0.0);

    struct IMU {
        constexpr static const double COG_offset = -0.45;
        constexpr static const double rate       = 200;
    };
    inline static const mms::VectorXY IMU = mms::VectorXY(IMU::COG_offset, 0.0);

    struct MAG {
        constexpr static const double COG_offset = -0.45;
        constexpr static const double rate       = 12.5;
    };
    inline static const mms::VectorXY MAG = mms::VectorXY(MAG::COG_offset, 0.0);

    struct Cameras {
        constexpr static const double COG_offset = -0.9;
        constexpr static const double rate       = 30;
    };
    inline static const mms::VectorXY Cameras = mms::VectorXY(Cameras::COG_offset, 0.0);

    struct WheelSpeed {
        constexpr static const double COG_offset = 0.0;
        constexpr static const double rate       = 50;
    };
    inline static const mms::VectorXY WheelSpeed = mms::VectorXY(WheelSpeed::COG_offset, 0.0);
};
} // namespace mms::VehicleParams