


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


#include "imdebug.h"

#include "MERLStructureBRDF.h"


#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <vector>
#include <string>

//#include <iostream>
using namespace std;


#define BRDF_SAMPLING_RES_THETA_H       90
#define BRDF_SAMPLING_RES_THETA_D       90
#define BRDF_SAMPLING_RES_PHI_D         360


#define M_PI	3.1415926535897932384626433832795



MERLStructureBRDF::MERLStructureBRDF(QString filename, int tResolution, int pResolution, double red, double green, double blue):
	MeasuredBRDF(3,filename, tResolution, pResolution),
	redScale(red),
	greenScale(green),
	blueScale(blue),
	brdf(NULL)
{
	
}



// cross product of two vectors
void 
MERLStructureBRDF::cross_product (double* v1, double* v2, double* out)
{
	out[0] = v1[1]*v2[2] - v1[2]*v2[1];
	out[1] = v1[2]*v2[0] - v1[0]*v2[2];
	out[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

// normalize vector
void 
MERLStructureBRDF::normalize(double* v)
{
	// normalize
	double len = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
	v[0] = v[0] / len;
	v[1] = v[1] / len;
	v[2] = v[2] / len;
}

// rotate vector along one axis
void 
MERLStructureBRDF::rotate_vector(double* vector, double* axis, double angle, double* out)
{
	double temp;
	double cross[3];
	double cos_ang = cos(angle);
	double sin_ang = sin(angle);

	out[0] = vector[0] * cos_ang;
	out[1] = vector[1] * cos_ang;
	out[2] = vector[2] * cos_ang;

	temp = axis[0]*vector[0]+axis[1]*vector[1]+axis[2]*vector[2];
	temp = temp*(1.0-cos_ang);

	out[0] += axis[0] * temp;
	out[1] += axis[1] * temp;
	out[2] += axis[2] * temp;

	cross_product (axis,vector,cross);
	
	out[0] += cross[0] * sin_ang;
	out[1] += cross[1] * sin_ang;
	out[2] += cross[2] * sin_ang;
}


// convert standard coordinates to half vector/difference vector coordinates
void 
MERLStructureBRDF::std_coords_to_half_diff_coords(double theta_in, double fi_in, double theta_out, double fi_out,
								double& theta_half,double& fi_half,double& theta_diff,double& fi_diff )
{

	// compute in vector
	double in_vec_z = cos(theta_in);
	double proj_in_vec = sin(theta_in);
	double in_vec_x = proj_in_vec*cos(fi_in);
	double in_vec_y = proj_in_vec*sin(fi_in);
	double in[3]= {in_vec_x,in_vec_y,in_vec_z};
	normalize(in);


	// compute out vector
	double out_vec_z = cos(theta_out);
	double proj_out_vec = sin(theta_out);
	double out_vec_x = proj_out_vec*cos(fi_out);
	double out_vec_y = proj_out_vec*sin(fi_out);
	double out[3]= {out_vec_x,out_vec_y,out_vec_z};
	normalize(out);


	// compute halfway vector
	double half_x = (in_vec_x + out_vec_x)/2.0f;
	double half_y = (in_vec_y + out_vec_y)/2.0f;
	double half_z = (in_vec_z + out_vec_z)/2.0f;
	double half[3] = {half_x,half_y,half_z};
	normalize(half);

	// compute  theta_half, fi_half
	theta_half = acos(half[2]);
	fi_half = atan2(half[1], half[0]);


	double bi_normal[3] = {0.0, 1.0, 0.0};
	double normal[3] = { 0.0, 0.0, 1.0 };
	double temp[3];
	double diff[3];

	// compute diff vector
	rotate_vector(in, normal , -fi_half, temp);
	rotate_vector(temp, bi_normal, -theta_half, diff);
	
	// compute  theta_diff, fi_diff	
	theta_diff = acos(diff[2]);
	fi_diff = atan2(diff[1], diff[0]);

}


// Lookup theta_half index
// This is a non-linear mapping!
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int 
MERLStructureBRDF::theta_half_index(double theta_half)
{
	if (theta_half <= 0.0)
		return 0;
	double theta_half_deg = ((theta_half / (M_PI/2.0))*BRDF_SAMPLING_RES_THETA_H);
	double temp = theta_half_deg*BRDF_SAMPLING_RES_THETA_H;
	temp = sqrt(temp);
	int ret_val = (int)temp;
	if (ret_val < 0) ret_val = 0;
	if (ret_val >= BRDF_SAMPLING_RES_THETA_H)
		ret_val = BRDF_SAMPLING_RES_THETA_H-1;
	return ret_val;
}


// Lookup theta_diff index
// In:  [0 .. pi/2]
// Out: [0 .. 89]
inline int 
MERLStructureBRDF::theta_diff_index(double theta_diff)
{
	int tmp = int(theta_diff / (M_PI * 0.5) * BRDF_SAMPLING_RES_THETA_D);
	if (tmp < 0)
		return 0;
	else if (tmp < BRDF_SAMPLING_RES_THETA_D - 1)
		return tmp;
	else
		return BRDF_SAMPLING_RES_THETA_D - 1;
}


// Lookup phi_diff index
inline int 
MERLStructureBRDF::phi_diff_index(double phi_diff)
{
	// Because of reciprocity, the BRDF is unchanged under
	// phi_diff -> phi_diff + M_PI
	if (phi_diff < 0.0)
		phi_diff += M_PI;

	// In: phi_diff in [0 .. pi]
	// Out: tmp in [0 .. 179]
	int tmp = int(phi_diff / M_PI * BRDF_SAMPLING_RES_PHI_D / 2);
	if (tmp < 0)	
		return 0;
	else if (tmp < BRDF_SAMPLING_RES_PHI_D / 2 - 1)
		return tmp;
	else
		return BRDF_SAMPLING_RES_PHI_D / 2 - 1;
}


// Given a pair of incoming/outgoing angles, look up the BRDF.
void 
MERLStructureBRDF::lookup_brdf_val(double *brdf, double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double& red_val,double& green_val,double& blue_val)
{
	// Convert to halfangle / difference angle coordinates
	double theta_half, fi_half, theta_diff, fi_diff;
	
	std_coords_to_half_diff_coords(theta_in, fi_in, theta_out, fi_out,
		       theta_half, fi_half, theta_diff, fi_diff);


	// Find index.
	// Note that phi_half is ignored, since isotropic BRDFs are assumed
	int ind = phi_diff_index(fi_diff) +
		  theta_diff_index(theta_diff) * BRDF_SAMPLING_RES_PHI_D / 2 +
		  theta_half_index(theta_half) * BRDF_SAMPLING_RES_PHI_D / 2 *
					         BRDF_SAMPLING_RES_THETA_D;

	red_val = brdf[ind] * redScale;
	green_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D/2] * greenScale;
	blue_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D] * blueScale;

	
	if (red_val < 0.0 || green_val < 0.0 || blue_val < 0.0){
		//fprintf(stderr, "Below horizon.\n");
		/*red_val = 0.;
		green_val = 0.;
		blue_val = 0;*/
	}
}

