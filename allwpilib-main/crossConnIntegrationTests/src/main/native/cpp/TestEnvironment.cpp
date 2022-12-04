// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstdlib>
#include <thread>

#include <fmt/core.h>
#include <hal/HAL.h>

#include "gtest/gtest.h"
#include "mockds/MockDS.h"

using namespace std::chrono_literals;

class TestEnvironment : public testing::Environment {
  bool m_alreadySetUp = false;
  MockDS m_mockDS;

 public:
  TestEnvironment() {
    // Only set up once. This allows gtest_repeat to be used to automatically
    // repeat tests.
    if (m_alreadySetUp) {
      return;
    }
    m_alreadySetUp = true;

    if (!HAL_Initialize(500, 0)) {
      fmt::print(stderr, "FATAL ERROR: HAL could not be initialized\n");
      std::exit(-1);
    }

    m_mockDS.Start();

    // This sets up the network communications library to enable the driver
    // station. After starting network coms, it will loop until the driver
    // station returns that the robot is enabled, to ensure that tests will be
    // able to run on the hardware.
    HAL_ObserveUserProgramStarting();

    fmt::print("Started coms\n");

    int enableCounter = 0;

    auto checkEnabled = []() {
      HAL_ControlWord controlWord;
      std::memset(&controlWord, 0, sizeof(controlWord));
      HAL_GetControlWord(&controlWord);
      return controlWord.enabled && controlWord.dsAttached;
    };
    HAL_RefreshDSData();
    while (!checkEnabled()) {
      if (enableCounter > 50) {
        // Robot did not enable properly after 5 seconds.
        // Force exit
        fmt::print(stderr, " Failed to enable. Aborting\n");
        std::terminate();
      }

      std::this_thread::sleep_for(100ms);

      fmt::print("Waiting for enable: {}\n", enableCounter++);
      HAL_RefreshDSData();
    }
    std::this_thread::sleep_for(500ms);
  }

  ~TestEnvironment() override { m_mockDS.Stop(); }
};

testing::Environment* const environment =
    testing::AddGlobalTestEnvironment(new TestEnvironment);
