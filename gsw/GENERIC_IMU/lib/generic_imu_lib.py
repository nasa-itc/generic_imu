# Library for GENERIC_IMU Target
from openc3.script import cmd, tlm, check, wait_check_packet, check_tolerance
import time

#
# Definitions
#
GENERIC_IMU_CMD_SLEEP = 0.25
GENERIC_IMU_RESPONSE_TIMEOUT = 5
GENERIC_IMU_TEST_LOOP_COUNT = 1
GENERIC_IMU_DEVICE_LOOP_COUNT = 5
GENERIC_IMU_DEVICE_ANGULAR_DIFF = 0.2
GENERIC_IMU_DEVICE_LINEAR_DIFF = 0.5

#
# Functions
#
def get_generic_imu_hk():
    cmd("GENERIC_IMU_DEBUG GENERIC_IMU_REQ_HK")
    count = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT")
    wait_check_packet("GENERIC_IMU_DEBUG", "GENERIC_IMU_HK_TLM", 1, GENERIC_IMU_RESPONSE_TIMEOUT)
    time.sleep(GENERIC_IMU_CMD_SLEEP)

def get_generic_imu_data():
    cmd("GENERIC_IMU_DEBUG GENERIC_IMU_REQ_DATA")
    wait_check_packet("GENERIC_IMU_DEBUG", "GENERIC_IMU_DATA_TLM", 1, GENERIC_IMU_RESPONSE_TIMEOUT)
    time.sleep(GENERIC_IMU_CMD_SLEEP)

def generic_imu_cmd(command_string):
    count = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT") + 1

    if (count == 256):
        count = 0

    cmd(command_string)
    get_generic_imu_hk()
    current = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT")
    if (current != count):
        # Try again
        cmd(command_string)
        get_generic_imu_hk()
        current = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT")
        if (current != count):
            # Third times the charm
            cmd(command_string)
            get_generic_imu_hk()
            current = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT")
            
    check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT >= {count}")

def enable_generic_imu():
    # Send command
    generic_imu_cmd("GENERIC_IMU_DEBUG GENERIC_IMU_ENABLE_CC")
    # Confirm
    check("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM DEVICE_ENABLED == 'ENABLED'")

def disable_generic_imu():
    # Send command
    generic_imu_cmd("GENERIC_IMU_DEBUG GENERIC_IMU_DISABLE_CC")
    # Confirm
    check("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM DEVICE_ENABLED == 'DISABLED'")

def safe_generic_imu():
    get_generic_imu_hk()
    state = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM DEVICE_ENABLED")
    if (state != "DISABLED"):
        disable_generic_imu()

def confirm_generic_imu_data():
    dev_cmd_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM DEVICE_COUNT")
    dev_cmd_err_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")
    
    get_generic_imu_data()
    # Note these checks assume default simulator configuration

    # X Axis Angular
    truth_42_GYRO_X = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA GYRO_B_X")
    check_tolerance("GENERIC_IMU_DEBUG GENERIC_IMU_DATA_TLM X_ANGULAR_RATE", truth_42_GYRO_X, GENERIC_IMU_DEVICE_ANGULAR_DIFF)

    # Y Axis Angular
    truth_42_GYRO_Y = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA GYRO_B_Y")
    check_tolerance("GENERIC_IMU_DEBUG GENERIC_IMU_DATA_TLM Y_ANGULAR_RATE", truth_42_GYRO_Y, GENERIC_IMU_DEVICE_ANGULAR_DIFF)

    # Z Axis Angular
    truth_42_GYRO_Z = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA GYRO_B_Z")
    check_tolerance("GENERIC_IMU_DEBUG GENERIC_IMU_DATA_TLM Z_ANGULAR_RATE", truth_42_GYRO_Z, GENERIC_IMU_DEVICE_ANGULAR_DIFF)

    # X Axis Linear
    truth_42_ACC_X = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA ACC_B_X")
    check_tolerance("GENERIC_IMU_DEBUG GENERIC_IMU_DATA_TLM X_LINEAR_ACCELERATION", truth_42_ACC_X, GENERIC_IMU_DEVICE_LINEAR_DIFF)

    # Y Axis Linear
    truth_42_ACC_Y = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA ACC_B_Y")
    check_tolerance("GENERIC_IMU_DEBUG GENERIC_IMU_DATA_TLM Y_LINEAR_ACCELERATION", truth_42_ACC_Y, GENERIC_IMU_DEVICE_LINEAR_DIFF)

    # Z Axis Linear
    truth_42_ACC_Z = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA ACC_B_Z")
    check_tolerance("GENERIC_IMU_DEBUG GENERIC_IMU_DATA_TLM Z_LINEAR_ACCELERATION", truth_42_ACC_Z, GENERIC_IMU_DEVICE_LINEAR_DIFF)

    get_generic_imu_hk()
    check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM DEVICE_COUNT >= {dev_cmd_cnt}")
    check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT == {dev_cmd_err_cnt}")

def confirm_generic_imu_data_loop():
    for n in range(GENERIC_IMU_DEVICE_LOOP_COUNT):
        confirm_generic_imu_data()

#
# Simulator Functions
#
def generic_imu_prepare_ast():
    # Get to known state
    safe_generic_imu()

    # Enable
    enable_generic_imu()

    # Confirm data
    confirm_generic_imu_data_loop()

def generic_imu_sim_enable():
    cmd("SIM_CMDBUS_BRIDGE GENERIC_IMU_SIM_ENABLE")

def generic_imu_sim_disable():
    cmd("SIM_CMDBUS_BRIDGE GENERIC_IMU_SIM_DISABLE")

def generic_imu_sim_set_status(status):
    cmd(f"SIM_CMDBUS_BRIDGE GENERIC_IMU_SIM_SET_STATUS with STATUS {status}")