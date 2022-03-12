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


#ifndef SpheresBRDF_H
#define SpheresBRDF_H

#include "GPUSimulationBRDF.h" 
#include "OgreSurfaceViewer.h"

#include <QTime>

class OgreSurfaceViewer;

class SpheresBRDF: public GPUSimulationBRDF
{
	Q_OBJECT
public:

	SpheresBRDF(int numberOfSpheres=100, float sphereRadius=0.1):
		GPUSimulationBRDF(1,QString("Spheres"),"FirstSpheresSimulation","IterationSpheresSimulation",10),
		nSpheres(numberOfSpheres),
		radius(sphereRadius),
		textureName("spheresData"),
//		info(NULL),
		overlap(false)
	{
		brdfType = QString("Spheres");

//		guiParams.push_back(new SpheresSimulationParameters());

//		createSpheres();

		//recreateParameterGUI();

		constructGrid();

		
	}

	~SpheresBRDF(){
	/*
		if(info != NULL)
			delete []info;
*/
		if(Ogre::TextureManager::getSingleton().resourceExists(textureName)){
			Ogre::TextureManager::getSingleton().remove(textureName);

		}
		
	}

	float* createSpheres();

	float getRadius(){ return radius; }
	void setRadius(float r){ radius = r; }
	//float* getInfo() {return info;}
	int getNSpheres(){return nSpheres;}
	float getTexWidth() {return texWidth; }
	void updateMaterial();
	void constructGrid();

	int setMaterialInfo(Ogre::String materialName);
	//TODO: implement this...
	void writeXML(QDomDocument *doc, QDomElement *parent);
	void recreateParameterGUI();

	public slots:
		void guiUpdated();
signals:
		void updateSurfaceViewer(int m);
		void updateBRDFViewer();

protected:
	Ogre::Vector4 createOneSphere();
	bool spheresIntersect(Ogre::Vector4 sph1, Ogre::Vector4 sph2);


	// internal parameters
	Ogre::String textureName;
	float texWidth; // spheres texture
	//float *info;

	// object parameters
	int nSpheres;
	float radius;
	bool overlap;

	int gridSide;
	int gGridTextureSide; // grid texture
	int texSide; // index texture
	
	
	

	
};



#endif