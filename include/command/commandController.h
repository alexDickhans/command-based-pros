#pragma once

#include "commandScheduler.h"
#include "trigger.h"

/**
 * @brief Command Controller overrides pros::Controller and makes it better interface with Triggers and command based
 * code
 *
 * ```c
 * // Initialize CommandController
 * CommandController primary(pros::E_CONTROLLER_MASTER);
 *
 *
 *
 */
class CommandController : public pros::Controller {
public:
	/**
	 * @brief Construct a new CommandController
	 *
	 * @param id Master, Partner controller ID
	 */
	explicit CommandController(const pros::controller_id_e_t id) : Controller(id) {}

	/**
	 *
	Trigger *getTrigger(pros::controller_digital_e_t button) {
		return new Trigger([this, button]() { return this->get_digital(button); },
						   CommandScheduler::getTeleopEventLoop());
	}
};
