//
//  surface-actions-utils.hpp
//  reaper_ptl_surface_actions
//
//  Created by Piotr Lechowicz on 02/08/2021.
//  Copyright © 2021 My Company. All rights reserved.
//

#ifndef surface_actions_utils_hpp
#define surface_actions_utils_hpp

#include <stdio.h>
#include <string>
#include <vector>

namespace surface_actions {

std::string toUpper(std::string input);
std::string toLower(std::string input);

/* returns true if all words in string name2 appears in name1 */
bool stringFuzzyMatch(const std::string& name1, const std::string& name2);

}




#endif /* surface_actions_utils_hpp */
