#
#	Copyright (c) 2009.
#		Adria Fores
#		Sumanta N. Pattanaik
#		Carles Bosch
#		Xavier Pueyo
#
#	Contact: adriafores@gmail.com	
#
#	This file is part of BRDFLab.
#
#    BRDFLab is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    BRDFLab is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with BRDFLab.  If not, see <http://www.gnu.org/licenses/>.
#


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(BRDFLab)

SET(SRC_FILES

	#Main
	source/Main.cpp
	source/BRDFLab.cpp 

	#Display
	source/BRDFViewer.cpp
	source/ogrewidget.cpp
	
	#object renderer
	source/ObjectRenderer.cpp
	source/OgreObjectRenderWindow.cpp
	source/OgreSurfaceViewer.cpp

	#simulation
	source/OgreSimulationWindow.cpp
	source/OgreGroovesWindow.cpp

	#Fitting
	source/Fitting.cpp
	source/LMFitting.cpp
	source/NLoptFitting.cpp

	#Console
	source/Console.cpp

	#XML highlight
	source/XML_Editor.cpp

)


SET(INC_FILES
	include/imdebug.h

	#Main
	include/BRDFLab.h 

	#Display
	include/BRDFViewer.h
	include/ogrewidget.h
	
	#object renderer
	include/ObjectRenderer.h
	include/OgreObjectRenderWindow.h
	include/OgreSurfaceViewer.h
	
	#simulation
	include/OgreSimulationWindow.h
	include/OgreGroovesWindow.h

	#Fitting
	include/Fitting.h
	include/LMFitting.h
	include/NLoptFitting.h

	#console
	include/Console.h


	#XML highlight
	include/XML_Editor.h
)


SET(INC_BRDF
	#BRDF hierarchy
	include/BRDF.h
	include/AnalyticBRDF.h 
	include/FittedBRDF.h 
	include/MeasuredBRDF.h
	include/MERLBRDFDatabase.h
	include/MERLStructureBRDF.h
	include/CarPaintBRDFDatabase.h
	include/RawDataBRDF.h
#	include/MERLReading.h
#	include/CarPaintReading.h
	include/ScatmechBRDF.h
	include/GPUSimulationBRDF.h
	include/SimulatedBRDF.h
	include/ReliefBRDF.h
	include/SpheresBRDF.h
	include/TrianglesBRDF.h
	include/GroovesBRDF.h
)

SET(SRC_BRDF
	#BRDF hierarchy
	source/BRDF.cpp
	source/AnalyticBRDF.cpp 
	source/FittedBRDF.cpp 
	source/MeasuredBRDF.cpp
	source/MERLBRDFDatabase.cpp
	source/MERLStructureBRDF.cpp
	source/CarPaintBRDFDatabase.cpp
	source/RawDataBRDF.cpp
	source/ScatmechBRDF.cpp
	source/GPUSimulationBRDF.cpp
	source/SimulatedBRDF.cpp
	source/ReliefBRDF.cpp
	source/SpheresBRDF.cpp
	source/TrianglesBRDF.cpp
	source/GroovesBRDF.cpp
	
)

SET(INC_LOBE

	#BRDF Lobes
	include/ParametricLOBE.h
	include/LobeParameter.h
#	include/AshikhminShirleyLOBE.h
#	include/LafortuneIsoLOBE.h
#	include/LafortuneAnisoLOBE.h
#	include/LambertianLOBE.h
#	include/BlinnPhongLOBE.h
#	include/WardLOBE.h
#	include/WardDuerLOBE.h
#	include/CookTorranceLOBE.h
#	include/Li_sparseLOBE.h
#	include/Ross_thickLOBE.h
)


SET(SRC_LOBE

	#BRDF Lobes
	source/ParametricLOBE.cpp
	source/LobeParameter.cpp
#	source/AshikhminShirleyLOBE.cpp
#	source/LafortuneIsoLOBE.cpp
#	source/LafortuneAnisoLOBE.cpp
#	source/LambertianLOBE.cpp
#	source/BlinnPhongLOBE.cpp
#	source/WardLOBE.cpp
#	source/WardDuerLOBE.cpp
#	source/CookTorranceLOBE.cpp
#	source/Li_sparseLOBE.cpp
#	source/Ross_thickLOBE.cpp

)

SET(INC_SCATMECH
	include/SubsurfaceParticleBRDF.h
	include/DoubleInteractionBRDF.h
)

SET(SRC_SCATMECH
	source/SubsurfaceParticleBRDF.cpp
	source/DoubleInteractionBRDF.cpp

)


SET(INC_DOCKS
	include/PropertyEditorWidget.h
	include/PropertyBRDFEditorWidget.h

)


SET(SRC_DOCKS
	source/PropertyEditorWidget.cpp
	source/PropertyBRDFEditorWidget.cpp

)

SET(MATERIAL_FILES
	../build/BRDFLab/Data/shaders/basics.material

)

#SET(BRDFs
#	brdfs/BRDF.h
#)


SET(UI_FILES
	ui/MainWindow.ui
)

SET(COLBERT_CODE_INC
	#rendering aux.
#	include/binaryobj.h
	include/hdrimage.h
	include/image.h 
	include/rgbe.h
	include/color.h
	include/vectors.h
#	include/wavefront.h
)

SET(COLBERT_CODE_SRC
	#rendering aux.
#	source/binaryobj.cpp
	source/hdrimage.cpp
	source/rgbe.cpp
#	source/wavefront.cpp
)

# muParser files
SET(INC_muParser

	muParser/include/muParser.h
	muParser/include/muParserBase.h
	muParser/include/muParserBytecode.h
	muParser/include/muParserCallback.h
	muParser/include/muParserDef.h
	muParser/include/muParserError.h
	muParser/include/muParserFixes.h
	muParser/include/muParserStack.h
	muParser/include/muParserToken.h
	muParser/include/muParserTokenReader.h
)

