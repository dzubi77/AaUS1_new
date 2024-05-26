#pragma once

#include <functional>
#include <string>

#include "TerritorialUnit.h"
#include <libds/amt/implicit_sequence.h>
#include <libds/adt/table.h>
#include <libds/adt/sorts.h>

const std::string CONSONANTS = "BbCcÈèDdÏïFfGgHhJjKkLl¼¾MmNnÒòPpQqRrØøSsŠšTtVvWwXxZz";
const std::string ALL_LETTERS = "AaÁáBbCcÈèDdÏïEeÉéÉìFfGgHhIiÍíJjKkLl¼¾MmNnÒòOoÓóPpQqRrØøSsŠšTtUuÚÚVvWwXxİıZz";

class Algorithm
{
public:
	template<typename IteratorType, typename T>
	static void filter(IteratorType begin, IteratorType end, ds::amt::IS<TerritorialUnit*>& finalStructure, const std::function<bool(const T*)>& predicate)
	{
		for (auto it = begin; it != end; ++it)
		{
			if (predicate(*it))
			{
				finalStructure.insertLast().data_ = *it;
			}
		} 
	}

	static void searchAndPrintTable(ds::adt::HashTable<std::string, ds::amt::IS<TerritorialUnit*>*>& table, const std::string& title)
	{
		if (table.contains(title))
		{
			auto data = table.find(title);
			for (auto it = data->begin(); it != data->end(); ++it)
			{
				auto element = *it;
				std::cout << element->toString();
			}
		}
		else
		{
			std::cout << "Table does not contain such unit! \n";
		}
	}

	static void sort(ds::amt::IS<TerritorialUnit*>& is)
	{
		ds::adt::QuickSort<TerritorialUnit*> sort;
		int input;
		std::cout << "How would like to sort data? \n";
		std::cout << "1) alphabetically \n2) according to consonant count \n";
		std::cin >> input;
		while (input < 1 || input > 3)
		{
			std::cout << "Wrong input! \n";
			std::cout << "How would like to sort data? \n";
			std::cout << "1) alphabetically \n2) according to consonant count \n";
			std::cin >> input;
		}
		if (input == 1)
		{
			sort.sort(is, [&](const TerritorialUnit* a, const TerritorialUnit* b) -> bool
				{
					return compareStrings(a->getTitle(), b->getTitle());
				});
		}
		else
		{
			sort.sort(is, [&](const TerritorialUnit* a, const TerritorialUnit* b) -> bool
				{
					return getConsonantCount(a->getTitle()) < getConsonantCount(b->getTitle());
				});
		}
	}

	static std::string stringToLower(std::string& str)
	{
		for (char& c : str)
		{
			c = std::tolower(c);
		}
		return str;
	}

	static int getConsonantCount(const std::string& str)
	{
		int count = 0;
		for (const char& c : str)
		{
			if (CONSONANTS.find(c) != std::string::npos)
			{
				++count;
			}
		}
		return count;
	}

	static bool compareStrings(const std::string& a, const std::string& b)
	{
		// go through shorter string
		// compare find at(a), at(b)
		// if not equal, return less, else go to next char 

		size_t min = a.size() < b.size() ? a.size() : b.size();
		for (size_t i = 0; i < min; ++i)
		{
			auto index1 = ALL_LETTERS.find(a.at(i));
			auto index2 = ALL_LETTERS.find(b.at(i));
			if (index1 < index2)
			{
				return true;
			}
			else if (index1 > index2)
			{
				return false;
			}
		}
		return false;
	}
};