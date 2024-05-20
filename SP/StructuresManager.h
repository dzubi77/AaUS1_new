#pragma once

#include <libds/amt/implicit_sequence.h>
#include <libds/amt/explicit_hierarchy.h>
#include <libds/adt/table.h>

#include "TerritorialUnit.h"

class StructuresManager
{
public:
	/* Constructor U1
	StructuresManager(ds::amt::IS<Region*>* regions, ds::amt::IS<District*>* districts, ds::amt::IS<Village*>* villages, ds::amt::MultiWayEH<TerritorialUnit*>* hierarchy,
		ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* regTab, ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* districtTab, ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* villageTab)
		: regions_(regions), districts_(districts), villages_(villages), hierarchy_(hierarchy),
			regionTab_(regTab), districtTab_(districtTab), villageTab_(villageTab) {}
			*/

	StructuresManager(ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* regTab, ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* disTab, ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* vilTab,
		ds::amt::MultiWayEH<TerritorialUnit*>* hierarchy) : regionTab_(regTab), districtTab_(disTab), villageTab_(vilTab), hierarchy_(hierarchy) {}
	StructuresManager(const StructuresManager& other) = default;
	~StructuresManager()
	{
		/* Destructors for U1
		delete regions_;
		regions_ = nullptr;
		delete districts_;
		districts_ = nullptr;
		delete villages_;
		villages_ = nullptr;
		*/
		delete hierarchy_;
		hierarchy_ = nullptr;
		delete regionTab_;
		regionTab_ = nullptr;
		delete districtTab_;
		districtTab_ = nullptr;
		delete villageTab_;
		villageTab_ = nullptr;
	}
	/* Getters for U1
	ds::amt::IS<Region*>* getRegions() { return regions_; }
	ds::amt::IS<District*>* getDistricts() { return districts_; }
	ds::amt::IS<Village*>* getVillages() { return villages_; }
	*/
	ds::amt::MultiWayEH<TerritorialUnit*>* getHierarchy() { return hierarchy_; }
	ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* getRegionTab() { return regionTab_; }
	ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* getDistrictTab() { return districtTab_; }
	ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* getVillageTab() { return villageTab_; }

	void deleteStructuresData()
	{
		/* destructor of data for U1
		// delete sequences' data
		for (auto it = regions_->begin(); it != regions_->end(); ++it)
		{
			delete *it;
		}
		for (auto it = districts_->begin(); it != districts_->end(); ++it)
		{
			delete* it;
		}
		for (auto it = villages_->begin(); it != villages_->end(); ++it)
		{
			delete* it;
		}
		*/

		// delete hierarchy's data
		hierarchy_->processPostOrder(hierarchy_->accessRoot(), [&](const ds::amt::MWEHBlock<TerritorialUnit*>* unit)
			{
				delete unit->data_;
			});

		// delete tables' data
		for (auto it = villageTab_->begin(); it != villageTab_->end(); ++it)
		{
			auto item = *it;
			delete item.data_;
		}
		for (auto it = districtTab_->begin(); it != districtTab_->end(); ++it)
		{
			auto item = *it;
			delete item.data_;
		}
		for (auto it = regionTab_->begin(); it != regionTab_->end(); ++it)
		{
			auto item = *it;
			delete item.data_;
		}
	}

	/*
	void loadHierarchyFromSequence()
	{
		hierarchy_->emplaceRoot().data_ = new TerritorialUnit(" ", "Ceska republika", "krajina");
		auto root = hierarchy_->accessRoot();
		size_t indexLv1 = 0;
		size_t indexLv2 = 0;
		size_t indexLv3 = 0;
		for (auto it = regions_->begin(); it != regions_->end(); ++it)
		{
			const auto data = *it;
			hierarchy_->emplaceSon(*root, indexLv1).data_ = data;
			indexLv1++;
		}
		indexLv1 = 0;
		for (auto it = districts_->begin(); it != districts_->end(); ++it)
		{
			const auto data = *it;
			auto parent = hierarchy_->accessSon(*root, indexLv1);
			if (parent->data_->getCode() == data->getRegionCode())
			{
				hierarchy_->emplaceSon(*parent, indexLv2).data_ = data;
			}
			else
			{
				indexLv2 = 0;
				indexLv1++;
				parent = hierarchy_->accessSon(*root, indexLv1);
				hierarchy_->emplaceSon(*parent, indexLv2).data_ = data;
			}
			indexLv2++;
		}
		indexLv1 = 0;
		indexLv2 = 0;
		for (auto it = villages_->begin(); it != villages_->end(); ++it)
		{
			const auto data = *it;
			auto parent = hierarchy_->accessSon(*hierarchy_->accessSon(*root, indexLv1), indexLv2);
			if (data->getDistrictCode() == parent->data_->getCode())
			{
				hierarchy_->emplaceSon(*parent, indexLv3).data_ = data;
			}
			else
			{
				indexLv2++;
				parent = hierarchy_->accessSon(*hierarchy_->accessSon(*root, indexLv1), indexLv2);
				if (parent == nullptr)
				{
					indexLv2 = 0;
					indexLv1++;
					parent = hierarchy_->accessSon(*hierarchy_->accessSon(*root, indexLv1), indexLv2);
				}
				indexLv3 = 0;
				hierarchy_->emplaceSon(*parent, indexLv3).data_ = data;
			}
			indexLv3++;
		}
	}
	*/

	void loadTables()
	{
		hierarchy_->processPostOrder(hierarchy_->accessRoot(), [&](const ds::amt::MWEHBlock<TerritorialUnit*>* unit)
			{
				if (unit->data_->getType() == "obec")
				{
					if (!villageTab_->contains(unit->data_->getTitle()))
					{
						auto tabData = new ds::amt::IS<TerritorialUnit*>();
						tabData->insertLast().data_ = unit->data_;
						villageTab_->insert(unit->data_->getTitle(), tabData);
					}
					else
					{
						auto sequence = villageTab_->find(unit->data_->getTitle());
						sequence->insertLast().data_ = unit->data_;
					}
				}
				else if (unit->data_->getType() == "okres")
				{
					if (!districtTab_->contains(unit->data_->getTitle()))
					{
						auto tabData = new ds::amt::IS<TerritorialUnit*>();
						tabData->insertLast().data_ = unit->data_;
						districtTab_->insert(unit->data_->getTitle(), tabData);
					}
					else
					{
						auto sequence = districtTab_->find(unit->data_->getTitle());
						sequence->insertLast().data_ = unit->data_;
					}
				}
				else
				{
					if (!regionTab_->contains(unit->data_->getTitle()))
					{
						auto tabData = new ds::amt::IS<TerritorialUnit*>();
						tabData->insertLast().data_ = unit->data_;
						regionTab_->insert(unit->data_->getTitle(), tabData);
					}
					else
					{
						auto sequence = regionTab_->find(unit->data_->getTitle());
						sequence->insertLast().data_ = unit->data_;
					}
				}
			});
	}
	
private:
	// structures for U1
	//ds::amt::IS<Region*>* regions_;
	//ds::amt::IS<District*>* districts_;
	//ds::amt::IS<Village*>* villages_;

	ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* regionTab_;
	ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* districtTab_;
	ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>* villageTab_;

	ds::amt::MultiWayEH<TerritorialUnit*>* hierarchy_;
};
