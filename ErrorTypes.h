#pragma once
#include <string>

class exeption
{
private:
	std::string message;
public:
	exeption(std::string str) : message(str) {}
	std::string what() { return message; }
};