// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.PWM;

/**
 * Cross the Road Electronics (CTRE) Talon and Talon SR Motor Controller.
 *
 * <p>Note that the Talon uses the following bounds for PWM values. These values should work
 * reasonably well for most controllers, but if users experience issues such as asymmetric behavior
 * around the deadband or inability to saturate the controller in either direction, calibration is
 * recommended. The calibration procedure can be found in the Talon User Manual available from CTRE.
 *
 * <ul>
 *   <li>2.037ms = full "forward"
 *   <li>1.539ms = the "high end" of the deadband range
 *   <li>1.513ms = center of the deadband range (off)
 *   <li>1.487ms = the "low end" of the deadband range
 *   <li>0.989ms = full "reverse"
 * </ul>
 */
public class Talon extends PWMMotorController {
  /**
   * Constructor for a Talon (original or Talon SR).
   *
   * @param channel The PWM channel that the Talon is attached to. 0-9 are on-board, 10-19 are on
   *     the MXP port
   */
  public Talon(final int channel) {
    super("Talon", channel);

    m_pwm.setBounds(2.037, 1.539, 1.513, 1.487, 0.989);
    m_pwm.setPeriodMultiplier(PWM.PeriodMultiplier.k1X);
    m_pwm.setSpeed(0.0);
    m_pwm.setZeroLatch();

    HAL.report(tResourceType.kResourceType_Talon, getChannel() + 1);
  }
}
