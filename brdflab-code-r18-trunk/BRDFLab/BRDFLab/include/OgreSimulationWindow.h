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

#ifndef OgreSimulationWindow_H
#define OgreSimulationWindow_H

#include "ogrewidget.h"
#include "GPUSimulationBRDF.h"
#include <QFileDialog>

class GPUSimulationBRDF;

class OgreSimulationWindow : public OgreWidget
{
	Q_OBJECT

public:
	OgreSimulationWindow(GPUSimulationBRDF *sim, Ogre::String firstIteration, Ogre::String otherIterations, int iterations = 5, int resolutionSimulation = 1000,QWidget *parent=0, Ogre::String name="SimulationWindow");

	~OgreSimulationWindow(){


		// delete used textures 
		if(Ogre::Root::getSingleton().getRenderSystem()->getRenderTarget("MRT0") != NULL){
			Ogre::Root::getSingleton().getRenderSystem()->detachRenderTarget("MRT0");
			Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget("MRT0");
		}

		if(Ogre::Root::getSingleton().getRenderSystem()->getRenderTarget("MRT1") != NULL){
			Ogre::Root::getSingleton().getRenderSystem()->detachRenderTarget("MRT1");
			Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget("MRT1");
		}


		if(Ogre::TextureManager::getSingleton().resourceExists("directions0")){
			Ogre::TextureManager::getSingleton().remove("directions0");

		}

		if(Ogre::TextureManager::getSingleton().resourceExists("directions1")){
			Ogre::TextureManager::getSingleton().remove("directions1");
		}

		if(Ogre::TextureManager::getSingleton().resourceExists("hits0")){
			Ogre::TextureManager::getSingleton().remove("hits0");

		}

		if(Ogre::TextureManager::getSingleton().resourceExists("hits1")){
			Ogre::TextureManager::getSingleton().remove("hits1");
		}


	}

	float* compute(Ogre::Vector2 incidentVector);
	float* processTexture(float* directionsTexture,  int &x, int &y, float &min, float &max);
	void mipmapping();
	void debugTexture(Ogre::String name);
	float* getTexture(Ogre::String name);
	Ogre::Vector2 get2DCoord( float pos, float texwidth );
	Ogre::Vector2 get2DCoordCorner( float pos, float texwidth );
	
	
	void createTextures(int w, int h);


protected:
	void initializeGL();
	void createScene();


private:
//	Ogre::RenderTexture* rt0;
	Ogre::MultiRenderTarget *rttTex;
	Ogre::MultiRenderTarget *rttTex1;
	Ogre::Rectangle2D *rectangle;
	int width;
	int height;

	Ogre::String firstIterationName;
	Ogre::String iterateMaterialName;
	int nIterations;

	int capturingWidth;
	int capturingHeight;

	GPUSimulationBRDF *simulation;
	
};

#endif