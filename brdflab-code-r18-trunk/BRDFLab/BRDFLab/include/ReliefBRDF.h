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


#ifndef ReliefBRDF_H
#define ReliefBRDF_H

#include "GPUSimulationBRDF.h" 

#include <QTime>

class OgreSurfaceViewer;

class ReliefBRDF: public GPUSimulationBRDF
{
	Q_OBJECT
public:

	ReliefBRDF(QString texture):
		GPUSimulationBRDF(1,QString("Relief"),"FirstHeightFieldSimulation","IterationHeightFieldSimulation",10),
		textureName(texture),
		depth(1.),
		tilt(1)
	{
		//initViewer();
		//initComputationScene();
		brdfType = QString("Relief");

		// create gui Parameters
//		guiParams.push_back(new HeightFieldSimulationParameters());
	
	}

	~ReliefBRDF(){

	}

	QString getTextureName(){
		return textureName;
	}


	int setMaterialInfo(Ogre::String materialName);

	void writeXML(QDomDocument *doc, QDomElement *parent);

	void updateMaterial();
	void recreateParameterGUI();

	public slots:
		void guiUpdated();

	signals:
		void updateSurfaceViewer(int m);

protected:
	
	QString textureName;
	float depth;
	int tilt;

};



#endif