#pragma once

#include <string>

#include "command.h"
#include "main.h"

/**
 * @brief A command that logs a message to the brain's LCD screen
 */
class LogBrainCommand : public InstantCommand {
private:
    std::string message;

public:
    /**
     * @brief Constructs a new LogBrainCommand
     *
     * @param message The message to log to the brain's LCD screen
     * @param args The arguments to format the message with
     */
    template <class ...Args>
    LogBrainCommand(std::string message, Args... args)
        : InstantCommand([&]() {
                                pros::lcd::clear_line(0);
                                pros::lcd::print(0, message.c_str(), args...);},
                                {}) {}

    /**
     * @brief Returns the requirements of this command
     *
     * @return An empty vector as this command has no requirements
     */
    std::vector<Subsystem *> getRequirements() override {
        return {};
    }

    /**
     * @brief Destructor for LogBrainCommand
     */
    ~LogBrainCommand() override = default;
};

/**
 * @brief Logs a message to the brain's LCD screen
 *
 * @param message The message to log
 * @param args The arguments to format the message with
 * 
 * @return A pointer to the new LogBrainCommand
 */
template <class ...Args>
inline Command *Command::logToBrain(std::string message, Args... args) {
    return new LogBrainCommand(message, args...);
}

/**
 * @brief A command that logs a message to the controller's screen
 */
class LogControllerCommand : public InstantCommand {
private:
    std::string message;

public:
    /**
     * @brief Constructs a new LogControllerCommand
     *
     * @param controller The controller to log to
     * @param message The message to log to the controller's screen
     * @param args The arguments to format the message with
     */
    template <class ...Args>
    LogControllerCommand(pros::Controller& controller, std::string message, Args... args)
        : InstantCommand([&]() {
                                assert(message.length() <= 20);
                                controller.clear();
                                pros::delay(50); // Minimum VEXnet/bluetooth refresh rate
                                controller.print(0, 0, message.c_str(), args...);},
                                {}) {}

    /**
     * @brief Returns the requirements of this command
     *
     * @return An empty vector as this command has no requirements
     */
    std::vector<Subsystem *> getRequirements() override {
        return {};
    }

    /**
     * @brief Destructor for LogControllerCommand
     */
    ~LogControllerCommand() override = default;
};

/**
     * @brief Logs a message to the controller's screen
     *
     * @param controller The controller to log to
     * @param message The message to log
     * @param args The arguments to format the message with
     * 
     * @return \refitem LogControllerCommand that logs the message
     */
template <class ...Args>
inline Command *Command::logToController(pros::Controller& controller, std::string message, Args... args) {
    return new LogControllerCommand(controller, message, args...);
}