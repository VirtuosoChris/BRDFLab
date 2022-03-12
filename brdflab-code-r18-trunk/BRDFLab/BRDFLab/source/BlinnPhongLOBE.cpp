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

#include "BlinnPhongLOBE.h"

using namespace std;

#define exponentIndex 0

BlinnPhongLOBE::BlinnPhongLOBE():
	ParametricLOBE("BlinnPhong", Isotropic, 1)
{
	paramNames[exponentIndex] = "Exponent";
	paramLb[exponentIndex] = 0.0;
	paramUb[exponentIndex] = 1.0e+5;
}

double 
BlinnPhongLOBE::evalLOBE(double *param, Ogre::Vector3 u, Ogre::Vector3 v){

	Ogre::Vector3 h(u+v);
	h.normalise();
	double cosDelta = h[2]; // Cosine of the angle between H and N. We assume N = [0 0 1]. So Cosine is the Z-component of H.
	double exponent = param[exponentIndex];
	double value=0.0;

	if (cosDelta > 0) 
		value = Ogre::Math::Pow(cosDelta,exponent)*(exponent+2)/(2*Ogre::Math::PI);

	return value;
}


void 
BlinnPhongLOBE::setDefaultParameters(double *param){

	param[exponentIndex] = 5.;

}

void
BlinnPhongLOBE::computeNImportanceSamples(double *p, float *samples, int nSamples){

	for(int i=0; i<nSamples; i++){
		// phi, theta	
		samples[i*2+0] = 2.*Ogre::Math::PI*samples[i*2+0];

		samples[i*2+1] = acos(Ogre::Math::Pow(samples[i*2+1],1./(p[exponentIndex]+1.)));
	}

}

void 
BlinnPhongLOBE::computeImportanceSamplingOutgoingVectors(double *p,float *samples,int nSamples, std::vector<Ogre::Vector3> *directions, Ogre::Vector3 v){


	for(int i=0; i<nSamples; i++){

		// access the correct sample, represents the halfway vector
		Ogre::Vector3 h = spherical2cart(Ogre::Vector2(samples[i*2+0],samples[i*2+1]));

		float pdf = computePDF(p,h,v);

		if(h.dotProduct(v) < 0.)
			h = -h;
		// compute the incident direction by reflecting about h
		Ogre::Vector3 u = -v +2. * h.dotProduct(v) * h;

		//		if(u[2] < 0.)
		//			u[2] = -u[2];

		// do something
		directions->push_back(u*pdf);

	}
}

float 
BlinnPhongLOBE::computePDF(double *p, Ogre::Vector3 u, Ogre::Vector3 v){

	Ogre::Vector2 incoming = cart2spherical(u);

	// colbert
	return (p[exponentIndex]+1)/(2.*Ogre::Math::PI)*powf((float)u[2],(float)p[exponentIndex])*sinf(incoming[1]);

	//pbrt
/*
	Ogre::Vector3 h(u+v);

	h.normalise();

	return ((float)p[exponentIndex]+1.)*pow(abs((float)u[2]),(float)p[exponentIndex])/(6.28319*4.*h.dotProduct(v));
*/
}


