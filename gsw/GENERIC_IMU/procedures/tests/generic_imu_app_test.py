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

def run_generic_imu_app_test():
    ##
    ## This script tests the standard cFS component application functionality.
    ## Currently this includes: 
    ##   Housekeeping, request telemetry to be published on the software bus
    ##   NOOP, no operation but confirm correct counters increment
    ##   Reset counters, increment as done in NOOP and confirm ability to clear repeatably
    ##   Invalid ground command, confirm bad lengths and codes are rejected
    ##


    ##
    ##   Housekeeping, request telemetry to be published on the software bus
    ##
    for n in range(GENERIC_IMU_TEST_LOOP_COUNT):
        get_generic_imu_hk()


    ##
    ## NOOP, no operation but confirm correct counters increment
    ##
    for n in range(GENERIC_IMU_TEST_LOOP_COUNT):
        generic_imu_cmd("GENERIC_IMU_DEBUG GENERIC_IMU_NOOP_CC")


    ##
    ## Reset counters, increment as done in NOOP and confirm ability to clear repeatably
    ##
    for n in range(GENERIC_IMU_TEST_LOOP_COUNT):
        generic_imu_cmd("GENERIC_IMU_DEBUG GENERIC_IMU_NOOP_CC")
        cmd("GENERIC_IMU_DEBUG GENERIC_IMU_RST_COUNTERS_CC") # Note standard `cmd` as we can't reset counters and then confirm increment
        get_generic_imu_hk()
        check("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT == 0")
        check("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_ERR_COUNT == 0")


    ##
    ##   Invalid ground command, confirm bad lengths and codes are rejected
    ##
    for n in range(GENERIC_IMU_TEST_LOOP_COUNT):
        # Bad length
        cmd_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
        cmd(f"GENERIC_IMU_DEBUG GENERIC_IMU_NOOP_CC with CCSDS_LENGTH {n+2}") # Note +2 due to CCSDS already being +1
        get_generic_imu_hk()
        check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")

    for n in range(6, 5 + GENERIC_IMU_TEST_LOOP_COUNT + 1):
        # Bad command codes
        cmd_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT")
        cmd_err_cnt = tlm("GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
        cmd(f"GENERIC_IMU_DEBUG GENERIC_IMU_NOOP_CC with CCSDS_FC {n+1}")
        get_generic_imu_hk()
        check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_COUNT == {cmd_cnt}")
        check(f"GENERIC_IMU_DEBUG GENERIC_IMU_HK_TLM CMD_ERR_COUNT == {cmd_err_cnt+1}")