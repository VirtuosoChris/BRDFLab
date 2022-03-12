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


#ifndef GPUSimulationBRDF_H
#define GPUSimulationBRDF_H

#include "SimulatedBRDF.h"
#include "OgreSimulationWindow.h"

#include <QTime>

class OgreSimulationWindow;

class GPUSimulationBRDF: public SimulatedBRDF
{
	Q_OBJECT
public:

	GPUSimulationBRDF(int channels, QString filename, Ogre::String firstIteration, Ogre::String otherIterations, int iterations):
		SimulatedBRDF(channels,QString("GPUSimulation")),
		firstMaterialName(firstIteration),
		iterateMaterialName(otherIterations),
		nIterations(iterations),
		outgoingDirections(NULL),
		simulationResolution(100)
	{
		//guiParams.push_back(new GPUSimulationParameters());
		initComputationScene();	
	}

	~GPUSimulationBRDF();
	
	QString getMaterialSimulationName(){
		return materialSimulation;
	}


	void initComputationScene();
	virtual int setMaterialInfo(Ogre::String materialName)=0;

	//TODO: implement this...
	void writeXML(QDomDocument *doc, QDomElement *parent){}

	void changeSimulationTexturesResolution(int width, int height);
	void setupGPUSimulationParameters(QtProperty *topItem);
	void updateGPUSimulationParameters(QList<QtProperty*> values);

	public slots:
		void computeBRDF(float* &texInfo, int &x, int &y, float &min, float &max);
		void reprocessTexture(int x);

signals:
		virtual void updateSurfaceViewer(int m);
		virtual void updateBRDFViewer();



protected:
	
	QString materialSimulation;
	Ogre::String firstMaterialName;
	Ogre::String iterateMaterialName;
	int nIterations;

	OgreSimulationWindow *simulationWindow;
	int simulationResolution;

	float *outgoingDirections;
	float minValue;
	float maxValue;
	
};



#endif