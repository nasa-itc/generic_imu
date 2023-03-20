#ifndef NOS3_GENERIC_IMUHARDWAREMODEL_HPP
#define NOS3_GENERIC_IMUHARDWAREMODEL_HPP

/*
** Includes
*/
#include <map>

#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>

#include <Client/Bus.hpp>
#include <Can/Client/CanSlave.hpp> //The right protocol, actually

#include <sim_i_data_provider.hpp>
#include <generic_imu_data_point.hpp>
#include <sim_i_hardware_model.hpp>


/*
** Defines
*/
#define GENERIC_IMU_SIM_SUCCESS 0
#define GENERIC_IMU_SIM_ERROR   1


/*
** Namespace
*/
namespace Nos3
{
    /* Standard for a hardware model */
    class Generic_imuHardwareModel : public SimIHardwareModel
    {
    public:
        /* Constructor and destructor */
        Generic_imuHardwareModel(const boost::property_tree::ptree& config);
        ~Generic_imuHardwareModel(void);
        std::vector<uint8_t> determine_can_response(const std::vector<uint8_t>& in_data); /* Handle data the hardware receives from its protocol bus */

    private:
        /* Private helper methods */
        void create_generic_imu_hk(std::vector<uint8_t>& out_data); 
        void create_generic_imu_data(std::vector<uint8_t>& out_data); 
        void command_callback(NosEngine::Common::Message msg); /* Handle backdoor commands and time tick to the simulator */

        /* Private data members */
        class IMUCanSlaveConnection*                        _can_connection; /* TODO: Finish changing everything else so this is actually CAN and not UART */
        std::unique_ptr<NosEngine::Client::Bus>             _time_bus; /* Standard */

        SimIDataProvider*                                   _generic_imu_dp; /* Only needed if the sim has a data provider */

        /* Internal state data */
        std::uint8_t                                        _enabled;
        std::uint32_t                                       _count;
        std::uint32_t                                       _config;
        std::uint32_t                                       _status;
        
        #define IMU_CAN_FRAME_SIZE 16
    };

    class IMUCanSlaveConnection : public NosEngine::Can::CanSlave
    {
    public:
        IMUCanSlaveConnection(Generic_imuHardwareModel* hm, int bus_address, std::string connection_string, std::string bus_name);
        size_t can_read(uint8_t *rbuf, size_t rlen);
        size_t can_write(const uint8_t *wbuf, size_t wlen);
    private:
        Generic_imuHardwareModel* _hardware_model;
        std::vector<uint8_t> _can_out_data;  
    };

}

#endif
