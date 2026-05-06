#include <gtest/gtest.h>
#include "common.h"

namespace esphome::soyosource_virtual_meter::testing {

// OEM (dumb) algorithm
// buffer=10  min=100  max=2000
// Documented example table in soyosource_virtual_meter.cpp.

class OemTest : public ::testing::Test {
 protected:
  TestableSoyosourceVirtualMeter m_;
  void SetUp() override {
    m_.set_buffer(10);
    m_.set_min_power_demand(100);
    m_.set_max_power_demand(2000);
  }
};

TEST_F(OemTest, ConsumptionAboveMaxPlusBuffer) {
  // 5000 > 2000 + 10 -> return max_power_demand
  EXPECT_EQ(m_.calc_oem(5000), 2000);
}

TEST_F(OemTest, ConsumptionJustAboveMaxPlusBuffer) {
  // 2011 > 2010 -> return max
  EXPECT_EQ(m_.calc_oem(2011), 2000);
}

TEST_F(OemTest, ConsumptionAboveMaxNotBuffer) {
  // 2010 <= 2010 (not > max+buffer), but 2010 > 2000 -> abs(10 - 2000) = 1990
  EXPECT_EQ(m_.calc_oem(2010), 1990);
}

TEST_F(OemTest, ConsumptionJustAboveMax) {
  // 2001 > 2000 -> 1990
  EXPECT_EQ(m_.calc_oem(2001), 1990);
}

TEST_F(OemTest, ConsumptionAtMin) {
  // 100 >= 100 -> (abs(100-10) + (100-10)) / 2 = 90
  EXPECT_EQ(m_.calc_oem(100), 90);
}

TEST_F(OemTest, ConsumptionInRange) {
  // 500 >= 100 -> (abs(500-10) + (500-10)) / 2 = 490
  EXPECT_EQ(m_.calc_oem(500), 490);
}

TEST_F(OemTest, ConsumptionBelowMin) {
  // 90 < 100 -> 0
  EXPECT_EQ(m_.calc_oem(90), 0);
}

TEST_F(OemTest, ConsumptionZero) {
  // 0 < 100 -> 0
  EXPECT_EQ(m_.calc_oem(0), 0);
}

// Negative measurements algorithm
// buffer=10  min=100  max=900  zero_output_on_min=true
// Documented example table in soyosource_virtual_meter.cpp.

class NegTest : public ::testing::Test {
 protected:
  TestableSoyosourceVirtualMeter m_;
  void SetUp() override {
    m_.set_buffer(10);
    m_.set_min_power_demand(100);
    m_.set_max_power_demand(900);
    m_.set_zero_output_on_min_power_demand(true);
  }
};

TEST_F(NegTest, HighConsumptionClampedToMax) {
  // importing=1000, last=500 -> 1500 -> clamped to 900
  EXPECT_EQ(m_.calc_neg(1010, 500), 900);
}

TEST_F(NegTest, ConsumptionAtMax) {
  // importing=400, last=500 -> 900 (== max)
  EXPECT_EQ(m_.calc_neg(410, 500), 900);
}

TEST_F(NegTest, ConsumptionReducesDemand) {
  // importing=300, last=500 -> 800
  EXPECT_EQ(m_.calc_neg(310, 500), 800);
}

TEST_F(NegTest, SmallExportReducesDemand) {
  // importing=0, last=500 -> 500
  EXPECT_EQ(m_.calc_neg(10, 500), 500);
}

TEST_F(NegTest, ExportReducesDemandToMin) {
  // importing=-490, last=500 -> 10 < min=100 -> zero_output -> 0
  EXPECT_EQ(m_.calc_neg(-490, 500), 0);
}

TEST_F(NegTest, LargeExportZeroOutput) {
  // importing=-700, last=500 -> -200 < min -> 0
  EXPECT_EQ(m_.calc_neg(-690, 500), 0);
}

TEST_F(NegTest, ZeroOutputOnMinFalseReturnsMin) {
  m_.set_zero_output_on_min_power_demand(false);
  // importing=-490, last=500 -> 10 < min -> return min=100
  EXPECT_EQ(m_.calc_neg(-490, 500), 100);
}

TEST_F(NegTest, LastDemandZeroAndNoImport) {
  // importing=-10, last=0 -> -10 < min -> 0
  EXPECT_EQ(m_.calc_neg(0, 0), 0);
}

// Restart on crossing zero algorithm
// buffer=10  min=100  max=900  zero_output_on_min=true
// Documented example table in soyosource_virtual_meter.cpp.

class ZeroCrossTest : public ::testing::Test {
 protected:
  TestableSoyosourceVirtualMeter m_;
  void SetUp() override {
    m_.set_buffer(10);
    m_.set_min_power_demand(100);
    m_.set_max_power_demand(900);
    m_.set_zero_output_on_min_power_demand(true);
  }
};

TEST_F(ZeroCrossTest, PositiveConsumptionDelegatesToNeg) {
  // consumption=10, last=500 -> delegates to neg: (10-10)+500=500
  EXPECT_EQ(m_.calc_zero(10, 500), 500);
}

TEST_F(ZeroCrossTest, ZeroConsumptionReturnsZero) {
  // consumption=0 <= 0 -> 0 regardless of last
  EXPECT_EQ(m_.calc_zero(0, 500), 0);
}

TEST_F(ZeroCrossTest, NegativeConsumptionReturnsZero) {
  // consumption=-10 <= 0 -> 0
  EXPECT_EQ(m_.calc_zero(-10, 500), 0);
}

TEST_F(ZeroCrossTest, LargeNegativeAlsoReturnsZero) {
  // consumption=-690 <= 0 -> 0
  EXPECT_EQ(m_.calc_zero(-690, 500), 0);
}

TEST_F(ZeroCrossTest, HighPositiveClampedToMax) {
  // consumption=1010, last=500 -> delegates to neg -> clamped to 900
  EXPECT_EQ(m_.calc_zero(1010, 500), 900);
}

}  // namespace esphome::soyosource_virtual_meter::testing
