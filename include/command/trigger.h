#pragma once

#include "commandScheduler.h"
#include "eventLoop.h"

#include "main.h"

/**
 * This class allows for easy triggering of Commands based on boolean inputs
 */
class Trigger {
private:
	std::function<bool()> condition;
	EventLoop *eventLoop;

public:
	/**
	 * @brief Create a Trigger with a specified condition and \refitem EventLoop
	 *
	 * @param condition The condition for the Trigger
	 * @param event_loop The \refitem EventLoop for the condition to run on
	 */
	Trigger(std::function<bool()> condition, EventLoop *event_loop) :
		condition(std::move(condition)), eventLoop(event_loop) {}

	/**
	 * @brief Create a Trigger with a specified condition and the default \refitem CommandScheduler \refitem EventLoop
	 *
	 * @param condition The condition for the Trigger
	 */
	explicit Trigger(std::function<bool()> condition) : condition(std::move(condition)) {
		eventLoop = CommandScheduler::getEventLoop();
	}

	/**
	 * @brief Binds a function to the \refitem EventLoop checks the condition to see if it has changed. Upon change the
	 * \refitem Command is scheduled
	 *
	 * @param command The command to run when the condition changes
	 * @return Trigger to allow for easy method chaining
	 */
	Trigger *onChange(Command *command) {
		eventLoop->bind([condition = condition, previous = condition(), command]() mutable {
			bool current = condition();

			if (previous != current) {
				command->schedule();
			}

			previous = current;
		});
		return this;
	}

	/**
	 * @brief Binds a function to the \refitem EventLoop checks the condition to see if it has changed from false to
	 * true. Upon change the \refitem Command is scheduled
	 *
	 * @param command The command to run when the condition changes from false to true
	 * @return Trigger to allow for easy method chaining
	 */
	Trigger *onTrue(Command *command) {
		eventLoop->bind([condition = condition, previous = condition(), command]() mutable {
			bool current = condition();

			if (!previous && current) {
				command->schedule();
			}

			previous = current;
		});
		return this;
	}

	/**
	 * @brief Binds a function to the \refitem EventLoop checks the condition to see if it has changed from true to
	 * false. Upon change the \refitem Command is scheduled
	 *
	 * @param command The command to run when the condition changes from true to false
	 * @return Trigger to allow for easy method chaining
	 */
	Trigger *onFalse(Command *command) {
		eventLoop->bind([condition = condition, previous = condition(), command]() mutable {
			bool current = condition();

			if (previous && !current) {
				command->schedule();
			}

			previous = current;
		});
		return this;
	}

	/**
	 * @brief Binds a function to the \refitem EventLoop checks the condition to see if it has changed from false to
	 * true. Upon change the \refitem Command is scheduled. When the Trigger detects the condition has changed from
	 * true to false, the Command is cancelled. It is not rescheduled when the command finishes (If this behavior is
	 * desired use a \refitem RepeatCommand)
	 *
	 * @param command The command to run when the condition is true
	 * @return Trigger to allow for easy method chaining
	 */
	Trigger *whileTrue(Command *command) {
		eventLoop->bind([condition = condition, previous = condition(), command]() mutable {
			bool current = condition();

			if (!previous && current) {
				command->schedule();
			} else if (previous && !current) {
				command->cancel();
			}

			previous = current;
		});
		return this;
	}

	/**
	 * @brief Binds a function to the \refitem EventLoop checks the condition to see if it has changed from true to
	 * false. Upon change the \refitem Command is scheduled. When the Trigger detects the condition has changed from
	 * false to true, the Command is cancelled. It is not rescheduled when the command finishes (If this behavior is
	 * desired use a \refitem RepeatCommand)
	 *
	 * @param command The command to run when the condition is false
	 * @return Trigger to allow for easy method chaining
	 */
	Trigger *whileFalse(Command *command) {
		eventLoop->bind([condition = condition, previous = condition(), command]() mutable {
			bool current = condition();

			if (previous && !current) {
				command->schedule();
			} else if (!previous && current) {
				command->cancel();
			}

			previous = current;
		});
		return this;
	}

