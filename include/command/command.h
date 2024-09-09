#pragma once

#include "subsystem.h"
#include "units/units.hpp"

/**
 * @brief Enum for different cancel behaviors for Commands
 *
 * Cancel Incoming causes newly scheduled commands to fail, keeping the current command running until it ends
 * uninterrupted. Cancel Running caused the currently running command to yield to the newly scheduled command and end
 * interrupted.
 *
 */
enum class CommandCancelBehavior {
	CancelIncoming,
	CancelRunning,
};

/**
 * @brief Abstract Command class to be overriden for new commands
 *
 */
class Command {
public:
	/** @brief Called before every time the command is used. Users can override this to create starting behaviors for
	 * custom commands
	 *
	 *  @code
	 *	void initialize() override {
	 *		// Initialization tasks here, for example setting motor position PID commands
	 *		motor.move_absolute(5, 200);
	 *	}
	 *  @endcode
	 */
	virtual void initialize() {}
	virtual void execute() {}
	virtual bool isFinished() { return false; };
	virtual void end(bool interrupted){};
	virtual std::vector<Subsystem *> getRequirements() { return {}; };
	virtual CommandCancelBehavior getCancelBehavior() { return CommandCancelBehavior::CancelRunning; };

	void schedule();
	void cancel();
	[[nodiscard]] bool scheduled() const;

	Command *andThen(Command *other);
	Command *withTimeout(QTime duration);
	Command *until(const std::function<bool()> &isFinish);
	Command *with(Command *other);
	Command *race(Command *other);
	Command *repeatedly();
	Command *asProxy();

	virtual ~Command() = default;
};
