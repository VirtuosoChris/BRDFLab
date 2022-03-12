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

#ifndef OgreGroovesWindow_H
#define OgreGroovesWindow_H

#include "ogrewidget.h"
//#include "GPUSimulationBRDF.h"
#include <QFileDialog>

//class GPUSimulationBRDF;

class OgreGroovesWindow : public OgreWidget
{
	Q_OBJECT

public:
	OgreGroovesWindow(Ogre::String textureName = "groovesTexture", QWidget *parent=0, Ogre::String name="GroovesWindow");

	~OgreGroovesWindow(){

		// delete used textures 

		if(Ogre::TextureManager::getSingleton().resourceExists(texName)){
			Ogre::TextureManager::getSingleton().remove(texName);

		}
	}

	void createGroovesTexture(double grooveWidth = 0.004, double grooveSpacing = 0.02, double grooveBotAngle = 60., double groovesAngle = 90.,int numGroovesPerSide = 3, int numTexelsPerGroove = 30);

protected:
	void initializeGL();
	void createScene();
	void debugTexture(Ogre::String name);


private:
	Ogre::RenderTexture* rt0;

	int width;
	int height;

	Ogre::String texName;

	
};

#endif