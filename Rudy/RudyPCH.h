#pragma once


//project core files
//log first ,  assertion based on log
#include "Rudy/Core/Log.h"

#include "Rudy/Core/Base.h" 

#include "Rudy/Debug/Instrumentor.h"




#include "Rudy/Core/FileSystem.h"

//third party 

#include <glad/glad.h>  //before glfw3.h
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>  //for glm::value_ptr

//standard library

#include <string> 
#include <vector>

#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>   
#include <map>
#include <unordered_set>
 
#include <functional>  //event system
#include <memory>   //smart pointers
#include <algorithm>
#include <utility>


#include <random> 
