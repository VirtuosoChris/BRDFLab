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

#ifndef Fitting_H
#define Fitting_H

#include "BRDF.h"
#include "MeasuredBRDF.h"
#include "AnalyticBRDF.h"
#include "SimulatedBRDF.h"
#include "RawDataBRDF.h"

#include <QDoubleSpinBox>
#include <QInputDialog>
#include <QCoreApplication>



//include OGRE for using Vector3 types and that stuff.
#include <Ogre.h>

#include <iostream>

class Fitting
{

public:


	Fitting(BRDF *source, AnalyticBRDF *destination, QString name, bool nonLinearOptimization = false);
	~Fitting(){
		if (lobeValueMatrix!= NULL){
			delete []lobeValueMatrix;
			lobeValueMatrix = NULL;
		}

		if (AtransposeA!= NULL){
			delete []AtransposeA;
			AtransposeA = NULL;
		}

		if (invAtransposeA!= NULL){
			delete []invAtransposeA;
			invAtransposeA = NULL;
		}

		if (AtransposeB!= NULL){
			delete []AtransposeB;
			AtransposeB = NULL;
		}
	}
	
	/*
		The optimize function launches the optimization process. Returns if the optimize has ben stoped by the user.
	*/
	virtual bool optimize()=0;
	void writeXML(QDomDocument *doc, QDomElement *parent);
	BRDF* getFittedBRDF() { return dst; }
	void setError(double e) {error = e;}
	void generateSyntheticDirections(std::vector<Ogre::Vector3> *u, std::vector<Ogre::Vector3> *v);
	int preOptimization();
	void stop();
	void setErrorGUI(QDoubleSpinBox *errorSpinBox);	
	int getStatus();
	void setErrorMetric(int eM);
	static void matInverse(double *M, int size, double *iM);
	static void computeBRDFVector(double *p, double *x);
	static double computeError(double *x);

protected:

	QString optimizerName;
	static BRDF *src;
	static AnalyticBRDF *dst;
	static double error;
	static int errorMetric;
	static double *lobeValueMatrix, // Size is (ySize/nChannels) of measured BRDF x nLobes of Analytic BRDF. Must be created at the instancing time.
	       *AtransposeA, // Size is nLobes x nLobes. Necessary for Linear Least Squares Computation
		   *invAtransposeA,
		   *AtransposeB; // size is nLobes x nChannels. Necessary for Linear Least Squares Computation

	static bool verbose;
	static bool forceStop;
	static bool nonLinear;

	static std::vector<Ogre::Vector3> *u;
	static std::vector<Ogre::Vector3> *v;	
	static double *p;
	static double *upBound;
	static double *lowBound;
	static double *y;
	static int ySize;
	static QDoubleSpinBox *errorGUI;

	int status;
	int maxIterations;


};


#endif