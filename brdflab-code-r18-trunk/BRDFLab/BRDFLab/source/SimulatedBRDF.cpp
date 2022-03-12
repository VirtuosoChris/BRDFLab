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

#include "SimulatedBRDF.h"
using namespace std;

void 
SimulatedBRDF::setCubeRoot(bool cr){

	cubeRoot = cr;

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Display2DTextureBRDF");
		
	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	if(cubeRoot)
		gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)0.33);
	else
		gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)1.);
}

void 
SimulatedBRDF::setIncidentVector(Ogre::Vector2 u){
	incidentVector = u;
}

void
SimulatedBRDF::displayBRDF(int x){

	if(x > 0){

		float *texInfo = NULL;
		float min = 0.;
		float max = 0.;
		int y = 0;

		computeBRDF(texInfo, x, y, min, max);
		
	/*	QTime *timer = new QTime();

		timer->start();
	*/	
		setDisplayTexture(texInfo, x, y, min, max);

	/*
		cout << "computing intensitys: " << timer->elapsed() << " ms" << endl;
		timer->restart();
		delete timer;
	*/
		delete []texInfo;

		// TODO: this is saved the other way around... fix it!
		textureResolution[0] = y;
		textureResolution[1] = x;

	}
}


void 
SimulatedBRDF::setDisplayTexture(float *texInfo, int x, int y, float min, float max){


	if(Ogre::TextureManager::getSingleton().resourceExists(intensityTextureName)){
		Ogre::TextureManager::getSingleton().remove(intensityTextureName);
	}

	// create Texture
	Ogre::DataStreamPtr indirectSteam = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(texInfo,x * y * sizeof(float)));
	
	Ogre::Image *intensityImage = new Ogre::Image();
	
	intensityImage->loadRawData(indirectSteam,x,y,1,Ogre::PF_FLOAT32_R,1,0);


	Ogre::TextureManager::getSingleton().loadImage(intensityTextureName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		*intensityImage,
		Ogre::TEX_TYPE_2D,
		0,
		1.0f,
		true,
		Ogre::PF_FLOAT32_R);


	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Display2DTextureBRDF");

	/// clean all the textures on the material.
	mat ->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	/// set the hash texture to the material
	Ogre::TextureUnitState *texUnitState =  mat->getTechnique(0)->getPass(0)->createTextureUnitState(intensityTextureName);

	/// disable filtering on both textures
	texUnitState->setTextureFiltering(Ogre::TFO_BILINEAR );
//	texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_WRAP);
//	texUnitState->setTextureFiltering(Ogre::FT_MAG,Ogre::TFO_BILINEAR );

	/// obtain gpu vertex shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuVertexParam->setNamedConstant("minV",(Ogre::Real)min);
	gpuVertexParam->setNamedConstant("maxV",(Ogre::Real)max);
	gpuVertexParam->setNamedConstant("thetaSize",(Ogre::Real)y);
	gpuVertexParam->setNamedConstant("brdfData",0);

}

void
SimulatedBRDF::generateFittingInformation(int thetaDivisions, int phiDivisions, std::vector<Ogre::Vector3> *u, std::vector<Ogre::Vector3> *v, double* &y, int &ySize){

	QProgressDialog *pb = new QProgressDialog(this);
	
	QLabel *text = new QLabel("Computing BRDF");

	pb->setLabel(text);
	pb->setRange(0,phiDivisions*(thetaDivisions-1));
	pb->setCancelButton(NULL);
	pb->setValue(0);
//	pb->hide();
//	pb->setWindowModality(Qt::WindowModal);
	pb->show();

	if(textureResolution[0] > 0){

		Ogre::Vector2 oldIncidentVector(incidentVector);

		ySize = (1+(textureResolution[0]-1)*textureResolution[1])*thetaDivisions*phiDivisions;
		
		y = new double[ySize];

//		std::vector<float> poma;

		int count = 0;

		float phiStep = 2.*Ogre::Math::PI/(float)phiDivisions;
		float thetaStep = Ogre::Math::PI/(2*(float)thetaDivisions);

		float phiOutStep = textureResolution[0]*2.*Ogre::Math::PI;
		float thetaOutStep = textureResolution[1]*Ogre::Math::PI/2.;

		for(int i=0; i<phiDivisions; i++){// incoming phi
			for(int j=0; j<thetaDivisions; j++){ // incoming theta
				
				float phi = phiStep*i;
				float theta = thetaStep*j;

				Ogre::Vector2 incidentVectorPass(phi,theta);

				this->setIncidentVector(incidentVectorPass);

				Ogre::Vector3 incident(BRDF::spherical2cart(incidentVectorPass));

				float *texInfo = NULL;
				float min = 0.;
				float max = 0.;

				int xResolution = textureResolution[0];
				int yResolution = textureResolution[1];

				// for each incident direction, compute the outgoing texture

				computeBRDF(texInfo, xResolution, yResolution, min, max);

				// decode the outgoing texture and fill up the direction vectors and result value.

				// set the theta = 0 value
				u->push_back(incident);
				v->push_back(BRDF::spherical2cart(Ogre::Vector2(0.,0.)));

				y[count++] = texInfo[0];
				//poma.push_back(texInfo[0]);

				for(int m=1; m<textureResolution[0]; m++){ // outgoing phi
					for(int n=0; n<textureResolution[1]; n++){ // outgoing theta
				
						float phiOut = (float)n/(float)textureResolution[1]*2.*Ogre::Math::PI;
						float thetaOut = (float)m/(float)textureResolution[0]*Ogre::Math::PI/2.;

//						uv[0] = Ogre::Math::IFloor(uv[0]/(2.*Ogre::Math::PI)*capturingWidth);//phi
//						uv[1] = Ogre::Math::IFloor((uv[1]/(Ogre::Math::PI/2.))*capturingHeight);//theta


						int position = n+ m*textureResolution[1];

						u->push_back(incident);
						v->push_back(BRDF::spherical2cart(Ogre::Vector2(phiOut,thetaOut)));
						
						
						y[count++] = (double)texInfo[position];
//						poma.push_back(texInfo[position]);

//						cout << "hola" << endl;

					}
				}

				delete []texInfo;

				pb->setValue(pb->value()+1);
/*
				if(pb->wasCanceled())
					return;
*/
//				QCoreApplication::processEvents();
				pb->update();
			}
		}

		delete pb;
		

		this->setIncidentVector(oldIncidentVector);
	}


}

