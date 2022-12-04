// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/BuiltInAccelerometerSim.h"  // NOLINT(build/include_order)

#include <hal/Accelerometer.h>
#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/BuiltInAccelerometer.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(AcclerometerSimTest, ActiveCallback) {
  HAL_Initialize(500, 0);

  BuiltInAccelerometerSim sim;

  sim.ResetData();

  bool wasTriggered = false;
  bool lastValue = false;

  auto cb = sim.RegisterActiveCallback(
      [&](std::string_view name, const HAL_Value* value) {
        wasTriggered = true;
        lastValue = value->data.v_boolean;
      },
      false);

  EXPECT_FALSE(wasTriggered);

  HAL_SetAccelerometerActive(true);

  EXPECT_TRUE(wasTriggered);
  EXPECT_TRUE(lastValue);
  EXPECT_TRUE(sim.GetActive());
}

TEST(AcclerometerSimTest, SetX) {
  HAL_Initialize(500, 0);
  BuiltInAccelerometerSim sim;
  sim.ResetData();

  DoubleCallback callback;
  constexpr double kTestValue = 1.91;

  BuiltInAccelerometer accel;
  auto cb = sim.RegisterXCallback(callback.GetCallback(), false);
  sim.SetX(kTestValue);
  EXPECT_EQ(kTestValue, accel.GetX());
  EXPECT_EQ(kTestValue, sim.GetX());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(kTestValue, callback.GetLastValue());
}

TEST(AcclerometerSimTest, SetY) {
  HAL_Initialize(500, 0);
  BuiltInAccelerometerSim sim;
  sim.ResetData();

  DoubleCallback callback;
  constexpr double kTestValue = 2.29;

  BuiltInAccelerometer accel;
  auto cb = sim.RegisterYCallback(callback.GetCallback(), false);
  sim.SetY(kTestValue);
  EXPECT_EQ(kTestValue, accel.GetY());
  EXPECT_EQ(kTestValue, sim.GetY());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(kTestValue, callback.GetLastValue());
}

TEST(AcclerometerSimTest, SetZ) {
  HAL_Initialize(500, 0);

  BuiltInAccelerometer accel;
  BuiltInAccelerometerSim sim(accel);
  sim.ResetData();

  DoubleCallback callback;
  constexpr double kTestValue = 3.405;

  auto cb = sim.RegisterZCallback(callback.GetCallback(), false);
  sim.SetZ(kTestValue);
  EXPECT_EQ(kTestValue, accel.GetZ());
  EXPECT_EQ(kTestValue, sim.GetZ());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(kTestValue, callback.GetLastValue());
}

TEST(AcclerometerSimTest, SetRange) {
  HAL_Initialize(500, 0);

  BuiltInAccelerometerSim sim;
  sim.ResetData();

  EnumCallback callback;

  Accelerometer::Range range = Accelerometer::kRange_4G;
  auto cb = sim.RegisterRangeCallback(callback.GetCallback(), false);
  BuiltInAccelerometer accel(range);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(static_cast<int>(range), sim.GetRange());
  EXPECT_EQ(static_cast<int>(range), callback.GetLastValue());

  // 2G
  callback.Reset();
  range = Accelerometer::kRange_2G;
  accel.SetRange(range);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(static_cast<int>(range), sim.GetRange());
  EXPECT_EQ(static_cast<int>(range), callback.GetLastValue());

  // 4G
  callback.Reset();
  range = Accelerometer::kRange_4G;
  accel.SetRange(range);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(static_cast<int>(range), sim.GetRange());
  EXPECT_EQ(static_cast<int>(range), callback.GetLastValue());

  // 8G
  callback.Reset();
  range = Accelerometer::kRange_8G;
  accel.SetRange(range);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(static_cast<int>(range), sim.GetRange());
  EXPECT_EQ(static_cast<int>(range), callback.GetLastValue());

  // 16G - Not supported
  callback.Reset();
  EXPECT_THROW(accel.SetRange(Accelerometer::kRange_16G), std::runtime_error);
  EXPECT_FALSE(callback.WasTriggered());
}
}  // namespace frc::sim
