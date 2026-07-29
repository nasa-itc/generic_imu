import sys
import glob

for p in glob.glob('/gems/gems/openc3-cosmos-nos3-*/targets/GENERIC_IMU/scripts'):
    if p not in sys.path:
        sys.path.append(p)

from openc3.script import cmd, tlm, check

try:
    from nos3.generic_imu_lib import *
except ImportError:
    pass

def run_generic_imu_ast_test():
    ##
    ## This script tests the cFS component device functionality.
    ## Currently this includes: 
    ##   Enable / disable, control hardware communications
    ##   Configuration, reconfigure generic_imu instrument register
    ##


    ##
    ## Enable / disable, control hardware communications
    ##
    for n in range(GENERIC_IMU_TEST_LOOP_COUNT):
        safe_generic_imu() # Get to known state

        # Manually command to disable when already disabled
        cmd_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
        cmd("GENERIC_IMU_DEBUG GENERIC_IMU_DISABLE_CC")
        get_generic_imu_hk()
        check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")

        # Enable
        enable_generic_imu()

        # Confirm device counters increment without errors
        confirm_generic_imu_data_loop()

        # Manually command to enable when already enabled
        cmd_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
        cmd("GENERIC_IMU_DEBUG GENERIC_IMU_ENABLE_CC")
        get_generic_imu_hk()
        check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")

        # Reconfirm data remains as expected
        confirm_generic_imu_data_loop()

        # Disable
        disable_generic_imu()