void
SimulatedBRDF::set3DTexture(){

	//textureResolution[0] -> theta
	//textureResolution[1] -> phi
	
	if(textureResolution[0] > 0){

		Ogre::Vector2 oldIncidentVector(incidentVector);

		int texSize = textureResolution[0]*textureResolution[0]*textureResolution[1];
		float *textureInformation = new float[texSize*nChannels];

		int count = 0;
		float j = 0.;

//		float *maxValue = new float[textureResolution[0]*3];

//		for(int i=0; i<textureResolution[0]*3; i++){
//			maxValue[i] = 0.;
//		}


		for (int i = 0; i < textureResolution[0]; i++) 
		{
			float theta_in = i * 0.5 * Ogre::Math::PI / textureResolution[0];

			Ogre::Vector2 incidentVectorPass(0.,theta_in);

			this->setIncidentVector(incidentVectorPass);

			float *texInfo = NULL;
			float min = 0.;
			float max = 0.;

			int xResolution = textureResolution[0];
			int yResolution = textureResolution[1];

			// for each incident direction, compute the outgoing texture

			computeBRDF(texInfo, xResolution, yResolution, min, max);

			// now the BRDF is computed, only to extact the information is required.
			for (int j=0; j<yResolution; j++){
				for (int k=0; k<xResolution; k++){

					float value = texInfo[((int)textureResolution[1])*j+k];

					textureInformation[(int)(textureResolution[0]*textureResolution[1]*i+j*textureResolution[1]+k)] = value;

//					if(value > maxValue[i+0])
//						maxValue[i] = value;

				}
			}
			
			delete []texInfo;
		}




		// construct texture
		Ogre::String texName("3DMeasuredData");

		if(Ogre::TextureManager::getSingleton().resourceExists(texName)){
			Ogre::TextureManager::getSingleton().remove(texName);
		}

		// create Texture
		Ogre::DataStreamPtr indirectSteam = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(textureInformation,texSize * sizeof(float)));
		
		Ogre::Image *intensityImage = new Ogre::Image();
		
		intensityImage->loadRawData(indirectSteam,textureResolution[1],textureResolution[0],textureResolution[0],Ogre::PF_FLOAT32_R,1,0);


		Ogre::TextureManager::getSingleton().loadImage(texName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			*intensityImage,
			Ogre::TEX_TYPE_3D,
			0,
			1.0f,
			true,
			Ogre::PF_FLOAT32_R);


		delete []textureInformation;
/*
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
		*/
/*
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


		gpuVertexParam->setNamedConstant("brdfData",0);
		gpuVertexParam->setNamedConstant("maxValues",1);

		// set the texture to the point light rendering material
*/
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("PointLight3DTextureBRDF");

		/// clean all the textures on the material.
		mat ->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

		/// set the hash texture to the material
		Ogre::TextureUnitState *texUnitState =  mat->getTechnique(0)->getPass(0)->createTextureUnitState(texName);

		/// disable filtering on both textures
		texUnitState->setTextureFiltering(Ogre::TFO_TRILINEAR );
	//	texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_WRAP);
//		texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_BORDER);
	//	texUnitState->setTextureFiltering(Ogre::FT_MAG,Ogre::TFO_BILINEAR );

		Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
		gpuFragmentParam->setNamedConstant("brdfData",0);
		gpuFragmentParam->setNamedConstant("channelsToDisplay",(Ogre::Real)1.);

		incidentVector = oldIncidentVector;

	}
}

void 
SimulatedBRDF::setGUIManagers( QtVariantPropertyManager * vM, QtTreePropertyBrowser * vE )
{

	variantManager = vM;
	variantEditor = vE;

	recreateParameterGUI();

	connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(guiUpdated()));
}