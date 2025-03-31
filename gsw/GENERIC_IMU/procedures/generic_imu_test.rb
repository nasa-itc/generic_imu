require 'cosmos'
require 'cosmos/script'
require 'mission_lib.rb'

class IMU_LPT < Cosmos::Test
  def setup
    
  end

  def test_lpt
    start("tests/generic_imu_lpt_test.rb")
  end

  def teardown

  end
end

class IMU_CPT < Cosmos::Test
  def setup
      
  end

  def test_cpt
    start("tests/generic_imu_cpt_test.rb")
  end

  def teardown

  end
end

class Generic_imu_Test < Cosmos::TestSuite
  def initialize
      super()
      add_test('IMU_CPT')
      add_test('IMU_LPT')
  end

  def setup
  end
  
  def teardown
  end
end