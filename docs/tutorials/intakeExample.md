# Annotated Intake Example

In this example we will make an Intake subsystem and applicable intake commands. All example code is located in
the [examples](https://github.com/alexDickhans/command-based-pros/tree/main/include/example) folder.

## Example Prerequisites

For this example you need to understand Object Oriented Programming, but more specifically, Classes, Member functions,
and Inheritance. If you need to learn
the [learncpp.com](https://www.learncpp.com/cpp-tutorial/introduction-to-object-oriented-programming/) articles.
Additionally you need to have an understanding of how anonymous functions, or lambda expressions work for the RunCommand
implementation.

## Project Structure

To organize your repository we recommend that you put all subsystems inside their own folder in your include directory.
This becomes very helpful later when there are many subsystems and commands in the same project. An example project tree
is below:

* include
    * subsystems
        * intake.h
        * otherSubsystem1.h
        * otherSubsystem2.h
    * commands
        * complexIntakeCommand.h
        * otherCommand1.h
    * api.h
    * main.h

## Subsystem Code

We will start by making the Intake Subsystem Implementation. We will make a file named intake.h and put the following
code into the file to include the necessary Classes

```c++
#pragma once

#include "command/subsystem.h"
#include "command/runCommand.h"
```

Next we will create the start of the Intake subsystem class. It is important to use the colon override operator, with
the public specifier to ensure that you are subclassing Subsystem correctly.

```c++
/**
 * Intake subsystem
 *
 * Allows controlling the percentage power of the intake subsystem
 */
class Intake : public Subsystem {
```

We then define the all resources that this Subsystem needs to control. For the Intake we only need the intake motor for
this subsystem.

```c++
private:
	/**
	 * Store the necessary resources for this subsystem in the private section. We will control these resources in
	 * member methods
	 */
	pros::Motor intakeMotor;
```

Next we have to create a constructor for this class. This will initialize all the resources we need for this subsystem.
We will call this later to create an instance of the subsystem.

```c++
public:
    /**
    * Construct a new Intake subsystem with pro::Motor object
    *
    * @param intake_motor
    */
    explicit Intake(pros::Motor intake_motor)
    : intakeMotor(std::move(intake_motor)) {
    }
```

Next we have to define a periodic function for this Subsystem. This function is not especially useful for intakes, but
for subsystems such as flywheels this can be important to run stuff such as PID loops. In this example we use the
periodic to print out debugging information for the intake.

```c++
/**
 * @brief Periodic runs every frame (10ms) by the command scheduler after the subsystem is registered.
 *
 * @note Periodic is very useful tasks such as PID that need to run every frame, but the target is only set once.
 */
void periodic() override {
    // EX: debugging tasks
    std::cout << "Intake speed: " << intakeMotor.get_actual_velocity() << std::endl;

    // Also:
    // Updating PID for something like a flywheel or odometry for a drivetrain subsystem
}
```

This is an example of a member function that sets the intake percentage of the motor. We can use this in commands later
to set the percentage voltage of the intake.

```c++
// Create member functions to actuate the subsystem
// EX: move the intake at a certain (signed) percentage of voltage.
/**
 * @brief This command moves the intake at a certain voltage potential requested as a percentage [-1, 1]
 *
 * @param pct Percent voltage for the intake in the range [-1, 1]
 */
void setPct(const double pct) {

    // Scale [-1, 1] to [-12000 mV, 12000 mV]
    this->intakeMotor.move_voltage(static_cast<std::int32_t>(pct * 12000.0));
}
```

This command creates a new RunCommand Pointer with the `new RunCommand(fn, requirements)` constructor. This will allow
us to easily create commands for the intake down the line. Breaking down this function, we return the RunCommand we
create, with the function being setting the intake percentage. We then add {this} to the requirements, this ensures the
subsystem pointer(this) is added to the requirements for this subsystem to stop multiple commands from running on the
intake.

```c++
/**
 * @brief Example way to create commands from class
 * This is very useful for simple commands or default commands
 *
 * @param pct Percentage Desired for this command
 * @return Command pointer to a RunCommand that moves the intake
 */
RunCommand* pctCommand(const double pct) {
    // Create a new RunCommand
    // The lambda body is called at every update, in this case setting the intake percentage
    return new RunCommand(
        [this, pct] () // Capture "this" and the percentage request
        {
            this->setPct(pct); // Set the percentage of the intake to the request
        },
        {this} // Add "this", the pointer to this subsystem that is currently running.
               // It is important to ensure that all subsystems that are being utilized in a command are properly
               // Freed to allow that command to run.
        );
}
```

To end off the class we define the destructor for this class. In the RAII design principle this function would free all
the memory it used, however because we only used a Motor object in this class it is automatically freed for us.

```c++
    // Free any additional resources that are needed.
	~Intake() override = default;

};
```
