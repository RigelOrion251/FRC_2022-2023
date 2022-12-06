// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Counter.h"  // NOLINT(build/include_order)

#include <units/time.h>

#include "TestBench.h"
#include "frc/Timer.h"
#include "frc/motorcontrol/Jaguar.h"
#include "frc/motorcontrol/Talon.h"
#include "frc/motorcontrol/Victor.h"
#include "gtest/gtest.h"

static constexpr auto kMotorDelay = 2.5_s;

static constexpr auto kMaxPeriod = 2_s;

class CounterTest : public testing::Test {
 protected:
  frc::Counter m_talonCounter{TestBench::kTalonEncoderChannelA};
  frc::Counter m_victorCounter{TestBench::kVictorEncoderChannelA};
  frc::Counter m_jaguarCounter{TestBench::kJaguarEncoderChannelA};
  frc::Talon m_talon{TestBench::kVictorChannel};
  frc::Victor m_victor{TestBench::kTalonChannel};
  frc::Jaguar m_jaguar{TestBench::kJaguarChannel};

  void Reset() {
    m_talonCounter.Reset();
    m_victorCounter.Reset();
    m_jaguarCounter.Reset();
    m_talon.Set(0.0);
    m_victor.Set(0.0);
    m_jaguar.Set(0.0);
  }
};

/**
 * Tests the counter by moving the motor and determining if the
 * counter is counting.
 */
TEST_F(CounterTest, CountTalon) {
  Reset();

  /* Run the motor forward and determine if the counter is counting. */
  m_talon.Set(1.0);
  frc::Wait(0.5_s);

  EXPECT_NE(0.0, m_talonCounter.Get()) << "The counter did not count (talon)";

  /* Set the motor to 0 and determine if the counter resets to 0. */
  m_talon.Set(0.0);
  frc::Wait(0.5_s);
  m_talonCounter.Reset();

  EXPECT_FLOAT_EQ(0.0, m_talonCounter.Get())
      << "The counter did not restart to 0 (talon)";
}

TEST_F(CounterTest, CountVictor) {
  Reset();

  /* Run the motor forward and determine if the counter is counting. */
  m_victor.Set(1.0);
  frc::Wait(0.5_s);

  EXPECT_NE(0.0, m_victorCounter.Get()) << "The counter did not count (victor)";

  /* Set the motor to 0 and determine if the counter resets to 0. */
  m_victor.Set(0.0);
  frc::Wait(0.5_s);
  m_victorCounter.Reset();

  EXPECT_FLOAT_EQ(0.0, m_victorCounter.Get())
      << "The counter did not restart to 0 (jaguar)";
}

TEST_F(CounterTest, CountJaguar) {
  Reset();

  /* Run the motor forward and determine if the counter is counting. */
  m_jaguar.Set(1.0);
  frc::Wait(0.5_s);

  EXPECT_NE(0.0, m_jaguarCounter.Get()) << "The counter did not count (jaguar)";

  /* Set the motor to 0 and determine if the counter resets to 0. */
  m_jaguar.Set(0.0);
  frc::Wait(0.5_s);
  m_jaguarCounter.Reset();

  EXPECT_FLOAT_EQ(0.0, m_jaguarCounter.Get())
      << "The counter did not restart to 0 (jaguar)";
}

/**
 * Tests the GetStopped and SetMaxPeriod methods by setting the Max Period and
 * getting the value after a period of time.
 */
TEST_F(CounterTest, TalonGetStopped) {
  Reset();

  /* Set the Max Period of the counter and run the motor */
  m_talonCounter.SetMaxPeriod(kMaxPeriod);
  m_talon.Set(1.0);
  frc::Wait(0.5_s);

  EXPECT_FALSE(m_talonCounter.GetStopped()) << "The counter did not count.";

  /* Stop the motor and wait until the Max Period is exceeded */
  m_talon.Set(0.0);
  frc::Wait(kMotorDelay);

  EXPECT_TRUE(m_talonCounter.GetStopped())
      << "The counter did not stop counting.";
}

TEST_F(CounterTest, VictorGetStopped) {
  Reset();

  /* Set the Max Period of the counter and run the motor */
  m_victorCounter.SetMaxPeriod(kMaxPeriod);
  m_victor.Set(1.0);
  frc::Wait(0.5_s);

  EXPECT_FALSE(m_victorCounter.GetStopped()) << "The counter did not count.";

  /* Stop the motor and wait until the Max Period is exceeded */
  m_victor.Set(0.0);
  frc::Wait(kMotorDelay);

  EXPECT_TRUE(m_victorCounter.GetStopped())
      << "The counter did not stop counting.";
}

TEST_F(CounterTest, JaguarGetStopped) {
  Reset();

  /* Set the Max Period of the counter and run the motor */
  m_jaguarCounter.SetMaxPeriod(kMaxPeriod);
  m_jaguar.Set(1.0);
  frc::Wait(0.5_s);

  EXPECT_FALSE(m_jaguarCounter.GetStopped()) << "The counter did not count.";

  /* Stop the motor and wait until the Max Period is exceeded */
  m_jaguar.Set(0.0);
  frc::Wait(kMotorDelay);

  EXPECT_TRUE(m_jaguarCounter.GetStopped())
      << "The counter did not stop counting.";
}
