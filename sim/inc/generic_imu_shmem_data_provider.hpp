#ifndef NOS3_GENERIC_IMU_SHMEM_DATA_PROVIDER_HPP
#define NOS3_GENERIC_IMU_SHMEM_DATA_PROVIDER_HPP

#include <boost/property_tree/ptree.hpp>
#include <ItcLogger/Logger.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <generic_imu_data_point.hpp>
#include <sim_i_data_provider.hpp>
#include <blackboard_data.hpp>

namespace Nos3
{
    namespace bip = boost::interprocess;

    class GenericImuShmemDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        GenericImuShmemDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~GenericImuShmemDataProvider(void) {};
        GenericImuShmemDataProvider& operator=(const GenericImuShmemDataProvider&) {return *this;};

        bip::mapped_region _shm_region;
        BlackboardData*    _blackboard_data;
    };
}

#endif
