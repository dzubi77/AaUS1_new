#include "FileHandler.h"

/* loading data for U1
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
*/

void FileHandler::loadData(ds::amt::MultiWayEH<TerritorialUnit*>& hierarchy)
{
    if (!input_.is_open())
    {
        std::cout << "Failed to open file!";
        return;
    }

    hierarchy.emplaceRoot().data_ = new TerritorialUnit(" ", "Ceska republika", "krajina");

    std::string row;
    std::string currentDistrict;
    int regionOrder = 0;
    int districtOrder = 0;
    int villageOrder = 0;

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

        if (districtCode.empty())
        {
            hierarchy.emplaceSon(*hierarchy.accessRoot(), regionOrder).data_ = new Region(regionCode, districtTitle);
            regionOrder++;
            districtOrder = 0;
            villageOrder = 0;
            continue;
        }

        if (currentDistrict != districtTitle)
        {
            currentDistrict = districtTitle;
            hierarchy.emplaceSon(*hierarchy.accessSon(*hierarchy.accessRoot(), regionOrder - 1), districtOrder).data_ = new District(districtCode, districtTitle, regionCode);
            districtOrder++;
            villageOrder = 0;
            if (hierarchy.accessSon(*hierarchy.accessRoot(), regionOrder - 1)->data_->getCode().empty())
            {
                Region* region = dynamic_cast<Region*>(hierarchy.accessSon(*hierarchy.accessRoot(), regionOrder - 1)->data_);
                region->setRegCode(regionCode);
            }
        }

        getline(iss, columnToIgnore, ',');
        getline(iss, villageTitle, ',');
        getline(iss, villageCode, ',');
        getline(iss, villageType, ',');

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

        hierarchy.emplaceSon(*hierarchy.accessSon(*hierarchy.accessSon(*hierarchy.accessRoot(), regionOrder - 1), districtOrder - 1), villageOrder).data_
            = new Village(villageCode, villageTitle, districtCode, villageType, area, population, popUnder14, popAbove65, sewerage, waterSupply, gasSupply);
        villageOrder++;
    }

    input_.close();
}