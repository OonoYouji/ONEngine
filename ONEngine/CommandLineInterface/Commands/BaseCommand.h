#pragma once

#include <string>

class BaseCommand {
public:

	virtual ~BaseCommand() {}

	virtual void Execution(const std::string& args) = 0;

protected:

};