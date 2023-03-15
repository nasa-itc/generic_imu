#include <ItcLogger/Logger.hpp>
#include <generic_imu_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_imuDataPoint::Generic_imuDataPoint(double count)
    {
        sim_logger->trace("Generic_imuDataPoint::Generic_imuDataPoint:  Defined Constructor executed");

        /* Do calculations based on provided data */
        _generic_imu_data_is_valid = true;
        _not_parsed = true;
        _generic_imu_data[0] = count * 0.001;
        _generic_imu_data[1] = count * 0.002;
        _generic_imu_data[2] = count * 0.003;
    }

    Generic_imuDataPoint::Generic_imuDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : _sc(spacecraft), _dp(*dp)
    {
        sim_logger->trace("Generic_imuDataPoint::Generic_imuDataPoint:  42 Constructor executed");

        /* Initialize data */
        _generic_imu_data_is_valid = false;
        _generic_imu_data[0] = 0.0;
        _generic_imu_data[1] = 0.0;
        _generic_imu_data[2] = 0.0;

        /*
        ** Declare 42 telemetry string prefix
        ** 42 variables defined in `42/Include/42types.h`
        ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
        */
        std::ostringstream MatchString;
        MatchString << "SC[" << spacecraft << "].svb = "; /* TODO: Change me to match the data from 42 you are interested in */
        size_t MSsize = MatchString.str().size();

        /* Parse 42 telemetry */
        std::vector<std::string> lines = dp->get_lines();
        try 
        {
            for (int i = 0; i < lines.size(); i++) 
            {
                /* Compare prefix */
                if (lines[i].compare(0, MSsize, MatchString.str()) == 0) 
                {
                    size_t found = lines[i].find_first_of("=");
                    /* Parse line */
                    std::istringstream iss(lines[i].substr(found+1, lines[i].size()-found-1));
                    /* Custom work to extract the data from the 42 string and save it off in the member data of this data point */
                    std::string s;
                    iss >> s;
                    _generic_imu_data[0] = std::stod(s);
                    iss >> s;
                    _generic_imu_data[1] = std::stod(s);
                    iss >> s;
                    _generic_imu_data[2] = std::stod(s);
                    /* Mark data as valid */
                    _generic_imu_data_is_valid = true;
                    /* Debug print */
                    sim_logger->trace("Generic_imuDataPoint::Generic_imuDataPoint:  Parsed svb = %f %f %f", _generic_imu_data[0], _generic_imu_data[1], _generic_imu_data[2]);
                }
            }
        } 
        catch(const std::exception& e) 
        {
            /* Report error */
            sim_logger->error("Generic_imuDataPoint::Generic_imuDataPoint:  Parsing exception %s", e.what());
        }
    }


   /*************************************************************************
    * Mutators
    *************************************************************************/
    void Generic_imuDataPoint::do_parsing(void) const
    {
        std::ostringstream MatchStringGyro;
        std::ostringstream MatchStringAccel;
        MatchStringGyro << "SC[" << _sc << "].AC.Gyro[";
        MatchStringAccel << "SC[" << _sc << "].AC.Accel[";
        size_t MSGyroSize = MatchStringGyro.str().size();
        size_t MSAccelSize = MatchStringAccel.str().size();

        _not_parsed = false;

        std::vector<std::string> lines = _dp.get_lines();

        try {
            for (int i = 0; i < lines.size(); i++) {
               if (lines[i].compare(0, MSGyroSize, MatchStringGyro.str()) == 0) { // e.g. SC[0].AC.gyro[
                  if (lines[i].compare(MSGyroSize, 1, "0") == 0) { // e,g, SC[0].AC.gyro[0
                     size_t found = lines[i].find_first_of("=");
                     _gyroRates[0] = std::stof(lines[i].substr(found+1, lines[i].size() - found - 1));
                     #ifdef IMU_SIM_DATAPOINT_DEBUG
                        sim_logger->trace("IMUDataPoint::do_parsing: Parsed gyroX. = found at %d, rhs=%s, _gyroRates[0]=%f",
                           found, lines[i].substr(found+1, lines[i].size() - found - 1).c_str(), _gyroRates[0]);
                     #endif
                  }
                  else if (lines[i].compare(MSGyroSize, 1, "1") == 0) { // e,g, SC[0].AC.gyro[1
                     size_t found = lines[i].find_first_of("=");
                     _gyroRates[1] = std::stof(lines[i].substr(found+1, lines[i].size() - found - 1));
                     #ifdef IMU_SIM_DATAPOINT_DEBUG
                        sim_logger->trace("IMUDataPoint::do_parsing: Parsed gyroY. = found at %d, rhs=%s, _gyroRates[1]=%f",
                           found, lines[i].substr(found+1, lines[i].size() - found - 1).c_str(), _gyroRates[1]);
                     #endif
                  }
                  else if (lines[i].compare(MSGyroSize, 1, "2") == 0) { // e,g, SC[0].AC.gyro[2
                        size_t found = lines[i].find_first_of("=");
                        _gyroRates[2] = std::stof(lines[i].substr(found+1, lines[i].size() - found - 1));
                        #ifdef IMU_SIM_DATAPOINT_DEBUG
                           sim_logger->trace("IMUDataPoint::do_parsing: Parsed gyroZ. = found at %d, rhs=%s, _gyroRates[2]=%f",
                              found, lines[i].substr(found+1, lines[i].size() - found - 1).c_str(), _gyroRates[2]);
                        #endif
                     }
                  }
                  else if (lines[i].compare(0, MSAccelSize, MatchStringAccel.str()) == 0) { // e.g. SC[0].AC.accel[
                  if (lines[i].compare(MSAccelSize, 1, "0") == 0) { // e,g, SC[0].AC.accel[0
                        size_t found = lines[i].find_first_of("=");
                        _accelRates[0] = std::stof(lines[i].substr(found+1, lines[i].size() - found - 1));
                        #ifdef IMU_SIM_DATAPOINT_DEBUG
                           sim_logger->trace("IMUDataPoint::do_parsing: Parsed accelX. = found at %d, rhs=%s, _accelRates[0]=%f",
                              found, lines[i].substr(found+1, lines[i].size() - found - 1).c_str(), _accelRates[0]);
                     #endif
                     }
                  else if (lines[i].compare(MSAccelSize, 1, "1") == 0) { // e,g, SC[0].AC.accel[1
                     size_t found = lines[i].find_first_of("=");
                     _accelRates[1] = std::stof(lines[i].substr(found+1, lines[i].size() - found - 1));
                     #ifdef IMU_SIM_DATAPOINT_DEBUG
                        sim_logger->trace("IMUDataPoint::do_parsing: Parsed accelY. = found at %d, rhs=%s, _accelRates[1]=%f",
                           found, lines[i].substr(found+1, lines[i].size() - found - 1).c_str(), _accelRates[1]);
                     #endif
                  }
                  else if (lines[i].compare(MSAccelSize, 1, "2") == 0) { // e,g, SC[0].AC.accel[2
                     size_t found = lines[i].find_first_of("=");
                     _accelRates[2] = std::stof(lines[i].substr(found+1, lines[i].size() - found - 1));
                     #ifdef IMU_SIM_DATAPOINT_DEBUG
                        sim_logger->trace("IMUDataPoint::do_parsing: Parsed accelZ. = found at %d, rhs=%s, _accelRates[2]=%f",
                           found, lines[i].substr(found+1, lines[i].size() - found - 1).c_str(), _accelRates[2]);
                     #endif
                  }
               }
            }

        } catch(const std::exception& e) {
            sim_logger->error("Stim300DataPoint::do_parsing: Parsing exception: %s", e.what());
        }
        #ifdef IMU_SIM_DATAPOINT_DEBUG
            sim_logger->debug("Stim300DataPoint::do_parsing: Parsed data point:\n%s", to_string().c_str());
        #endif
    }



    /* Used for printing a representation of the data point */
    std::string Generic_imuDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_imuDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Generic_imu Data Point:   Valid: ";
        ss << (_generic_imu_data_is_valid ? "Valid" : "INVALID");
        ss << std::setprecision(std::numeric_limits<double>::digits10); /* Full double precision */
        ss << " Generic_imu Data: "
           << _generic_imu_data[0]
           << " "
           << _generic_imu_data[1]
           << " "
           << _generic_imu_data[2];

        return ss.str();
    }
} /* namespace Nos3 */
