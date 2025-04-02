require 'cosmos'
require 'cosmos/script'
require "cfs_lib.rb"
#require 'math'

##
## NOOP
##
initial_command_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT")
initial_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_IMU GENERIC_IMU_NOOP_CC")
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Successful Disable
##
initial_command_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT")
initial_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_IMU GENERIC_IMU_DISABLE_CC")
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ENABLED == 'DISABLED'", 30)

sleep(5)

##
## Failed Disable (doubled)
##
initial_command_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT")
initial_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_IMU GENERIC_IMU_DISABLE_CC")
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT > #{initial_device_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ENABLED == 'DISABLED'", 30)

sleep(5)

##
## HK without Device
##
initial_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_IMU GENERIC_IMU_REQ_HK")
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Data without Device
##
initial_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_IMU GENERIC_IMU_REQ_DATA")
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Successful Enable
##
initial_command_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT")
initial_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_IMU GENERIC_IMU_ENABLE_CC")
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ENABLED == 'ENABLED'", 30)

sleep(5)

##
## Failed Enable (doubled)
##
initial_command_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT")
initial_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_IMU GENERIC_IMU_ENABLE_CC")
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT > #{initial_command_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT > #{initial_device_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ENABLED == 'ENABLED'", 30)

sleep(5)

##
## Housekeeping w/ Device
##
initial_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")
cmd("GENERIC_IMU GENERIC_IMU_REQ_HK")
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Data w/ Device
##
initial_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT")
initial_device_error_count = tlm("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT")

cmd("GENERIC_IMU GENERIC_IMU_REQ_DATA")

diff = 0.2

# X Axis
imu_angular_acc_x = tlm("GENERIC_IMU GENERIC_IMU_DATA_TLM X_ANGULAR_ACCELERATION")
truth_42_wn_0 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA WN_0")
wait_check_tolerance("GENERIC_IMU GENERIC_IMU_DATA_TLM X_ANGULAR_ACCELERATION", truth_42_wn_0, diff, 15)

# Y Axis
imu_angular_acc_y = tlm("GENERIC_IMU GENERIC_IMU_DATA_TLM Y_ANGULAR_ACCELERATION")
truth_42_wn_1 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA WN_1")
wait_check_tolerance("GENERIC_IMU GENERIC_IMU_DATA_TLM Y_ANGULAR_ACCELERATION", truth_42_wn_1, diff, 15)

# Z Axis
imu_angular_acc_z = tlm("GENERIC_IMU GENERIC_IMU_DATA_TLM Z_ANGULAR_ACCELERATION")
truth_42_wn_2 = tlm("SIM_42_TRUTH SIM_42_TRUTH_DATA WN_2")
wait_check_tolerance("GENERIC_IMU GENERIC_IMU_DATA_TLM Z_ANGULAR_ACCELERATION", truth_42_wn_2, diff, 15)

wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == #{initial_error_count}", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT == #{initial_device_error_count}", 30)

sleep(5)

##
## Reset Counters
##
cmd("GENERIC_IMU GENERIC_IMU_DISABLE_CC") # disable to be able to properly check that device count gets reset
cmd("GENERIC_IMU GENERIC_IMU_RST_COUNTERS_CC")
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_COUNT == 0", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM CMD_ERR_COUNT == 0", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_COUNT == 0", 30)
wait_check("GENERIC_IMU GENERIC_IMU_HK_TLM DEVICE_ERR_COUNT == 0", 30)

##
## Reenable so test can be run multiple times
##
cmd("GENERIC_IMU GENERIC_IMU_ENABLE_CC")