/*
// Given a pair of incoming/outgoing angles, look up the BRDF.
void 
MERLStructureBRDF::lookup_brdf_val_without_scaling(float *brdf, double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double& red_val,double& green_val,double& blue_val)
{
	// Convert to halfangle / difference angle coordinates
	double theta_half, fi_half, theta_diff, fi_diff;
	
	std_coords_to_half_diff_coords(theta_in, fi_in, theta_out, fi_out,
		       theta_half, fi_half, theta_diff, fi_diff);


	// Find index.
	// Note that phi_half is ignored, since isotropic BRDFs are assumed
	int ind = phi_diff_index(fi_diff) +
		  theta_diff_index(theta_diff) * BRDF_SAMPLING_RES_PHI_D / 2 +
		  theta_half_index(theta_half) * BRDF_SAMPLING_RES_PHI_D / 2 *
					         BRDF_SAMPLING_RES_THETA_D;

	red_val = brdf[ind];
	green_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D/2];
	blue_val = brdf[ind + BRDF_SAMPLING_RES_THETA_H*BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_PHI_D];

	
	if (red_val < 0.0 || green_val < 0.0 || blue_val < 0.0){
		//fprintf(stderr, "Below horizon.\n");
		//red_val = 0.;
		//green_val = 0.;
		//blue_val = 0;
	}
}
*/




// Read BRDF data
bool 
MERLStructureBRDF::read_brdf(const char *filename, double* &brdf)
{
	FILE *f = fopen(filename, "rb");
	if (!f)
		return false;

	int dims[3];
	fread(dims, sizeof(int), 3, f);
	int n = dims[0] * dims[1] * dims[2];
	if (n != BRDF_SAMPLING_RES_THETA_H *
		 BRDF_SAMPLING_RES_THETA_D *
		 BRDF_SAMPLING_RES_PHI_D / 2) 
	{
		fprintf(stderr, "Dimensions don't match\n");
		fclose(f);
		return false;
	}

	brdf = (double*) malloc (sizeof(double)*3*n);
	fread(brdf, sizeof(double), 3*n, f);

	fclose(f);

	set3DTexture();

	return true;
}


