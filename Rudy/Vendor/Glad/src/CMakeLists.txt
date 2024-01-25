cmake_minimum_required(VERSION 3.5) 


#define the include directory macro . 
project(glad LANGUAGES C CXX) 


# Add the directory where the glad header files are located
include_directories(../include)

# Create a static library target named 'glad'
add_library(glad STATIC glad.c)

 