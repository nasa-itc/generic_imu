#ifndef NOS3_GENERIC_IMUDATAPOINT_HPP
#define NOS3_GENERIC_IMUDATAPOINT_HPP

#include <boost/shared_ptr.hpp>
#include <sim_42data_point.hpp>

namespace Nos3
{
    /* Standard for a data point used transfer data between a data provider and a hardware model */
    class Generic_imuDataPoint : public SimIDataPoint
    {
    public:
        /* Constructors */
        Generic_imuDataPoint(double count);
        Generic_imuDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp);

        /* Accessors */
        /* Provide the hardware model a way to get the specific data out of the data point */
        std::string to_string(void) const;
        double      get_generic_imu_data_x(void) const {return _generic_imu_data[0];}
        double      get_generic_imu_data_y(void) const {return _generic_imu_data[1];}
        double      get_generic_imu_data_z(void) const {return _generic_imu_data[2];}
        float       get_generic_imu_gyro_x(void) const {parse_data_point(); return _gyroRates[0];}
        float       get_generic_imu_acc_x(void) const {parse_data_point(); return _accelRates[0];}
        float       get_generic_imu_gyro_y(void) const {parse_data_point(); return _gyroRates[1];}
        float       get_generic_imu_acc_y(void) const {parse_data_point(); return _accelRates[1];}
        float       get_generic_imu_gyro_z(void) const {parse_data_point(); return _gyroRates[2];}
        float       get_generic_imu_acc_z(void) const {parse_data_point(); return _accelRates[2];}

        bool        is_generic_imu_data_valid(void) const {return _generic_imu_data_is_valid;}
    
    private:
        /* Disallow these */
        Generic_imuDataPoint(void) {};
        Generic_imuDataPoint(const Generic_imuDataPoint&) {};
        ~Generic_imuDataPoint(void) {};

        /* Specific data you need to get from the data provider to the hardware model */
        /* You only get to this data through the accessors above */
        mutable bool   _generic_imu_data_is_valid;
        mutable double _generic_imu_data[3];

        // Added from the previous version of the IMU (stim300)
        inline void parse_data_point(void) const {if (_not_parsed) do_parsing();}
        void        do_parsing(void) const;
 
        Sim42DataPoint _dp;
        int16_t _sc;
        mutable bool _not_parsed;
//        mutable std::vector<float> _gyroRates, _accelRates;
        mutable float _gyroRates[3];
        mutable float _accelRates[3];
    };
}

#endif
