#pragma once

#include <string>

class TerritorialUnit
{
public:
	TerritorialUnit() = default;
	TerritorialUnit(std::string code, const std::string& title, std::string type) : code_(std::move(code)), title_(title), type_(std::move(type)) {}
	virtual ~TerritorialUnit() = default;

	virtual std::string toString() const
	{
		return code_ + " | " + title_ + '\n';
	}

	std::string getCode() const { return code_; }

	std::string getTitle() const
	{
		return title_;
	}

	std::string getType() const
	{
		return type_;
	}

protected:
	std::string code_, title_, type_;
};