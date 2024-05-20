#pragma once
#include "TerritorialUnit.h"

class District : public TerritorialUnit
{
public:
	District() : TerritorialUnit() {}
	District(std::string& districtCode, const std::string& districtTitle, std::string& regionCode) : TerritorialUnit(districtCode, districtTitle, "okres"), regionCode_(regionCode) {}

	std::string getRegionCode() const { return regionCode_; }

	std::string toString() const override
	{
		return '\t' + code_ + " | " + title_ + '\n';
	}

private:
	std::string regionCode_;
};

