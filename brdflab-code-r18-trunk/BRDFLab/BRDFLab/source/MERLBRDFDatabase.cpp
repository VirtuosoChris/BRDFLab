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

#include "MERLBRDFDatabase.h"

//#include "MERLReading.h"

using namespace std;

MERLBRDFDatabase::MERLBRDFDatabase(QString filename):
	MERLStructureBRDF(filename, 90, 180, (1.0/1500.0),(1.15/1500.0),(1.66/1500.0))
{
	//90, 180

	brdfType = QString("MERLBRDFDatabase");	
	
}

void
MERLBRDFDatabase::readMeasuredData()
{
	std::string name = materialName.toStdString();

	const char *file = name.c_str();	
	
//	double *brdf;
	
	// read brdf
	if (!read_brdf(file, brdf)) 
	{
		cout << "error reading..." << endl;//		fprintf(stderr, "Error reading %s\n", filename);
		return;
	}
/*
	// matusik doesn't care about measurements at grazing angles (more than 80 degrees)
	double maxTheta = 80 * Ogre::Math::PI / 180;

	// we will store the nChannels*valid_directions, as we don't know previously the number of valid_directions that will appear
	// we create the temporal double vector
	std::vector<double> yTemp;
	
	int j = 7;

	int n=16;
	for (int i = 0; i < n; i++) 
	{
	    double theta_in = i * 0.5 * Ogre::Math::PI / n;
	   // for (int j = 0; j < 4*n; j++) 
		{
			double phi_in = j * 2.0 * Ogre::Math::PI / (4*n);
			for (int k = 0; k < n; k++) 
			{
				double theta_out = k * 0.5 * Ogre::Math::PI / n;
				for (int l = 0; l < 4*n; l++) 
				{
					// if both theta are not at grazing angles...
					if((theta_in <= maxTheta)&&(theta_out <= maxTheta)){

						double phi_out = l * 2.0 * Ogre::Math::PI / (4*n);
						double red,green,blue;
						lookup_brdf_val(brdf, theta_in, phi_in, theta_out, phi_out, red, green, blue);
	//					printf("%f %f %f\n", (float)red, (float)green, (float)blue);

						// if ALL measures are positive..
						if((red >= 0)&&(green >= 0)&&(blue >= 0)){
							
//							Ogre::Vector4 dir(phi_in,theta_in,phi_out,theta_out);
//							directions.push_back(dir);

							//precompute incident and outgoing directions
							Ogre::Vector3 uDir(spherical2cart(Ogre::Vector2(phi_in,theta_in)));
							uDir.normalise();
							Ogre::Vector3 vDir(spherical2cart(Ogre::Vector2(phi_out,theta_out)));
							vDir.normalise();

							//push directions
							u.push_back(uDir);
							v.push_back(vDir);

							switch(nChannels){
				
								case 3:
									yTemp.push_back(red);
									yTemp.push_back(green);
									yTemp.push_back(blue);
									break;
								case 1:
									yTemp.push_back(red);
									break;

							}
						}
					}
				}
			}
	    }
	}

//	delete []brdf;

	// fullfill the vector that will be used for optimization with the measured data
	ySize = yTemp.size();
	y = new double[ySize];

	for(unsigned int i=0; i<yTemp.size(); i++){
		y[i] = yTemp[i];
	}

	//clean temporaly data
	yTemp.clear();

//	materialName = materialName.section('/',-1);
*/
	cout << "Measured Data read finished." << endl;

}

void
MERLBRDFDatabase::writeXML(QDomDocument *doc, QDomElement *parent){
	
	QDomElement material = doc->createElement("Material");

	QDomElement type = doc->createElement("Type");
	type.appendChild(doc->createTextNode(brdfType));

	QDomElement url = doc->createElement("URL");
	url.appendChild(doc->createTextNode("http://www.merl.com/brdf/"));

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
MERLBRDFDatabase::setMeasuredPoints(float phi_in, float theta_in, std::vector<Ogre::Vector3> *points){
	
	int n = 16;

	Ogre::Real cr(1.);

	if(cubeRoot)
		cr = (Ogre::Real)0.33;

	for (int k = 0; k < thetaResolution; k++) 
	{
		double theta_out = k * 0.5 * Ogre::Math::PI / thetaResolution;
		for (int l = 0; l < phiResolution; l++) 
		{
			// if both theta are not at grazing angles...
//			if((theta_in <= maxTheta)&&(theta_out <= maxTheta)){

				double phi_out = l * 2.0 * Ogre::Math::PI / (phiResolution);
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
