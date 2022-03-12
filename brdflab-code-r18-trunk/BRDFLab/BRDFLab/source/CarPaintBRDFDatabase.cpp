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

#include "CarPaintBRDFDatabase.h"
#include <fstream>

//#include "MERLReading.h"

using namespace std;

CarPaintBRDFDatabase::CarPaintBRDFDatabase(int channels, QString filename):
	MERLStructureBRDF(filename, 16, 64, 1., 1., 1.)
{
	brdfType = QString("CarPaintBRDFDatabase");	
}

void
CarPaintBRDFDatabase::readMeasuredData()
{

	std::string name = materialName.toStdString();

	const char *filename = name.c_str();	

	const unsigned THETA = 90;
	const unsigned PHI = 180;

	brdf = new double[PHI*THETA*THETA*3];

	FILE *f = fopen(filename, "rb");
	if (!f)
		exit(1);

	//skip the header
	fseek ( f , 84 , SEEK_SET );

	int i = 0;
	float *c = new float[3];

	for (int phi_d = 0; phi_d < PHI; phi_d++)
	  for (int theta_h = 0; theta_h < THETA; theta_h++)
		for (int theta_d = 0; theta_d < THETA; theta_d++) {
		  
			fread(c, sizeof(float), 3, f);

			int ind = phi_d + theta_d*PHI + theta_h*THETA*THETA;

			if(c[0] >= 0. && c[1] >= 0. && c[2] >= 0.){
				brdf[ind] = c[0];
				brdf[ind + THETA*THETA*PHI] = c[1];	
				brdf[ind + THETA*THETA*(PHI*2)] = c[2];
			}else{
				brdf[ind] = 0.;
				brdf[ind + THETA*THETA*PHI] = 0.;	
				brdf[ind + THETA*THETA*(PHI*2)] = 0.;
			}
		//	cout << c[0] << " " << c[1] << " " << c[2] << endl;

		}

	delete []c;

	fclose(f);

	cout << "Measured Data read finished." << endl;

	set3DTexture();

//	setMeasuredPoints();

}

void
CarPaintBRDFDatabase::writeXML(QDomDocument *doc, QDomElement *parent){
	
	QDomElement material = doc->createElement("Material");

	QDomElement type = doc->createElement("Type");
	type.appendChild(doc->createTextNode(brdfType));

	QDomElement url = doc->createElement("URL");
	url.appendChild(doc->createTextNode("http://www.mpi-inf.mpg.de/~guenther/carpaint/"));

	QDomElement name = doc->createElement("Name");
	name.appendChild(doc->createTextNode(materialName));

	QDomElement format = doc->createElement("Format");
	format.appendChild(doc->createTextNode("isotropic"));
	
	material.appendChild(type);
	material.appendChild(url);
	material.appendChild(name);
	material.appendChild(format);

	parent->appendChild(material);


}



/*
void 
CarPaintBRDFDatabase::setMeasuredPoints(float phi_in, float theta_in, std::vector<Ogre::Vector3> *points){	
	int n = 16;

	Ogre::Real cr(1.);

	if(cubeRoot)
		cr = (Ogre::Real)0.33;
	
	for (int k = 0; k < n; k++) 
	{
		double theta_out = k * 0.5 * Ogre::Math::PI / n;
		for (int l = 0; l < 4*n; l++) 
		{
			// if both theta are not at grazing angles...
//			if((theta_in <= maxTheta)&&(theta_out <= maxTheta)){

				double phi_out = l * 2.0 * Ogre::Math::PI / (4*n);
				double red,green,blue;
				lookup_brdf_val(brdf, theta_in, phi_in, theta_out, phi_out, red, green, blue);

				// if ALL measures are positive..
				if((red >= 0)&&(green >= 0)&&(blue >= 0)){
					switch(displayChannel){
//							case 1: // TO SOLVE : instead of all the channels we are using the red one!!!!
//								--- Adria. Why so? ---
//								points->push_back(Ogre::Vector3(spherical2cart(Ogre::Vector2(phi_out,theta_out))*red));
//								break;
							case 0:
								points->push_back(Ogre::Vector3(spherical2cart(Ogre::Vector2(phi_out,theta_out))*Ogre::Math::Pow(red,cr)));
								break;
							case 1:
								points->push_back(Ogre::Vector3(spherical2cart(Ogre::Vector2(phi_out,theta_out))*Ogre::Math::Pow((Ogre::Real)green,cr)));
								break;
							case 2:
								points->push_back(Ogre::Vector3(spherical2cart(Ogre::Vector2(phi_out,theta_out))*Ogre::Math::Pow(blue,cr)));
								break;
						}
				}
		}
	}
}
*/