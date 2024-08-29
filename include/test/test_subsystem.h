#pragma once

#include "command/subsystem.h"

class TestSubsystem : public Subsystem {
public:
	TestSubsystem() = default;
	void periodic() override {
		std::cout << "periodic" << std::endl;
	}

	void print(std::string message) {
		printf("Message: %s\n", message.c_str());
	}
};