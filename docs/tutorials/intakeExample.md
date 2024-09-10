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

## main.h

In main.h we have to add the necessary includes to the environment. For this example you will want to include the
subsystem you just made. In our project this looks like the following.

```c++
#include "subsystems/intake.h"
```

## main.cpp

First we have to make a CommandController object for easier Trigger creation, and an Intake pointer to store the Intake
subsystem instantiation.

```c++
CommandController primary(pros::E_CONTROLLER_MASTER);

Intake *intake;
```

To actually run the intake code we need to first run the CommandScheduler. We suggest you do this by creating this task
that runs the necessary CommandScheduler::run() every 10ms.

```c++
/**
 * @brief This function runs the update scheduler at each frame with a consistent schedule
 *
 * @warning This function or alternative similar to it must be running to ensure the \refitem CommandScheduler is run
 */
[[noreturn]] void update_loop() {
	// Loop forever
	while (true) {
		// Store the start time
		auto start_time = pros::millis();

		// Run the command scheduler
		// This might be an expensive(Time wise) computation
		CommandScheduler::run();

		// Use delay until if this computation ends up being expensive, keeping loop time in check
		pros::c::task_delay_until(&start_time, 10);
	}
}
```

Next, in our initialize function we have to start the `update_loop` function. To do this we start it with a task to run
it asynchronously from the main code

```c++
void initialize() {
	// Start the command scheduler task
	pros::Task commandSchedulerTask(update_loop);
```

Then we have to initialize and register the Subsystem with the CommandScheduler, to register command we have to also set
a default command, in this case it stops the intake:

```c++
// Create a new intake object and store it in the global intake
intake = new Intake(pros::Motor(1));

CommandScheduler::registerSubsystem(intake, intake->pctCommand(0.0));
```

Then we have to define the triggers. These triggers make the intake move forwards and backwards on command.

```c++
// Set pctCommand to run while R1 is true
primary.getTrigger(DIGITAL_R1)->whileTrue(intake->pctCommand(-1.0));

// Toggle pctCommand to run while R1 turns to ture
primary.getTrigger(DIGITAL_R2)->toggleOnTrue(intake->pctCommand(1.0));
```

Create a command that dejams the intake by moving it back and forth repeatedly using the Command composition sequence
with the command shorthand form.

```c++
// Dejam mode, causes the intake to move back and forth quickly
primary.getTrigger(DIGITAL_A)->whileTrue(intake->pctCommand(-1.0)
                                                    ->withTimeout(300_ms)
                                                    ->andThen(intake->pctCommand(1.0)
                                                        ->withTimeout(300_ms))
                                                    ->repeatedly());
```

And there we go! This is a fully functional intake subsystem that allows complex commands such as dejam and in the
future color sorting to be done much easier than with other solutions. 
