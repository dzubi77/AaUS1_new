#include "FileHandler.h"

// loading data for U1
void FileHandler::loadData(ds::amt::IS<Region*>& regions, ds::amt::IS<District*>& districts, ds::amt::IS<Village*>& villages)
{
    if (!input_.is_open())
    {
        std::cout << "Failed to open file!";
        return;
    }

    std::string row;
    std::string currentDistrict;
    bool regionCreated = false;

    while (getline(input_, row))
    {
        std::istringstream iss(row);
        std::string regionCode, districtCode, districtTitle, villageTitle, villageCode, villageType;
        std::string columnToIgnore;
        int area, population, popUnder14, popAbove65;
        char sewerage, waterSupply, gasSupply;

        getline(iss, regionCode, ',');
        getline(iss, districtCode, ',');
        getline(iss, districtTitle, ',');

        if (!regionCreated)
        {
            if (districtCode.empty())
            {
                regions.insertLast().data_ = new Region(districtCode, districtTitle);
                regionCreated = true;
                continue;
            }
        }
        else
        {
            regions.accessLast()->data_->setRegCode(regionCode);
            regionCreated = false;
        }

        getline(iss, columnToIgnore, ',');
        getline(iss, villageTitle, ',');
        getline(iss, villageCode, ',');
        getline(iss, villageType, ',');

        if (districtTitle != currentDistrict)
        {
            districts.insertLast().data_ = new District(districtCode, districtTitle, regionCode);
            currentDistrict = districtTitle;
        }

        // loading int variables
        std::string strVal;

        getline(iss, columnToIgnore, ',');
        getline(iss, columnToIgnore, ',');

        getline(iss, strVal, ',');
        strVal.erase(std::remove(strVal.begin(), strVal.end(), ' '), strVal.end());
        std::stringstream(strVal) >> area;
        getline(iss, strVal, ',');
        strVal.erase(std::remove(strVal.begin(), strVal.end(), ' '), strVal.end());
        std::stringstream(strVal) >> population;
        getline(iss, strVal, ',');
        strVal.erase(std::remove(strVal.begin(), strVal.end(), ' '), strVal.end());
        std::stringstream(strVal) >> popUnder14;
        getline(iss, strVal, ',');
        strVal.erase(std::remove(strVal.begin(), strVal.end(), ' '), strVal.end());
        std::stringstream(strVal) >> popAbove65;

        // loading char variables
        getline(iss, strVal, ',');
        std::stringstream(strVal) >> sewerage;
        getline(iss, strVal, ',');
        std::stringstream(strVal) >> waterSupply;
        getline(iss, strVal, ',');
        std::stringstream(strVal) >> gasSupply;
        villages.insertLast().data_ = new Village(villageCode, villageTitle, districtCode, villageType, area, population, popUnder14, popAbove65, sewerage, waterSupply, gasSupply);
    }

    input_.close();
}