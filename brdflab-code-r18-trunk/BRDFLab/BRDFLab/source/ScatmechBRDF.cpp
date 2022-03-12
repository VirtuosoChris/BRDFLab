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

#include "ScatmechBRDF.h"

#include "imdebug.h"

using namespace SCATMECH;
using namespace std;

ScatmechBRDF::~ScatmechBRDF(){

}



/// caution here!!!!
/*
	MuellerMatrix m=brdf_model->Mueller(incidentVector[1],polarCoord[0][1],incidentVector[0]-polarCoord[0][0],0);        

    // Calculate the Stokes vector for p-polarized incident light...
    StokesVector s=m*StokesVectorUnitUnpol();//StokesVectorUnitP();
*/

float
ScatmechBRDF::evalBRDF(Ogre::Vector3 incoming, Ogre::Vector3 outgoing){
		
	Ogre::Vector2 in(BRDF::cart2spherical(incoming));
	Ogre::Vector2 out(BRDF::cart2spherical(outgoing));

	return evalBRDF(in,out);
}

float
ScatmechBRDF::evalBRDF(Ogre::Vector2 in, Ogre::Vector2 out){
		
	MuellerMatrix m=brdfModel->Mueller((double)in[1],(double)out[1],((double)in[0])-(double)out[0],0.);        
	
	// Calculate the Stokes vector for p-polarized incident light...
	StokesVector s=m*StokesVectorUnitUnpol();//StokesVectorUnitP();

	return (float)s.I();
}


void 
ScatmechBRDF::computeBRDF(float* &texInfo, int &x, int &y, float &min, float &max){

	y = x;
	x = x;//4*x;

	// create array
	if(texInfo != NULL){
		delete []texInfo;
		cout << "Warning!" << endl;
	}
	texInfo = new float[x*y/*(y+1)*/];
	
	QTime *timer = new QTime();

	timer->start();

	double step_phi = (2*Ogre::Math::PI)/(x-1);
	double step_theta = (Ogre::Math::PI/2)/(y-1);//(y-1);

//	y++;

	int v = 0;

	for(int i=0; i<y; i++){
		for(int j=0; j<x; j++){
			
			if(i==0){ // the first line is equal!!!

				if(v == 0){
					double theta=step_theta*i;

					if(theta > Ogre::Math::PI/2. - 0.0001) 
						theta = Ogre::Math::PI/2. - 0.0001;

					//eval BRDF
					texInfo[v] = evalBRDF(incidentVector, Ogre::Vector2(step_phi*j,theta));
				
					min = texInfo[v];
					max = min;
				}else{
					texInfo[v] = texInfo[0];
				}

			}else{

				double theta=step_theta*i;

				if(theta > Ogre::Math::PI/2. - 0.0001) 
					theta = Ogre::Math::PI/2. - 0.0001;

				//eval BRDF
				texInfo[v] = evalBRDF(incidentVector, Ogre::Vector2(step_phi*j,theta));
			
				if(texInfo[v] < min)
					min = texInfo[v];
				if(texInfo[v] > max)
					max = texInfo[v];

			}

			v++;

		}
	}


	cout << "computing texInfo: " << timer->elapsed() << " ms" << endl;
	
	delete timer;

	//imdebug("r w=%d h=%d %p  b=32f", x, y, texInfo);
}