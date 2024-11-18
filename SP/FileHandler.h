#pragma once

#include <string>
#include <fstream>

#include <libds/amt/implicit_sequence.h>
#include <libds/adt/table.h>
#include "Region.h"
#include "District.h"
#include "Village.h"

class FileHandler
{
public:
	FileHandler() = default;
	FileHandler(const std::string& fileName) : input_(fileName) {}

	void loadData(ds::amt::IS<Region*>& regions, ds::amt::IS<District*>& districts, ds::amt::IS<Village*>& villages);

private:
	std::ifstream input_;
};