#include "Algorithm.h"
#include "FileHandler.h"
#include "StructuresManager.h"

#include <iostream>
#include <libds/heap_monitor.h>
#include <libds/amt/explicit_hierarchy.h>
#include <Windows.h>

using namespace ds::amt;
using namespace ds::adt;

//all commented code can be uncommented to run sequence structures
//do not forget to comment other StructureManager constructor and other FileHandler::loadData() :D

int main(int argc, const char** argv) 
{
    initHeapMonitor();
    SetConsoleCP(1250);
    SetConsoleOutputCP(1250);

    FileHandler file("CR.csv");
    StructuresManager manager(new IS<Region*>(), new IS<District*>(), new IS<Village*>(), new MultiWayEH<TerritorialUnit*>(),
        new HashTable<std::string, IS<TerritorialUnit*>*>(), new HashTable<std::string, IS<TerritorialUnit*>*>(), new HashTable<std::string, IS<TerritorialUnit*>*>());
    auto regions = manager.getRegions();
    auto districts = manager.getDistricts();
    auto villages = manager.getVillages();
	auto hierarchy = manager.getHierarchy();
    auto regTab = manager.getRegionTab();
    auto districtTab = manager.getDistrictTab();
    auto villageTab = manager.getVillageTab();

    file.loadData(*regions, *districts, *villages);
    manager.loadHierarchyFromSequence();
    manager.loadTables();

    
	std::cout << "*******************  WELCOME!  **************** \n";
    std::string option;
    auto currentNode = hierarchy->accessRoot();
    
	while (true) 
    {
        std::cout << "You are currently at: " << currentNode->data_->getTitle() << " [" << currentNode->data_->getType() << "] \n";
        std::cout << "-------------------------------------------\n";
        std::cout << "Choose operation [parent - move to superior unit, son - move to subordinate unit, \n"
					 "                  sw - startsWith, co - contains, ht - hasType, \n"
					 "                  search - search in table] or type 'end' to close the application:  \n";
        std::cin >> option;
        std::cout << "-------------------------------------------\n";
    
        if (option == "parent")
        {
            if (hierarchy->accessParent(*currentNode) != nullptr)
            {
                currentNode = hierarchy->accessParent(*currentNode);
            }
            else
            {
                std::cout << "Unit does not have parent! \n";
            }
            std::cout << '\n';
        }
        else if (option == "son")
        {
            if (currentNode->data_->getType() == "obec")
            {
                std::cout << "Unit does not have son! \n";
            }
            else 
            {
                size_t sonOrder;
                std::cout << "Choose number of unit you would like to go: \n";
                for (size_t i = 0; i < currentNode->sons_->size(); i++)
                {
                    std::cout << i + 1 << ". | " << currentNode->sons_->access(i)->data_->data_->toString();
                }
                std::cin >> sonOrder;
                sonOrder--;
                if (sonOrder >= 0 && sonOrder < currentNode->sons_->size())
                {
                    currentNode = hierarchy->accessSon(*currentNode, sonOrder);
                }
                else
                {
                    std::cout << "No such son! \n";
                }
            }
            std::cout << '\n';
        }

        else if (option == "search")
        {
            std::string title, type;
            std::cout << "Enter the title of territorial unit: \n";
            std::cin >> std::ws;
            getline(std::cin, title);
            std::cout << "-------------------------------------------\n";
            std::cout << "Enter the type of territorial unit: \n";
            std::cin >> type;
            std::cout << "-------------------------------------------\n";
            if (type == "kraj")
            {
                Algorithm::searchAndPrintTable(*regTab, title);
            }
            else if (type == "okres")
            {
                Algorithm::searchAndPrintTable(*districtTab, title);
            }
            else if (type == "obec")
            {
                Algorithm::searchAndPrintTable(*villageTab, title);
            }
            else
            {
                std::cout << "Unknown command! \n";
                std::cout << '\n';
            }
            std::cout << "--------------------------------------------\n";
            std::cout << '\n';
        }

        else if (option == "sw" || option == "co" || option == "ht") 
        {
            std::string substring;
            std::cout << "Enter substring: \n";
            std::cin >> substring;
            std::cout << "-------------------------------------------\n";

            IS<TerritorialUnit*> finalIS;
            if (option == "sw")
            {
                const std::function startsWith = [&](const TerritorialUnit* unit) -> bool
                    {
                        if (substring.size() > unit->getTitle().size())
                        {
                            return false;
                        }
                        const std::string lowerSubstring = Algorithm::stringToLower(substring);
                        std::string title = unit->getTitle();
                        const std::string titleLower = Algorithm::stringToLower(title);
						for (size_t i = 0; i < substring.size(); i++)
						{
							if (lowerSubstring.at(i) != titleLower.at(i))
							{
                                return false;
							}
						}
                        return true;
                    };
                // code from U1
                int seq;
                std::cout << "Do you want to load from sequence? [1 - yes] \n";
                std::cin >> seq;
                if (seq == 1)
                {
                    std::string type;
                    std::cout << "which type of unit you want to filter? \n";
                    std::cin >> type;
                    if (type == "obec")
                    {
                        Algorithm::filter(villages->begin(), villages->end(), finalIS, startsWith);
                    }
                    else if (type == "soorp")
                    {
                        Algorithm::filter(districts->begin(), districts->end(), finalIS, startsWith);
                    }
                    else if (type == "kraj")
                    {
                        Algorithm::filter(regions->begin(), regions->end(), finalIS, startsWith);
                    }
                }
                else 
                {
                    Algorithm::filter(Hierarchy<MWEHBlock<TerritorialUnit*>>::PreOrderHierarchyIterator(hierarchy, currentNode), hierarchy->end(), finalIS, startsWith);
                }
            }
            else if (option == "co")
            {
                const std::function contains = [&](const TerritorialUnit* unit) -> bool
                    {
                        std::string title = unit->getTitle();
                        return Algorithm::stringToLower(title).find(Algorithm::stringToLower(substring)) != std::string::npos;
                    };
                // code from U1
                int seq;
                std::cout << "Do you want to load from sequence? [1 - yes] \n";
                std::cin >> seq;
                if (seq == 1)
                {
                    std::string type;
                    std::cout << "which type of unit you want to filter? \n";
                    std::cin >> type;
                    if (type == "obec")
                    {
                        Algorithm::filter(villages->begin(), villages->end(), finalIS, contains);
                    }
                    else if (type == "soorp")
                    {
                        Algorithm::filter(districts->begin(), districts->end(), finalIS, contains);
                    }
                    else if (type == "kraj")
                    {
                        Algorithm::filter(regions->begin(), regions->end(), finalIS, contains);
                    }
                }
                else
                {
                    Algorithm::filter(Hierarchy<MWEHBlock<TerritorialUnit*>>::PreOrderHierarchyIterator(hierarchy, currentNode), hierarchy->end(), finalIS, contains);
                }
            }
            else
            {
                const std::function hasType = [&](const TerritorialUnit* unit) -> bool
                    {
                        return unit->getType() == Algorithm::stringToLower(substring);
                    };
                Algorithm::filter(Hierarchy<MWEHBlock<TerritorialUnit*>>::PreOrderHierarchyIterator(hierarchy, currentNode), hierarchy->end(), finalIS, hasType);
            }

            if (!finalIS.isEmpty())
            {
                int sort;
                std::cout << "Would you like to sort data? [1 - yes; any other number will not sort the data!]\n";
                std::cin >> sort;
                if (sort == 1)
                {
                    Algorithm::sort(finalIS);
                }
                
                for (const auto terUnit : finalIS)
                {
                    std::cout << terUnit->toString() << "[consonant count: " << Algorithm::getConsonantCount(terUnit->getTitle()) << "]" << "\n";
                }
            }
            else
            {
                std::cout << "No matching territorial units found! \n";
            }
            std::cout << '\n';
        }
        else if (option == "end")
        {
            std::cout << "Thanks for using our program, see you soon :) \n";
        	break;
        }
        else
        {
            std::cout << "Unknown command! \n";
            std::cout << '\n';
        }
    } 
    
    manager.deleteStructuresData();
	return 0;
}