#pragma once

class TestCommand : public Command {
	private:
	TestSubsystem* subsystem;
	public:
	TestCommand(TestSubsystem* test_subsystem ) : subsystem(test_subsystem) {
	}

	void initialize() final {
		subsystem->print("Initialize");
	}

	void execute() final {
		subsystem->print("Execute");
	}

	void end(bool interrupted) final {
		subsystem->print("end");
	}

	std::vector<Subsystem *> getRequirements() final {
		return {subsystem};
	}

	~TestCommand() override = default;
};