	/**
	 * @brief Binds a function to the \refitem EventLoop checks the condition to see if it has changed from false to
	 * true. Upon change the \refitem Command is scheduled if it is not already scheduled, and cancelled otherwise
	 *
	 * @param command The command to toggle when the condition changes to true
	 * @return Trigger to allow for easy method chaining
	 */
	Trigger *toggleOnTrue(Command *command) {
		eventLoop->bind([condition = condition, previous = condition(), command]() mutable {
			bool current = condition();

			if (!previous && current) {
				if (command->scheduled()) {
					command->cancel();
				} else {
					command->schedule();
				}
			}

			previous = current;
		});
		return this;
	}

	/**
	 * @brief Binds a function to the \refitem EventLoop checks the condition to see if it has changed from true to
	 * false. Upon change the \refitem Command is scheduled if it is not already scheduled, and cancelled otherwise
	 *
	 * @param command The command to toggle when the condition changes to false
	 * @return Trigger to allow for easy method chaining
	 */
	Trigger *toggleOnFalse(Command *command) {
		eventLoop->bind([condition = condition, previous = condition(), command]() mutable {
			bool current = condition();

			if (previous && !current) {
				if (command->scheduled()) {
					command->cancel();
				} else {
					command->schedule();
				}
			}

			previous = current;
		});
		return this;
	}

	/**
     * @brief Binds a function to the \refitem EventLoop that checks if a command has completed and then schedules another command or performs an action
     *
     * @param command The command to check for completion
     * @param onCompleteCommand The command to run when the first command completes
     * @return Trigger to allow for easy method chaining
     */
    Trigger *onComplete(Command *command, Command *onCompleteCommand) {
        eventLoop->bind([command, onCompleteCommand]() mutable {
            if (command->isFinished()) {
                onCompleteCommand->schedule();
            }
        });
        return this;
    }

	/**
     * @brief Binds a function to the \refitem EventLoop that logs a string to the controller screen
     *
     * @param message The message to log
     * @return Trigger to allow for easy method chaining
     */
	template <typename... Args>
	Trigger *controllerDebugLog(const int16_t line, const char* message, Args... args) {
		eventLoop->bind([condition = condition, message]() mutable {
			assert(message <= 20 * sizeof(char)); // 20 is the maximum characters displayable on the controller
			pros::Controller master(pros::E_CONTROLLER_MASTER);

			master::clear_line(line);
			pros::delay(50); // Minimum refresh rate for VEXnet/bluetooth controller
			pros::lcd::print(line, message, args...);
		});
		return this;
	}

	// A version that defaults the line argument to 0
	template <typename... Args>
	Trigger *controllerDebugLog(const char* message, Args... args) {
		eventLoop->bind([condition = condition, message]() mutable {
			assert(message <= 20 * sizeof(char)); // 20 is the maximum characters displayable on the controller
			pros::Controller master(pros::E_CONTROLLER_MASTER);

			master::clear_line(0);
			pros::delay(50); // Minimum refresh rate for VEXnet/bluetooth controller
			pros::lcd::print(0, message, args...);
		});
		return this;
	}

	/**
     * @brief Binds a function to the \refitem EventLoop that logs a string to the brain screen
     *
     * @param message The message to log
     * @return Trigger to allow for easy method chaining
     */
	template <typename... Args>
    Trigger *brainDebugLog(const int16_t line, const char* message, Args... args) {
        eventLoop->bind([condition = condition, message]() mutable {
			pros::lcd::clear_line(line);
            pros::lcd::print(line, message, args...);
        });
        return this;
    }

	// A version that defaults the line argument to 0
	template <typename... Args>
    Trigger *brainDebugLog(const char* message, Args... args) {
        eventLoop->bind([condition = condition, message]() mutable {
			pros::lcd::clear_line(0);
            pros::lcd::print(0, message, args...);
        });
        return this;
    }
};
