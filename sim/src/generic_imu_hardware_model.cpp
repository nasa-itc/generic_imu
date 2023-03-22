#include <generic_imu_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_imuHardwareModel,"GENERIC_IMU");

    extern ItcLogger::Logger *sim_logger;

    Generic_imuHardwareModel::Generic_imuHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _enabled(0), _count(0), _config(0), _status(0)
    {
        /* Get the NOS engine connection string */
        std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        /* Get a data provider */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_IMU_PROVIDER");
        _generic_imu_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  Data provider %s created.", dp_name.c_str());

        /* Get on a protocol bus */
        /* Note: Initialized defaults in case value not found in config file */
        std::string bus_name = "can_0";
        int node_port = 0;
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            /* Loop through the connections for hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections"))
            {
                /* v.second is the child tree (v.first is the name of the child) */
                if (v.second.get("type", "").compare("can") == 0)
                {
                    /* Configuration found */
                    bus_name = v.second.get("bus-name", bus_name);
                    node_port = v.second.get("node-port", node_port);
                    break;
                }
            }
        }
//        _can_connection.reset(new NosEngine::Can::CanSlave(_hub, config.get("simulator.name", "generic_imu_sim"), connection_string, bus_name)); //This might not be how to do this; I just replaced NosEngine::Uart::Uart with NosEngine::Can::CanSlave.
//        _can_connection->open(node_port);
        _can_connection = new IMUCanSlaveConnection(this, node_port, connection_string, bus_name);
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  Now on CAN bus name %s, port %d.", bus_name.c_str(), node_port);
    
        /* Configure protocol callback */
