//
//  surface-actions-utils.cpp
//  reaper_ptl_surface_actions
//
//  Created by Piotr Lechowicz on 02/08/2021.
//  Copyright © 2021 My Company. All rights reserved.
//

#include "surface-actions-utils.hpp"

#include <sstream>

namespace surface_actions {

std::string toUpper(std::string input)
{
	std::for_each(input.begin(), input.end(), [](char& c) {
		c = ::toupper(c);
	});
	return input;
}

std::string toLower(std::string input)
{
	std::for_each(input.begin(), input.end(), [](char& c) {
		c = ::tolower(c);
	});
	return input;
}

bool stringFuzzyMatch(const std::string& name1, const std::string& name2)
{
	std::string copyName1 = toLower(name1);
	std::string copyName2 = toLower(name2);
	
	std::istringstream iss(copyName2);
	
	while (iss)
	{
		std::string word;
		iss >> word;
		if (copyName1.find(word) == std::string::npos)
		{
			return false;
		}
	}
	return true;
}

}
