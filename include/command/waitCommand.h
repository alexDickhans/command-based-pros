#pragma once

#include "command.h"
#include "units/units.hpp"

/**
 * @brief Creates a \refitem Command with no requirements that finishes after a user-specified duration
 */
class WaitCommand : public Command {
	units::QTime startTime;
	units::QTime duration;
public:
	/**
	 * @brief Creates a new WaitCommand that runs for a user-specified duration
	 *
	 * @param duration The duration in QTime to run this \refitem Command
	 */
	explicit WaitCommand(const units::QTime &duration)
		: duration(duration) {
	}

	/**
	 * @brief Initializes the WaitCommand and sets the start time of the WaitCommand
	 */
	void initialize() override {
		startTime = pros::millis() * units::millisecond;
	}

	/**
	 * @brief Returns when the WaitCommand's duration has passed
	 *
	 * @return Returns true if the duration has passed, false otherwise
	 */
	bool isFinished() override {
		return pros::millis() * units::millisecond - startTime > duration;
	}

	~WaitCommand() override = default;
};

inline Command *Command::withTimeout(const units::QTime duration) {
	return new ParallelRaceGroup({new WaitCommand(duration), this});
}