void 
MERLStructureBRDF::getFittingInformation(std::vector<Ogre::Vector3> *u, std::vector<Ogre::Vector3> *v, double* &y, int &ySize){


	// matusik doesn't care about measurements at grazing angles (more than 80 degrees)
	double maxTheta = 80 * Ogre::Math::PI / 180;

	// we will store the nChannels*valid_directions, as we don't know previously the number of valid_directions that will appear
	// we create the temporal double vector
	std::vector<double> yTemp;

	int j = 0;

	float phiDivisions = 64.;
	float thetaDivisions = 16.;


	for (int i = 0; i < thetaDivisions; i++) 
	{
	    double theta_in = i * 0.5 * Ogre::Math::PI / thetaDivisions;
	   // for (int j = 0; j < phiDivisions; j++) // we are dealing with isotropic materials 
		{
			double phi_in = j * 2.0 * Ogre::Math::PI / (phiDivisions);
			for (int k = 0; k < thetaDivisions; k++) 
			{
				double theta_out = k * 0.5 * Ogre::Math::PI / thetaDivisions;
				for (int l = 0; l < phiDivisions; l++) 
				{
					// if both theta are not at grazing angles...
					if((theta_in <= maxTheta)&&(theta_out <= maxTheta))
					{

						double phi_out = l * 2.0 * Ogre::Math::PI / (phiDivisions);
						double red,green,blue;
						lookup_brdf_val(brdf, theta_in, phi_in, theta_out, phi_out, red, green, blue);

						// if ALL measures are positive..
						if((red > 0)&&(green > 0)&&(blue > 0)){

							//precompute incident and outgoing directions
							Ogre::Vector3 uDir(spherical2cart(Ogre::Vector2(phi_in,theta_in)));
							uDir.normalise();
							Ogre::Vector3 vDir(spherical2cart(Ogre::Vector2(phi_out,theta_out)));
							vDir.normalise();

							//push directions
							u->push_back(uDir);
							v->push_back(vDir);

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

	// fullfill the vector that will be used for optimization with the measured data
	ySize = yTemp.size();
	y = new double[ySize];

	for(unsigned int i=0; i<yTemp.size(); i++){
		y[i] = yTemp[i];
	}

	//clean temporaly data
	yTemp.clear();


}

void 
MERLStructureBRDF::setMeasuredPoints(float phi_in, float theta_in, std::vector<Ogre::Vector3> *points, int mode){

	Ogre::Real cr(1.);

	if(cubeRoot)
		cr = (Ogre::Real)0.33;

	// local variables.
	int thetaResolution = 16;
	int phiResolution = 64;
	
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
//				if((red >= 0)&&(green >= 0)&&(blue >= 0)){

				float value;

				switch(displayChannel){
					case 0:
						if(red >= 0.)
							value = red;
						break;
					case 1:
						if(green >= 0.)
							value = green;
						break;
					case 2:
						if(blue >= 0.)
							value = blue;
						break;
				}


				switch(mode){

					case 1:
						value *= Ogre::Math::Pow(value,0.33);
						break;
					case 2:
						value *= log(value+1.);
						break;
				}

				if(value>0.)
					points->push_back(Ogre::Vector3(spherical2cart(Ogre::Vector2(phi_out,theta_out))*value));
			
		}
	}
}


void
MERLStructureBRDF::set3DTexture(){


	int texSize = BRDF_SAMPLING_RES_THETA_D*BRDF_SAMPLING_RES_THETA_H*(180);
	float *texInfo = new float[texSize*3];

	int count = 0;
	float j = 0.;

	float *maxValue = new float[BRDF_SAMPLING_RES_THETA_H*3];

	for(int i=0; i<BRDF_SAMPLING_RES_THETA_H*3; i++){
		maxValue[i] = 0.;
	}


	for (int i = 0; i < BRDF_SAMPLING_RES_THETA_H; i++) 
	{
	    double theta_in = i * 0.5 * Ogre::Math::PI / BRDF_SAMPLING_RES_THETA_D;
	   // for (int j = 0; j < phiResolution; j++) // we are dealing with isotropic materials 
		{
			double phi_in = j * 2.0 * Ogre::Math::PI / (phiResolution);
			for (int k = 0; k < BRDF_SAMPLING_RES_THETA_D; k++) 
			{
				double theta_out = k * 0.5 * Ogre::Math::PI / BRDF_SAMPLING_RES_THETA_D;
				for (int l = 0; l < 180; l++) 
				{
						double phi_out = l * 2.0 * Ogre::Math::PI / (180);
						double red,green,blue;
						lookup_brdf_val(brdf, theta_in, phi_in, theta_out, phi_out, red, green, blue);

						texInfo[count++] = red;
						texInfo[count++] = green;
						texInfo[count++] = blue;

						// test max
						if(red > maxValue[i*3+0])
							maxValue[i*3+0] = red;
						if(green > maxValue[i*3+1])
							maxValue[i*3+1] = green;
						if(blue > maxValue[i*3+2])
							maxValue[i*3+2] = blue;
				}
			}
	    }
	}

/*
	for(int i=0;i<count; i++)
		if(texInfo[i]<0)
			cout << "Problema: " << texInfo[i] << endl;
*/
	// construct texture
	Ogre::String texName("3DMeasuredData");

	if(Ogre::TextureManager::getSingleton().resourceExists(texName)){
		Ogre::TextureManager::getSingleton().remove(texName);
	}

	// create Texture
	Ogre::DataStreamPtr indirectSteam = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(texInfo,texSize * 3 * sizeof(float)));
	
	Ogre::Image *intensityImage = new Ogre::Image();
	
	intensityImage->loadRawData(indirectSteam,180,BRDF_SAMPLING_RES_THETA_H,BRDF_SAMPLING_RES_THETA_D,Ogre::PF_FLOAT32_RGB,1,0);


	Ogre::TextureManager::getSingleton().loadImage(texName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		*intensityImage,
		Ogre::TEX_TYPE_3D,
		0,
		1.0f,
		true,
		Ogre::PF_FLOAT32_RGB);


	delete []texInfo;

	// construct texture
	Ogre::String maxValuesTexName("3DMaxValues");

	if(Ogre::TextureManager::getSingleton().resourceExists(maxValuesTexName)){
		Ogre::TextureManager::getSingleton().remove(maxValuesTexName);
	}

	// create Texture
	Ogre::DataStreamPtr maxSteam = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(maxValue,BRDF_SAMPLING_RES_THETA_H * 3 * sizeof(float)));

	Ogre::Image *maxImage = new Ogre::Image();

	maxImage->loadRawData(maxSteam,BRDF_SAMPLING_RES_THETA_H,1,1,Ogre::PF_FLOAT32_RGB,1,0);


	Ogre::TextureManager::getSingleton().loadImage(maxValuesTexName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		*maxImage,
		Ogre::TEX_TYPE_1D,
		0,
		1.0f,
		true,
		Ogre::PF_FLOAT32_RGB);

//	imdebug("rgb b=32f w=%d h=%d %p", BRDF_SAMPLING_RES_THETA_H, 1, maxValue);

	delete []maxValue;

	// set the texture to the display lobe material

	// add the texture to the material
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Display3DTextureBRDF");

	/// clean all the textures on the material.
	mat ->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	/// set the hash texture to the material
	Ogre::TextureUnitState *texUnitState =  mat->getTechnique(0)->getPass(0)->createTextureUnitState(texName);
	Ogre::TextureUnitState *texUnitState2 =  mat->getTechnique(0)->getPass(0)->createTextureUnitState(maxValuesTexName);

	/// disable filtering on both textures
	texUnitState->setTextureFiltering(Ogre::TFO_TRILINEAR );
//	texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_WRAP);
//	texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_BORDER);
//	texUnitState->setTextureFiltering(Ogre::FT_MAG,Ogre::TFO_BILINEAR );

	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)1.);
	gpuVertexParam->setNamedConstant("brdfData",0);
	gpuVertexParam->setNamedConstant("maxValues",1);

	// set the texture to the point light rendering material

	mat = Ogre::MaterialManager::getSingleton().getByName("PointLight3DTextureBRDF");

	/// clean all the textures on the material.
	mat ->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	/// set the hash texture to the material
	texUnitState =  mat->getTechnique(0)->getPass(0)->createTextureUnitState(texName);

	/// disable filtering on both textures
	texUnitState->setTextureFiltering(Ogre::TFO_TRILINEAR );
//	texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_WRAP);
//	texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_BORDER);
//	texUnitState->setTextureFiltering(Ogre::FT_MAG,Ogre::TFO_BILINEAR );

	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	gpuFragmentParam->setNamedConstant("brdfData",0);
	gpuFragmentParam->setNamedConstant("channelsToDisplay",(Ogre::Real)3.);
}

void 
MERLStructureBRDF::setCubeRoot(bool cr){

	cubeRoot = cr;

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Display3DTextureBRDF");
		
	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	if(cubeRoot)
		gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)0.33);
	else
		gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)1.);
}



void 
MERLStructureBRDF::changeDisplayChannel(int x){

	displayChannel = x;

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Display3DTextureBRDF");
		
	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	switch(x){
		case 0:
			gpuVertexParam->setNamedConstant("displayChannel",Ogre::Vector3(1.,0.,0.));
			break;
		case 1:
			gpuVertexParam->setNamedConstant("displayChannel",Ogre::Vector3(0.,1.,0.));
			break;
		case 2:
			gpuVertexParam->setNamedConstant("displayChannel",Ogre::Vector3(0.,0.,1.));
			break;
	}
}
