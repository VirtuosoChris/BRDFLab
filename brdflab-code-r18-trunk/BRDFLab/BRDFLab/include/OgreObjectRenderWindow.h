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

#ifndef OgreObjectRenderWindow_H
#define OgreObjectRenderWindow_H

#include "ogrewidget.h"
#include "hdrimage.h"

class OgreObjectRenderWindow : public OgreWidget
{
	Q_OBJECT

public:
	OgreObjectRenderWindow(QWidget *parent=0, Ogre::String name="ObjectRenderer");
	void setKeyGamma(Ogre::Vector3 value);
	void setMaterials(Ogre::String point, Ogre::String envMap);

	Ogre::String getPointLightMaterialName() {return pointLightMaterialName; }
	Ogre::String getEnvMapMaterialName() {return envMapLightMaterialName; }

	public slots:
			void setLightType(int);
			void setModel(int);
			void setNSamples(int);
			void resizeEvent(QResizeEvent *);
			void setProbe(int i);

private:

	void createScene();
	void generateUniformSampledDirections(int n);
	void loadHDRBackground(QString filename);
	void sphericalHarmonicsCoeficients(QString hdrBackground);
	void genLafortuneSamples(int smpls, float cxy, float cz, float n);
	void setUpAxis();
	void setInitialCameraPosition();
	void generateTangents(Ogre::String meshName);
	
	int model;
	int lightType;
	Ogre::SceneNode *sphereSkyNode;
	Ogre::OverlayContainer* panel;

	
	Ogre::String pointLightMaterialName;
	Ogre::String envMapLightMaterialName;	
};

#endif