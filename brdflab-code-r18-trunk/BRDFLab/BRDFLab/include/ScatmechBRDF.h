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


#ifndef ScatmechBRDF_H
#define ScatmechBRDF_H

#include "SimulatedBRDF.h" 

// scatmech includes
#include "scatmech/scatmech.h"
#include "scatmech/brdf.h"

#include <QTime>

class ScatmechBRDF: public SimulatedBRDF
{
	Q_OBJECT
public:

	ScatmechBRDF(int channels, QString filename):
		SimulatedBRDF(channels,QString("Scatmech")),
		materialModel(filename),
		brdfModel(NULL)
//		incidentVector(0.,0.),
//		resIntensityTex(0)
	{
		brdfType = QString("Scatmech");
	}

	~ScatmechBRDF();

	QString getMaterialModel(){
		return materialName;
	}
	

	//TODO: implement this...
	void writeXML(QDomDocument *doc, QDomElement *parent){}

//	void setQPropertyEditorWidget(QPropertyEditorWidget *qpe);
//	void setIncidentVector(Ogre::Vector2 u);
	void recreateParameterGUI(){}

	public slots:
//		virtual void guiUpdated()=0;
		void computeBRDF(float* &texInfo, int &x, int &y, float &min, float &max);


public:

	float evalBRDF(Ogre::Vector2 in, Ogre::Vector2 out);
	float evalBRDF(Ogre::Vector3 incoming, Ogre::Vector3 outgoing);

	// create intensity
	// called directly when incoming direction changes or lobe quality vary.

	// recompute BRDF

/*
	virtual void readMeasuredData()=0;
	virtual void writeXML(QDomDocument *doc, QDomElement *parent)=0;
	virtual void setMeasuredPoints(float phi_in, float theta_in, std::vector<Ogre::Vector3> *points)=0;

	std::vector<Ogre::Vector3>* getIncidentDirections(){ return &u;	}

	std::vector<Ogre::Vector3>* getOutgoingDirections(){ return &v; }

	double* getMeasuredData(){ return y; }
	int getYSize() { return ySize; }
	QString getMaterialName() { return brdfType; }
	int getThetaResolution() { return thetaResolution; }
	int getPhiResolution() { return phiResolution; }


*/
protected:
	
	QString materialModel;
	
//	int resIntensityTex;
//	Ogre::Vector2 incidentVector;
	SCATMECH::BRDF_Model *brdfModel;
//	QPropertyEditorWidget *propertyEditor;


	/*
	int ySize;
	double *y; // measured vector
	std::vector<Ogre::Vector3> u; // incident directions
	std::vector<Ogre::Vector3> v; // outgoing directions

	int thetaResolution;
	int phiResolution;*/
	
};



#endif