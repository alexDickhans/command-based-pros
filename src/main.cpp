#include "main.h"

CommandController primary(pros::E_CONTROLLER_MASTER);

Intake *intake;

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

void initialize() {
	// Start the command scheduler task
	pros::Task commandSchedulerTask(update_loop);

	// Create a new intake object and store it in the global intake
	intake = new Intake(pros::Motor(1));

	CommandScheduler::registerSubsystem(intake, intake->pctCommand(0.0));

	// Set pctCommand to run while R1 is true
	primary.getTrigger(DIGITAL_R1)->whileTrue(intake->pctCommand(-1.0)
													->logToController(primary, "Intake on at %i", static_cast<int>(-1.0 * 100))
											 );

	// Toggle pctCommand to run while R1 turns to true
	primary.getTrigger(DIGITAL_R2)->toggleOnTrue(intake->pctCommand(1.0)
													   ->logToBrain("Intake toggled on at %i percent speed", static_cast<int>(1.0 * 100))
												);

	// Dejam mode, causes the intake to move back and forth quickly
	primary.getTrigger(DIGITAL_A)->whileTrue(intake->pctCommand(-1.0)
														->withTimeout(300_ms)
														->andThen(intake->pctCommand(1.0)
														->withTimeout(300_ms))
														->repeatedly());
}

void disabled() {}

void competition_initialize() {}

void autonomous() {}

void opcontrol() {}
