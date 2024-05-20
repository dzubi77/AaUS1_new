#pragma once
#include "TerritorialUnit.h"

class Region : public TerritorialUnit
{
public:
	Region() : TerritorialUnit() {}
	Region(std::string& code, const std::string& title) : TerritorialUnit(code, title, "kraj") {}

	void setRegCode(std::string& code)
	{
		code_ = code;
	}
};

