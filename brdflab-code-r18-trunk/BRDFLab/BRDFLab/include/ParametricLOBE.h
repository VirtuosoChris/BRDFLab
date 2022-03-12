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

#ifndef ParametricLOBE_H
#define ParametricLOBE_H

//include OGRE for using Vector3 types and that stuff.
#include <Ogre.h>

#include "LobeParameter.h"
#include "muParser.h"

#include <QString>
#include <QDomDocument>
#include <QFile>
#include <iostream>


class ParametricLOBE
{
public:
	enum LobeTypes {Isotropic, Anisotropic};
	
	ParametricLOBE(QString fileName);
	~ParametricLOBE();
	
	double evalLOBE(double *param, Ogre::Vector3 u, Ogre::Vector3 v, int combinationType);
		
	QString getLobeName(){return lobeName;};
	void setDefaultParameters(double *param, int combinationType);
	int getNParameters(int combinationType);
	void initForOptimize(double *lb, double *ub, int combinationType);
	void showParameters(double *p, int combinationType);
	void loadXML(QDomElement param, double *p, int combinationType);
	void writeXML(QDomDocument *doc, QDomElement *parent, double *p, int combinationType);
	LobeParameter getLobeParameter(int i);
	
	QString getEquationGLSL(int combinationType);
	QString getPdfGLSL(){return pdfGLSL; }
	QString getImportanceSamplingGLSL(){return importanceSamplingGLSL; }
	QString getSpecularLobeDirection() {return specularLobeDirection; }
	QString getMirrorDirectionGLSL() {return mirrorDirectionGLSL; }

	virtual float computePDF(double *p, Ogre::Vector3 u, Ogre::Vector3 v) {return 1; }
	virtual void computeNImportanceSamples(double *p, float *samples, int nSamples); 
	virtual void computeImportanceSamplingOutgoingVectors(double *p,float *samples,int nSamples, std::vector<Ogre::Vector3> *directions, Ogre::Vector3 v);
	void computeImportanceSamplingOutgoingVectorsHalfwayVector(double *p,float *samples,int nSamples, std::vector<Ogre::Vector3> *directions, Ogre::Vector3 v);

	Ogre::Vector2 cart2spherical( Ogre::Vector3 pos )
	{

		// 0 -> phi
		// 1 -> theta

		// code from colbert et al.
		pos.normalise();

		Ogre::Radian phi = Ogre::Math::ATan2(pos[1],pos[0]);

		float fphi = phi.valueRadians();

		fphi = (fphi>0.)?fphi:((2.*Ogre::Math::PI) + fphi);

		Ogre::Radian theta = Ogre::Math::ACos(pos[2]);


		return Ogre::Vector2(fphi,theta.valueRadians());
	}


	Ogre::Vector3 spherical2cart(Ogre::Vector2 i){

		// 0 -> phi
		// 1 -> theta

		Ogre::Vector3 out;

		out[0] = sin(i[1])*cos(i[0]);
		out[1] = sin(i[1])*sin(i[0]);
		out[2] = cos(i[1]);

		return out;
	}


	double fresnel(double F0, double dotVH);

protected:

	QString lobeName;

	LobeTypes lobeType; // isotropic or anisotropic

	// number of parameters of the model
	int nParameters;

	std::vector<LobeParameter> parameters;
	QString equation, equationGLSL, pdfGLSL, importanceSamplingGLSL, specularLobeDirection, mirrorDirectionGLSL;
	mu::Parser  parser;

	double ux,uy,uz,vx,vy,vz, theta_i, theta_o, phi_i, phi_o;
	double *paramVal;
};


#endif