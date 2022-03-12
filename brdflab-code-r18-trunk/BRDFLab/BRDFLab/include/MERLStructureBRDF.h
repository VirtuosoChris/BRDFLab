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

#ifndef MERLStructureBRDF_H
#define MERLStructureBRDF_H

#include "MeasuredBRDF.h" 

class MERLStructureBRDF: public MeasuredBRDF
{

public:

	MERLStructureBRDF(QString filename, int tResolution, int pResolution, double red, double green, double blue);

	~MERLStructureBRDF(){
		if(brdf != NULL)
			delete []brdf;

		// clean texture
		if(Ogre::TextureManager::getSingleton().resourceExists("3DMeasuredData")){
			Ogre::TextureManager::getSingleton().remove("3DMeasuredData");
		}

		if(Ogre::TextureManager::getSingleton().resourceExists("3DMaxValues")){
			Ogre::TextureManager::getSingleton().remove("3DMaxValues");
		}

	}

	
	virtual void writeXML(QDomDocument *doc, QDomElement *parent)=0;
	
	virtual void readMeasuredData()=0;
//	virtual void setMeasuredPoints(float phi_in, float theta_in, std::vector<Ogre::Vector3> *points)=0;
	virtual void getFittingInformation(std::vector<Ogre::Vector3> *u, std::vector<Ogre::Vector3> *v, double* &y, int &ySize);

	void setMeasuredPoints(float phi_in, float theta_in, std::vector<Ogre::Vector3> *points, int mode);

	void set3DTexture();
	void setCubeRoot(bool cr);
	void changeDisplayChannel(int x);

protected:

	void cross_product (double* v1, double* v2, double* out);
	void normalize(double* v);
	void rotate_vector(double* vector, double* axis, double angle, double* out);
	void std_coords_to_half_diff_coords(double theta_in, double fi_in, double theta_out, double fi_out,
								double& theta_half,double& fi_half,double& theta_diff,double& fi_diff );
	inline int theta_half_index(double theta_half);
	inline int theta_diff_index(double theta_diff);
	inline int phi_diff_index(double phi_diff);
	void lookup_brdf_val(double *brdf, double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double& red_val,double& green_val,double& blue_val);
	void lookup_brdf_val_without_scaling(float *brdf, double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double& red_val,double& green_val,double& blue_val);
	bool read_brdf(const char *filename, double* &brdf);



	double redScale;
	double greenScale; 
	double blueScale; 

	double *brdf;


};	



#endif