SET(SRC_muParser
	muParser/source/muParser.cpp
	muParser/source/muParserBase.cpp
	muParser/source/muParserBytecode.cpp
	muParser/source/muParserCallback.cpp
	muParser/source/muParserError.cpp
	muParser/source/muParserTokenReader.cpp
)


#SET(RESOURCE_FILES
#	resources/resources.qrc
#)

#"Sources" and "Headers" are the group names in Visual Studio.
SOURCE_GROUP("Sources" FILES ${SRC_FILES})
SOURCE_GROUP("Headers" FILES ${INC_FILES})

# lobes
SOURCE_GROUP("Lobes\\\\Headers" FILES ${INC_LOBE})
SOURCE_GROUP("Lobes\\\\Sources" FILES ${SRC_LOBE})

#scatmech
SOURCE_GROUP("Scatmech\\\\Headers" FILES ${INC_SCATMECH})
SOURCE_GROUP("Scatmech\\\\Sources" FILES ${SRC_SCATMECH})

# muParser
SOURCE_GROUP("muParser\\\\Headers" FILES ${INC_muParser})
SOURCE_GROUP("muParser\\\\Sources" FILES ${SRC_muParser})

# brdfs
SOURCE_GROUP("Brdfs\\\\Headers" FILES ${INC_BRDF})
SOURCE_GROUP("Brdfs\\\\Sources" FILES ${SRC_BRDF})

#docks
SOURCE_GROUP("GUI\\\\Headers" FILES ${INC_DOCKS})
SOURCE_GROUP("GUI\\\\Sources" FILES ${SRC_DOCKS})

# Colbert code for hdr managing and dual paraboloid
SOURCE_GROUP("Auxiliary\\\\Headers" FILES ${COLBERT_CODE_INC})
SOURCE_GROUP("Auxiliary\\\\Sources" FILES ${COLBERT_CODE_SRC})


FIND_PACKAGE(Qt4)
SET(QT_USE_QTXML 1)
SET(QT_USE_QTNETWORK 1)
INCLUDE(${QT_USE_FILE})

QT4_WRAP_CPP(MOC_SRCS ${INC_FILES} ${INC_DOCKS} ${INC_BRDF})
QT4_WRAP_UI(UI_SRCS ${UI_FILES})
#QT4_ADD_RESOURCES(RESOURCE_SRCS ${RESOURCE_FILES})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${OGRE_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/levmar-2.3 ${CMAKE_CURRENT_SOURCE_DIR}/scatmech ${CMAKE_CURRENT_SOURCE_DIR}/qtpropertybrowser-2.5_1-opensource/src/ ${CMAKE_CURRENT_SOURCE_DIR}/muParser/include/ ${CMAKE_CURRENT_SOURCE_DIR}/nlopt) 
LINK_DIRECTORIES(${OGRE_LIB_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/levmar-2.3 ${CMAKE_CURRENT_SOURCE_DIR}/scatmech/release ${CMAKE_CURRENT_SOURCE_DIR}/scatmech/debug ${CMAKE_CURRENT_SOURCE_DIR}/qtpropertybrowser-2.5_1-opensource/lib ${CMAKE_CURRENT_SOURCE_DIR}/nlopt)

ADD_EXECUTABLE(BRDFLab ${INC_FILES} ${SRC_FILES} ${INC_LOBE} ${SRC_LOBE} ${INC_BRDF} ${SRC_BRDF} ${INC_DOCKS} ${SRC_DOCKS} ${MOC_SRCS} ${UI_SRCS} ${COLBERT_CODE_INC} ${COLBERT_CODE_SRC} ${INC_SCATMECH} ${SRC_SCATMECH} ${INC_muParser} ${SRC_muParser}) # ${RESOURCE_SRCS})

SET(SCATMECH_LIBRARIES debug scatmechd.lib optimized scatmech.lib)
SET(QT_PROPERTY_BROWSER_LIBRARIES debug QtSolutions_PropertyBrowser-2.5d.lib optimized QtSolutions_PropertyBrowser-2.5.lib)

#ADD_LIBRARY(BRDFLab ${SRC_FILES} ${INC_FILES} ${MOC_SRCS} ${UI_SRCS} ) #${RESOURCE_SRCS})
TARGET_LINK_LIBRARIES(BRDFLab ${QT_LIBRARIES} ${OGRE_LIBRARIES} ${SCATMECH_LIBRARIES} ${QT_PROPERTY_BROWSER_LIBRARIES} blas.lib clapack.lib LIBF77.lib levmar.lib LIBI77.lib imdebug.lib libnlopt-0.lib)
 
#IF (WIN32) #Windows -- TO IMPROVE
#CONFIGURE_FILE(${OGRE_LIB_DIR}/OgreMain_d.dll ${CMAKE_CURRENT_BINARY_DIR}/debug/OgreMain_d.dll COPYONLY)
#CONFIGURE_FILE(${OGRE_LIB_DIR}/RenderSystem_GL_d.dll ${CMAKE_CURRENT_BINARY_DIR}/debug/RenderSystem_GL_d.dll COPYONLY)

#CONFIGURE_FILE(${OGRE_LIB_DIR}/OgreMain.dll ${CMAKE_CURRENT_BINARY_DIR}/release/OgreMain.dll COPYONLY)
#CONFIGURE_FILE(${OGRE_LIB_DIR}/RenderSystem_GL.dll ${CMAKE_CURRENT_BINARY_DIR}/release/RenderSystem_GL.dll COPYONLY)
#ENDIF (WIN32)
