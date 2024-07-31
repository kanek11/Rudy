#pragma once


//project core files
// assert -> log-> base
// assert -> platform detection 



//usually independent of other files,but many depend on it
#include "Rudy/Core/PlatformDetection.h"


//standard library
#include <vector> 
#include <string>  
#include <unordered_map>   
#include <map>
#include <unordered_set>
 
#include <functional>  //event system
#include <memory>   //smart pointers
#include <algorithm>
#include <utility>


#include <sstream>
#include <fstream>
#include <iostream>

#include <random>  



//third party 

#include <glad/glad.h>  //before glfw3.h
#include <GLFW/glfw3.h> 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>  //for glm::value_ptr



#include "Rudy/Core/Base.h" 
#include "Rudy/Core/Log.h" 
#include "Rudy/Core/Assert.h" 
#include "Rudy/Debug/Instrumentor.h"  

#include "Rudy/Core/FileSystem.h"