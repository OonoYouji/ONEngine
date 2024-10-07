#pragma once

#include <string>

class BaseCommand {
public:

	BaseCommand() {}
	virtual ~BaseCommand() {}

	virtual void Execution(const std::string& args) = 0;

protected:

};