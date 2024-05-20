#pragma once
#include "TerritorialUnit.h"

class Village : public TerritorialUnit
{
public:
	Village() : TerritorialUnit(), area_(0), population_(0), popUnder14_(0), popAbove65_(0), sewerage_(' '), waterSupply_(' '), gasSupply_(' ') {}
	Village(std::string& code, const std::string& title, std::string& districtCode, const std::string& villageType, int area, int population, int popUnder14, int popAbove65, char sewerage, char waterSupply, char gasSupply)
		: TerritorialUnit(code, title, "obec"), districtCode_(districtCode), villageType_(villageType), area_(area), population_(population), popUnder14_(popUnder14), popAbove65_(popAbove65),
			sewerage_(sewerage), waterSupply_(waterSupply), gasSupply_(gasSupply) {}

	std::string toString() const override
	{
		return "\t \t" + code_ + " | " + title_ + " | " + villageType_ + " | " + std::to_string(area_) + " | " + std::to_string(population_) + " | " + std::to_string(popUnder14_)
			+ " | " + std::to_string(popAbove65_) + " | " + sewerage_ + " | " + waterSupply_ + " | " + gasSupply_ + '\n';
	}

	std::string getDistrictCode() const { return districtCode_; }

private:
	std::string districtCode_, villageType_;
	int area_, population_, popUnder14_, popAbove65_;
	char sewerage_, waterSupply_, gasSupply_;
};

