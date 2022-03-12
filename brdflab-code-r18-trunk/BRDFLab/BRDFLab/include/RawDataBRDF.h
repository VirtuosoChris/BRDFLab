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


#ifndef RawDataBRDF_H
#define RawDataBRDF_H

#include "BRDF.h" 

#include <QFile>
#include <QTextStream>

class RawDataBRDF: public BRDF
{
	Q_OBJECT
public:

	RawDataBRDF(int channels, QString dataFileName);

	~RawDataBRDF();

	virtual void writeXML(QDomDocument *doc, QDomElement *parent) {}
	QString getMaterialName() { return brdfType; }

	void readMeasuredData();


//	float* getRenderingtInfoForFitting(int x, bool getLightDir);
//	float* getStepOptimization(Ogre::Vector3 lightPosition);

	
	std::vector<Ogre::Vector3>* getIncidentDirections() {return &u;}
	std::vector<Ogre::Vector3>* getOutgoingDirections() {return &v;}
	std::vector<Ogre::Vector3>* getIntensities() {return &y;}
	


	void updateMaterial(){
	
	}

	public slots:
		void setCubeRoot(bool cr) {};	
	
protected:
	
	QString fileName;

	std::vector<Ogre::Vector3> u;
	std::vector<Ogre::Vector3> v;
	std::vector<Ogre::Vector3> y;
	
};



#endif