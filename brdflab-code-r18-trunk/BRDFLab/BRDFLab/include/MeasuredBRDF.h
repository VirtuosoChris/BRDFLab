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


#ifndef MeasuredBRDF_H
#define MeasuredBRDF_H

#include "BRDF.h" 

//#include "MERLReading.h"

class MeasuredBRDF: public BRDF
{
	Q_OBJECT
public:

	MeasuredBRDF(int channels, QString filename, int tResolution, int pResolution):
		BRDF(channels,QString("Measured")),
		materialName(filename),
		cubeRoot(false),
		thetaResolution(tResolution),
		phiResolution(pResolution)
	{
		
	}

	~MeasuredBRDF()
	{
/*
		if(y!=NULL)
			delete []y;

		u.clear();
		v.clear();
*/
	}

	virtual void readMeasuredData()=0;
	virtual void writeXML(QDomDocument *doc, QDomElement *parent)=0;
	virtual void setMeasuredPoints(float phi_in, float theta_in, std::vector<Ogre::Vector3> *points, int mode)=0;

	virtual void getFittingInformation(std::vector<Ogre::Vector3> *u, std::vector<Ogre::Vector3> *v, double* &y, int &ySize)=0;

//	std::vector<Ogre::Vector3>* getIncidentDirections(){ return &u;	}

//	std::vector<Ogre::Vector3>* getOutgoingDirections(){ return &v; }

//	double* getMeasuredData(){ return y; }
//	int getYSize() { return ySize; }
	QString getMaterialName() { return brdfType; }
	int getThetaResolution() { return thetaResolution; }
	int getPhiResolution() { return phiResolution; }
	
	void updateMaterial(){}

	public slots:
			void setCubeRoot(bool cr){ cubeRoot = cr; }
	

protected:
	
	QString materialName;
	QString path;
//	int ySize;
//	double *y; // measured vector
//	std::vector<Ogre::Vector3> u; // incident directions
//	std::vector<Ogre::Vector3> v; // outgoing directions

	int thetaResolution;
	int phiResolution;

	bool cubeRoot;
	
};



#endif