//        _can_connection->set_read_callback(std::bind(&Generic_imuHardwareModel::determine_can_response, this, std::placeholders::_1, std::placeholders::_2));
//        _uart_connection->set_read_callback(std::bind(&Generic_imuHardwareModel::uart_read_callback, this, std::placeholders::_1, std::placeholders::_2)); //This will need to be replaced with the above.

        /* Get on the command bus*/
        std::string time_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) 
        {
            /* Loop through the connections for the hardware model */
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections"))
            {
                /* v.first is the name of the child */
                /* v.second is the child tree */
                if (v.second.get("type", "").compare("time") == 0) // 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    /* Found it... don't need to go through any more items*/
                    break; 
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  Now on time bus named %s.", time_bus_name.c_str());

        /* Construction complete */
        sim_logger->info("Generic_imuHardwareModel::Generic_imuHardwareModel:  Construction complete.");
    }


    Generic_imuHardwareModel::~Generic_imuHardwareModel(void)
    {        
        /* Close the protocol bus */
//        _can_connection->close();
        delete _can_connection;
        _can_connection = nullptr;

        /* Clean up the data provider */
        delete _generic_imu_dp;
        _generic_imu_dp = nullptr;

        /* The bus will clean up the time node */
    }


    /* Automagically set up by the base class to be called */
    void Generic_imuHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        /* Get the data out of the message */
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_imuHardwareModel::command_callback:  Received command: %s.", dbf.data);

        /* Do something with the data */
        std::string command = dbf.data;
        std::string response = "Generic_imuHardwareModel::command_callback:  INVALID COMMAND! (Try HELP)";
        boost::to_upper(command);
        if (command.compare("HELP") == 0) 
        {
            response = "Generic_imuHardwareModel::command_callback: Valid commands are HELP, ENABLE, DISABLE, STATUS=X, or STOP";
        }
        else if (command.compare("ENABLE") == 0) 
        {
            _enabled = GENERIC_IMU_SIM_SUCCESS;
            response = "Generic_imuHardwareModel::command_callback:  Enabled";
        }
        else if (command.compare("DISABLE") == 0) 
        {
            _enabled = GENERIC_IMU_SIM_ERROR;
            _count = 0;
            _config = 0;
            _status = 0;
            response = "Generic_imuHardwareModel::command_callback:  Disabled";
        }
        else if (command.substr(0,7).compare("STATUS=") == 0)
        {
            try
            {
                _status = std::stod(command.substr(7));
                response = "Generic_imuHardwareModel::command_callback:  Status set";
            }
            catch (...)
            {
                response = "Generic_imuHardwareModel::command_callback:  Status invalid";
            }            
        }
        else if (command.compare("STOP") == 0) 
        {
            _keep_running = false;
            response = "Generic_imuHardwareModel::command_callback:  Stopping";
        }
        /* TODO: Add anything additional commands here */

        /* Send a reply */
        sim_logger->info("Generic_imuHardwareModel::command_callback:  Sending reply: %s.", response.c_str());
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }


    /* Custom function to prepare the Generic_imu HK telemetry */
    void Generic_imuHardwareModel::create_generic_imu_hk(std::vector<uint8_t>& out_data)
    {
        /* Prepare data size */
        out_data.resize(12, 0x00);

        // I removed both header and trailer from this, since CAN does not use the same kind
        // as other protocols do.

        /* Sequence count */
        out_data[0] = (_count >> 24) & 0x000000FF; 
        out_data[1] = (_count >> 16) & 0x000000FF; 
        out_data[2] = (_count >>  8) & 0x000000FF; 
        out_data[3] =  _count & 0x000000FF;
        
        /* Configuration */
        out_data[4] = (_config >> 24) & 0x000000FF; 
        out_data[5] = (_config >> 16) & 0x000000FF; 
        out_data[6] = (_config >>  8) & 0x000000FF; 
        out_data[7] =  _config & 0x000000FF;

        /* Device Status */
        out_data[8] = (_status >> 24) & 0x000000FF; 
        out_data[9] = (_status >> 16) & 0x000000FF; 
        out_data[10] = (_status >>  8) & 0x000000FF; 
        out_data[11] =  _status & 0x000000FF;

    }


    /* Custom function to prepare the Generic_imu Data */
    void Generic_imuHardwareModel::create_generic_imu_data(std::vector<uint8_t>& out_data)
    {
        boost::shared_ptr<Generic_imuDataPoint> data_point = boost::dynamic_pointer_cast<Generic_imuDataPoint>(_generic_imu_dp->get_data_point());

        /* Prepare data size */
        out_data.resize(28, 0x00); //Was originally 14

        // Header and trailer removed from this one as well.
       
        /* Sequence count */
        out_data[0] = (_count >> 24) & 0x000000FF; 
        out_data[1] = (_count >> 16) & 0x000000FF; 
        out_data[2] = (_count >>  8) & 0x000000FF; 
        out_data[3] =  _count & 0x000000FF;
        
        /* 
        ** Payload 
        ** 
        ** Device is big engian (most significant byte first)
        ** Assuming data is valid regardless of dynamic / environmental data
        ** Floating poing numbers are extremely problematic 
        **   (https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html)
        ** Most hardware transmits some type of unsigned integer (e.g. from an ADC), so that's what we've done
        ** Scale each of the x, y, z (which are in the range [-1.0, 1.0]) by 32767, 
        **   and add 32768 so that the result fits in a uint16
        */
//        uint16_t x   = (uint16_t)(data_point->get_generic_imu_data_x()*32767.0 + 32768.0);
//        out_data[6]  = (x >> 8) & 0x00FF;
//        out_data[7]  =  x       & 0x00FF;
        printf("X linear acceleration: %d\n", data_point->get_generic_imu_acc_x());
        printf("X angular acceleration: %d\n", data_point->get_generic_imu_gyro_x());
        printf("Y linear acceleration: %d\n", data_point->get_generic_imu_acc_y());
        printf("Y angular acceleration: %d\n", data_point->get_generic_imu_gyro_y());
        printf("Z linear acceleration: %d\n", data_point->get_generic_imu_acc_z());
        printf("Z angular acceleration: %d\n", data_point->get_generic_imu_gyro_z());

        uint32_t x_linear = (uint32_t)(data_point->get_generic_imu_acc_x()*8338607.0 + 8338608.0); //I am not sure of this conversion strategy, and it is the same one I use everywhere below.
        out_data[ 4] = (x_linear >> 24) & 0x00FF;
        out_data[ 5] = (x_linear >> 16) & 0x00FF;
        out_data[ 6] = (x_linear >> 8 ) & 0x00FF;
        out_data[ 7] =  x_linear        & 0x00FF;
        uint32_t x_angular = (uint32_t)(data_point->get_generic_imu_gyro_x()*8338607.0 + 8338608.0); 
        out_data[ 8] = (x_angular >> 24) & 0x00FF;
        out_data[ 9] = (x_angular >> 16) & 0x00FF;
        out_data[10] = (x_angular >> 8 ) & 0x00FF;
        out_data[11] =  x_angular        & 0x00FF;
        uint32_t y_linear = (uint32_t)(data_point->get_generic_imu_acc_y()*8338607.0 + 8338608.0); 
        out_data[12] = (y_linear >> 24) & 0x00FF;
        out_data[13] = (y_linear >> 16) & 0x00FF;
        out_data[14] = (y_linear >> 8 ) & 0x00FF;
        out_data[15] =  y_linear        & 0x00FF;
        uint32_t y_angular = (uint32_t)(data_point->get_generic_imu_gyro_y()*8338607.0 + 8338608.0); 
        out_data[16] = (y_angular >> 24) & 0x00FF;
        out_data[17] = (y_angular >> 16) & 0x00FF;
        out_data[18] = (y_angular >> 8 ) & 0x00FF;
        out_data[19] =  y_angular        & 0x00FF;
        uint32_t z_linear = (uint32_t)(data_point->get_generic_imu_acc_z()*8338607.0 + 8338608.0); 
        out_data[20] = (z_linear >> 24) & 0x00FF;
        out_data[21] = (z_linear >> 16) & 0x00FF;
        out_data[22] = (z_linear >> 8 ) & 0x00FF;
        out_data[23] =  z_linear        & 0x00FF;
        uint32_t z_angular = (uint32_t)(data_point->get_generic_imu_gyro_z()*8338607.0 + 8338608.0); 
        out_data[24] = (z_angular >> 24) & 0x00FF;
        out_data[25] = (z_angular >> 16) & 0x00FF;
        out_data[26] = (z_angular >> 8 ) & 0x00FF;
        out_data[27] =  z_angular        & 0x00FF;

//        uint16_t y   = (uint16_t)(data_point->get_generic_imu_data_y()*8338607.0 + 8338608.0);
//        out_data[8]  = (y >> 8) & 0x00FF;
//        out_data[9]  =  y       & 0x00FF;
//        uint16_t z   = (uint16_t)(data_point->get_generic_imu_data_z()*8338607.0 + 8338608.0);
//        out_data[10] = (z >> 8) & 0x00FF;
//        out_data[11] =  z       & 0x00FF;

    }


    /* Protocol callback */
    std::vector<uint8_t> Generic_imuHardwareModel::determine_can_response(const std::vector<uint8_t>& in_data)
    {
        std::vector<uint8_t> out_data; 
        std::uint8_t valid = GENERIC_IMU_SIM_SUCCESS;
        
        std::uint32_t rcv_config;

        /* Retrieve data and log in man readable format */
        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  REQUEST %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str());

        /* Check simulator is enabled */
        if (_enabled != GENERIC_IMU_SIM_SUCCESS)
        {
            sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Generic_imu sim disabled!");
            valid = GENERIC_IMU_SIM_ERROR;
        }
        else
        {
            /* Check if message is incorrect size */
            if (in_data.size() != 9)
            {
                sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Invalid command size of %d received!", in_data.size());
                valid = GENERIC_IMU_SIM_ERROR;
            }
            // If I were to check the header, this is where I would want to do it.
            // Ditto for the trailer. The header (first chunk, at least) should typically
            // be 8, and the second chunk should be 0. The first 8 bits should be 80.

            if (valid == GENERIC_IMU_SIM_SUCCESS)
            {   
                /* Process command */
                switch (in_data[2])
                {
                    case 0:
                        /* NOOP */
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  NOOP command received!");
                        break;

                    case 1:
                        /* Request HK */
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Send HK command received!");
                        create_generic_imu_hk(out_data);
                        break;

                    case 2:
                        /* Request data */
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Send data command received!");
                        create_generic_imu_data(out_data);
                        break;

                    case 3:
                        /* Configuration */
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Configuration command received!");
                        _config  = in_data[5] << 24;
                        _config |= in_data[6] << 16;
                        _config |= in_data[7] << 8;
                        _config |= in_data[8];
                        break;
                    
                    default:
                        /* Unused command code */
                        valid = GENERIC_IMU_SIM_ERROR;
                        sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  Unused command %d received!", in_data[2]);
                        break;
                }
            }
        }

        /* Increment count and echo command since format valid */
        if (valid == GENERIC_IMU_SIM_SUCCESS)
        {
            _count++;
//            _can_connection->can_write(&in_data[0], in_data.size()); //THIS IS A PROBLEM

            /* Send response if existing */
            if (out_data.size() > 0)
            {
                sim_logger->debug("Generic_imuHardwareModel::determine_can_response:  REPLY %s",
                    SimIHardwareModel::uint8_vector_to_hex_string(out_data).c_str());
//                _can_connection->can_write(&out_data[0], out_data.size()); //SO IS THIS
            }
        }
    return out_data;
    }

    IMUCanSlaveConnection::IMUCanSlaveConnection(Generic_imuHardwareModel* hm, int bus_address, std::string connection_string, std::string bus_name)
        : NosEngine::Can::CanSlave(bus_address, connection_string, bus_name)
    {
        _hardware_model = hm;
        _can_out_data = {0,0,0,0,0,0,0,0,0,0,0,0,0};
        sim_logger->debug("IMUCanSlaveConnection::IMUCanSlaveConnection:  IMUCanSlaveConnection constructor complete");
    }

    size_t IMUCanSlaveConnection::can_read(uint8_t *rbuf, size_t rlen)
    {
        // copy data to buffer
        _can_out_data.resize(rlen);
        std::copy(_can_out_data.begin(), _can_out_data.begin() + _can_out_data.size(), rbuf);
        return rlen;
    }

    size_t IMUCanSlaveConnection::can_write(const uint8_t *wbuf, size_t wlen)
    {
        std::vector<uint8_t> in_data(wbuf, wbuf + _IMU_CAN_FRAME_SIZE);
        _can_out_data = _hardware_model->determine_can_response(in_data);
        return wlen;
    }

}
