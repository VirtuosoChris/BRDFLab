CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(BRDFLab)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
#set(CMAKE_MODULE_PATH $ENV{OGRE_HOME}/CMake)  # use CMake files that come with Ogre instead (OGRE_HOME needs to be defined)
FIND_PACKAGE(OGRE REQUIRED) #Ogre is needed by almost everything

# Boost is usually needed by OGRE
# (Extracted from http://www.vtk.org/Wiki/CMake/Examples#Finding_Packages)
# (BOOST_ROOT should be defined for this to work)
SET(Boost_USE_MULTITHREAD ON)  #needed?
FIND_PACKAGE(Boost)
if(Boost_FOUND)
	INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS})
	LINK_DIRECTORIES(${LINK_DIRECTORIES} ${Boost_LIBRARY_DIRS})
endif(Boost_FOUND)

ADD_SUBDIRECTORY(BRDFLab)
#ADD_SUBDIRECTORY(Demo)
