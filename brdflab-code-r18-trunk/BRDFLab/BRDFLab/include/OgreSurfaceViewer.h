/*
	Copyright (c) 2009.
		Adria Fores
		Sumanta N. Pattanaik
		Carles Bosch
		Xavier Pueyo

	Contact: adriafores@gmail.com	

	This file is part of BRDFLab.

    BRDFLab is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    BRDFLab is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BRDFLab.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OgreSurfaceViewer_H
#define OgreSurfaceViewer_H

#include "ogrewidget.h"
#include "GPUSimulationBRDF.h"
#include "SpheresBRDF.h"
#include "ReliefBRDF.h"
#include "TrianglesBRDF.h"

class SpheresBRDF;
class ReliefBRDF;
class TrianglesBRDF;

class OgreSurfaceViewer : public OgreWidget
{
	Q_OBJECT

public:
	OgreSurfaceViewer(QWidget *parent=NULL, Ogre::String name="ObjectSurfaceViewer");
//	~OgreSurfaceViewer(){}


	void displayBox(Ogre::String materialName);
	void displayFloor(Ogre::String materialName);
	void cleanDisplay();

	void display(GPUSimulationBRDF *sim);
	void displayRelief(Ogre::String textureName);
	void displaySpheres(SpheresBRDF *sph);
	void displayTriangles(TrianglesBRDF *tri);
	void updateCamera();

private:

	void createScene();
//	void setUpAxis();
	void setInitialCameraPosition();


	Ogre::Vector3 center;
};

#endif