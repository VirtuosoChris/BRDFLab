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


#ifndef SimulatedBRDF_H
#define SimulatedBRDF_H

#include "BRDF.h"

#include <QProgressDialog>
#include <QLabel>
#include <QCoreApplication>
#include <QTime>

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"


class SimulatedBRDF: public BRDF
{
	Q_OBJECT
public:

	SimulatedBRDF(int channels, QString filename):
		BRDF(channels,QString("Simulated")),
		materialName(filename),
		incidentVector(0.,0.),
		textureResolution(16.,4.),
		intensityTextureName("DisplayTexture")
	{
		setCubeRoot(cubeRoot);
	}

	~SimulatedBRDF(){
	}

	QString getMaterialName(){
		return materialName;
	}

	//TODO: implement this...
	void writeXML(QDomDocument *doc, QDomElement *parent){}

	void setIncidentVector(Ogre::Vector2 u);
	void updateMaterial(){}
	void set3DTexture();

	public slots:
		virtual void computeBRDF(float* &texInfo, int &x, int &y, float &min, float &max)=0;
		void displayBRDF(int x);
		void setDisplayTexture(float *texInfo, int x, int y, float min, float max);
		void setCubeRoot(bool cr);
		virtual void recreateParameterGUI()=0;
		virtual void guiUpdated()=0;


public:

	float evalBRDF(Ogre::Vector3 incoming, Ogre::Vector3 outgoing);
	void generateFittingInformation(int thetaDivisions, int phiDivisions, std::vector<Ogre::Vector3> *u, std::vector<Ogre::Vector3> *v, double* &y, int &ySize);
	void setGUIManagers( QtVariantPropertyManager * vM, QtTreePropertyBrowser * vE );

protected:
//	void createIntensityTexture(float *texInfo, int x, int y, float min = 0., float max = 1.);
	
	QString materialName;
	Ogre::String intensityTextureName;
	
	Ogre::Vector2 incidentVector;
	Ogre::Vector2 textureResolution;

	// GUI
	QtVariantPropertyManager *variantManager;
	QtTreePropertyBrowser *variantEditor;
	
};



#endif