// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.wpilibj.GenericHID;
import edu.wpi.first.wpilibj.event.EventLoop;
import edu.wpi.first.wpilibj2.command.CommandScheduler;

/**
 * A version of {@link GenericHID} with {@link Trigger} factories for command-based.
 *
 * @see GenericHID
 */
public class CommandGenericHID {
  private final GenericHID m_hid;

  /**
   * Construct an instance of a device.
   *
   * @param port The port index on the Driver Station that the device is plugged into.
   */
  public CommandGenericHID(int port) {
    m_hid = new GenericHID(port);
  }

  /**
   * Get the underlying GenericHID object.
   *
   * @return the wrapped GenericHID object
   */
  public GenericHID getHID() {
    return m_hid;
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @return an event instance representing the button's digital signal attached to the {@link
   *     CommandScheduler#getDefaultButtonLoop() default scheduler button loop}.
   * @see #button(int, EventLoop)
   */
  public Trigger button(int button) {
    return this.button(button, CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs an event instance around this button's digital signal.
   *
   * @param button the button index
   * @param loop the event loop instance to attach the event to.
   * @return an event instance representing the button's digital signal attached to the given loop.
   */
  public Trigger button(int button, EventLoop loop) {
    return new Trigger(loop, () -> m_hid.getRawButton(button));
  }

  /**
   * Constructs a Trigger instance based around this angle of the default (index 0) POV on the HID,
   * attached to {@link CommandScheduler#getDefaultButtonLoop() the default command scheduler button
   * loop}.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (eg right is 90,
   * upper-left is 315).
   *
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(int angle) {
    return pov(0, angle, CommandScheduler.getInstance().getDefaultButtonLoop());
  }

  /**
   * Constructs a Trigger instance based around this angle of a POV on the HID.
   *
   * <p>The POV angles start at 0 in the up direction, and increase clockwise (eg right is 90,
   * upper-left is 315).
   *
   * @param pov index of the POV to read (starting at 0). Defaults to 0.
   * @param angle POV angle in degrees, or -1 for the center / not pressed.
   * @param loop the event loop instance to attach the event to. Defaults to {@link
   *     CommandScheduler#getDefaultButtonLoop() the default command scheduler button loop}.
   * @return a Trigger instance based around this angle of a POV on the HID.
   */
  public Trigger pov(int pov, int angle, EventLoop loop) {
    return new Trigger(loop, () -> m_hid.getPOV(pov) == angle);
  }

  /**
   * Constructs a Trigger instance based around the 0-degree angle (up) of the default (index 0) POV
   * on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 0-degree angle of a POV on the HID.
   */
  public Trigger povUp() {
    return pov(0);
  }

  /**
   * Constructs a Trigger instance based around the 45-degree angle (right up) of the default (index
   * 0) POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the 45-degree angle of a POV on the HID.
   */
  public Trigger povUpRight() {
    return pov(45);
  }

  /**
   * Constructs a Trigger instance based around the 90-degree angle (right) of the default (index 0)
   * POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 90-degree angle of a POV on the HID.
   */
  public Trigger povRight() {
    return pov(90);
  }

  /**
   * Constructs a Trigger instance based around the 135-degree angle (right down) of the default
   * (index 0) POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the
   * default command scheduler button loop}.
   *
   * @return a Trigger instance based around the 135-degree angle of a POV on the HID.
   */
  public Trigger povDownRight() {
    return pov(135);
  }

  /**
   * Constructs a Trigger instance based around the 180-degree angle (down) of the default (index 0)
   * POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 180-degree angle of a POV on the HID.
   */
  public Trigger povDown() {
    return pov(180);
  }

  /**
   * Constructs a Trigger instance based around the 45-degree angle (down left) of the default
   * (index 0) POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the
   * default command scheduler button loop}.
   *
   * @return a Trigger instance based around the 225-degree angle of a POV on the HID.
   */
  public Trigger povDownLeft() {
    return pov(225);
  }

  /**
   * Constructs a Trigger instance based around the 270-degree angle (left) of the default (index 0)
   * POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the 270-degree angle of a POV on the HID.
   */
  public Trigger povLeft() {
    return pov(270);
  }

  /**
   * Constructs a Trigger instance based around the 315-degree angle (left up) of the default (index
   * 0) POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default
   * command scheduler button loop}.
   *
   * @return a Trigger instance based around the 315-degree angle of a POV on the HID.
   */
  public Trigger povUpLeft() {
    return pov(315);
  }

  /**
   * Constructs a Trigger instance based around the center (not pressed) of the default (index 0)
   * POV on the HID, attached to {@link CommandScheduler#getDefaultButtonLoop() the default command
   * scheduler button loop}.
   *
   * @return a Trigger instance based around the center of a POV on the HID.
   */
  public Trigger povCenter() {
    return pov(-1);
  }

  /**
   * Get the value of the axis.
   *
   * @param axis The axis to read, starting at 0.
   * @return The value of the axis.
   */
  public double getRawAxis(int axis) {
    return m_hid.getRawAxis(axis);
  }
}
