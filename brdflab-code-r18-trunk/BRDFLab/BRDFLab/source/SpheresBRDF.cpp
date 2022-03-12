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

#include "SpheresBRDF.h"


#include "imdebug.h"


#include <iostream>
using namespace std;

int 
SpheresBRDF::setMaterialInfo(Ogre::String materialName){

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);

	Ogre::TextureUnitState *heightTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState(textureName);
	heightTexture->setTextureFiltering(Ogre::TFO_NONE);


	Ogre::GpuProgramParametersSharedPtr gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParam->setNamedConstant("texWidth",(Ogre::Real)texWidth);
	gpuParam->setNamedConstant("nSpheres",(Ogre::Real)nSpheres);

	//gpuParam->setNamedConstant("sphere",Ogre::Vector4(0.5,0.5,0.5,0.2));

	gpuParam->setNamedConstant("data",0);

//	if(materialName == "FirstSpheresSimulation"){
		heightTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("SpheresIndexTexture");
		heightTexture->setTextureFiltering(Ogre::TFO_NONE);

		heightTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("SpheresGridTexture");
		heightTexture->setTextureFiltering(Ogre::TFO_NONE);

		gpuParam->setNamedConstant("width",gridSide);
		gpuParam->setNamedConstant("height",gridSide);
		gpuParam->setNamedConstant("depth",gridSide);

		gpuParam->setNamedConstant("indexWidth",(Ogre::Real)texSide);
		gpuParam->setNamedConstant("gridWidth",(Ogre::Real)gGridTextureSide);

		gpuParam->setNamedConstant("index",1);
		gpuParam->setNamedConstant("grid",2);
//	}

	return 3;

}


Ogre::Vector4
SpheresBRDF::createOneSphere(){

	float radiusCorrection = 1-radius*2.;

	return Ogre::Vector4(Ogre::Math::UnitRandom()*radiusCorrection+radius,Ogre::Math::UnitRandom()*radiusCorrection+radius,Ogre::Math::UnitRandom()*radiusCorrection+radius,radius);
}


/**
This method, given two spheres, return if they intersect each other or not.


*/
bool
SpheresBRDF::spheresIntersect(Ogre::Vector4 sph1, Ogre::Vector4 sph2){

	Ogre::Vector3 vector(sph1[0]-sph2[0],sph1[1]-sph2[1],sph1[2]-sph2[2]);

	float distance = vector.length();

	if(distance < sph1[3] + sph2[3])
		return true;
	else
		return false;
}



/**
This method creates randomly distributed spheres over the range 0..1 on X,Y,Z and radius.

*/
float*
SpheresBRDF::createSpheres(){

	float *info;

	if(nSpheres<= 0.)
		return NULL;

	// if we allow overlaping, we can have the desired number of spheres, without any problem.
	if(overlap){

		texWidth = ceil(sqrt((float)nSpheres));

		int size = 4.*texWidth*texWidth;
/*
		if(info != NULL){
			delete info;
			info = NULL;
		}
*/
		info = new float[size];

		for(int i=0; i<texWidth*texWidth; i++){

			if(i<nSpheres){
				// create the sphere
				Ogre::Vector4 sph = createOneSphere();

				// put the sphere in a proper format
				info[i*4+0] = sph[0];
				info[i*4+1] = sph[1];
				info[i*4+2] = sph[2];
				info[i*4+3] = sph[3];
			}else{
				info[i*4+0] = 0.;
				info[i*4+1] = 0.;
				info[i*4+2] = 0.;
				info[i*4+3] = 0.;
			}
		}

		// only nSpheres value is correct!!! The information left on the info array is set to zero.



	}else{ // without overlaping, we can't have as many spheres as we want.

		const int MAX_TESTS = 100;
		bool enoughSpace = true;
		std::vector<Ogre::Vector4> spheres;
		int nSpheresCreated = 0;

		while((nSpheresCreated < nSpheres)&&(enoughSpace)){

			// create one sphere

			int tryCount = 0;

			// test MAX_TEST times to fit a sphere on the avaliable space
			while(tryCount < MAX_TESTS){

				// create a new sphere.
				Ogre::Vector4 sph = createOneSphere();
				bool intersect = false;
				int i = 0;

				while((!intersect)&&(i<spheres.size())){
					
					intersect = spheresIntersect(sph,spheres[i]);

					i++;
				}

				if(intersect)
					tryCount++;
				else{
					spheres.push_back(sph);
					tryCount = MAX_TESTS + 1;
				}
				
			}

			if (tryCount == MAX_TESTS)
				enoughSpace = false;
			else
				nSpheresCreated++;
				
		}

		if(!enoughSpace)
			nSpheres = nSpheresCreated;
		else
			if(nSpheres != nSpheresCreated)
				cerr << "nSpheres MUST be equal to nSpheresCreated" << endl;

		// transform from the vector to a float* to store the spheres.
		texWidth = ceil(sqrt((float)nSpheres));

		int size = 4.*texWidth*texWidth;
/*
		if(info != NULL){
			delete info;
			info = NULL;
		}
*/
		info = new float[size];

		for(int i=0; i<texWidth*texWidth; i++){

			if(i<nSpheres){
				// create the sphere
				Ogre::Vector4 sph = createOneSphere();

				// put the sphere in a proper format
				info[i*4+0] = spheres[i][0];
				info[i*4+1] = spheres[i][1];
				info[i*4+2] = spheres[i][2];
				info[i*4+3] = spheres[i][3];
			}else{
				info[i*4+0] = 0.;
				info[i*4+1] = 0.;
				info[i*4+2] = 0.;
				info[i*4+3] = 0.;
			}
		}

		spheres.clear();

		//dynamic_cast<SpheresSimulationParameters*>(guiParams[1])->setNSpheres(nSpheres);


	}

	//create texture
	if(Ogre::TextureManager::getSingleton().resourceExists(textureName)){

		Ogre::TextureManager::getSingleton().unload(textureName);

		Ogre::TextureManager::getSingleton().remove(textureName);
	}

	Ogre::DataStreamPtr spheresStream = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(info,4* texWidth * texWidth * sizeof(float)));
	
	Ogre::Image *spheresImage = new Ogre::Image();
	
	spheresImage->loadRawData(spheresStream,texWidth,texWidth,1,Ogre::PF_FLOAT32_RGBA,1,0);

	Ogre::TextureManager::getSingleton().loadImage(textureName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		*spheresImage,
		Ogre::TEX_TYPE_2D,
		0,
		1.0f,
		true,
		Ogre::PF_FLOAT32_RGBA);

	//Ogre::TextureManager::getSingleton().load(textureName);

//	delete []info;
	return info;


}

bool
sphereBoxIntersection(float Cx, float Cy, float Cz, float r, int x, int y, int z){
	float dmin = 0.;

	if( Cx < x ) 
		dmin += Ogre::Math::Sqr( Cx - x );
	else if( Cx > x+1 ) 
		dmin += Ogre::Math::Sqr( Cx - (x+1) );

	if( Cy < y ) 
		dmin += Ogre::Math::Sqr( Cy - y );
	else if( Cy > y+1 ) 
		dmin += Ogre::Math::Sqr( Cy - (y+1) );

	if( Cz < z ) 
		dmin += Ogre::Math::Sqr( Cz - z );
	else if( Cz > z+1 ) 
		dmin += Ogre::Math::Sqr( Cz - (z+1) );

	if( dmin <= r ) 
		return true;
	else
		return false;


}


void
SpheresBRDF::constructGrid(){
	
	float *info = createSpheres();

	// define grid size
	gridSide = Ogre::Math::Pow((Ogre::Real)nSpheres,0.33);

	cout << "Grid side: " << gridSide << endl;

	int gridSize = gridSide*gridSide*gridSide;

	// create a vector of index
	std::vector< std::vector<int> > sphGrid;

	// sphGrid[nPosicioGrid][nSphere]
	
	//sphGrid.push_back(z*gridSide*gridSide+y*gridSide+x);

	for(int z = 0; z< gridSide; z++)
		for(int y = 0; y< gridSide; y++)
			for(int x = 0; x< gridSide; x++){
				
				std::vector<int> pos;
				sphGrid.push_back(pos);

				// test all the spheres if they intersect or are inside the sphGrid[i]
				
				for(int i = 0; i < nSpheres; i++){
					if(sphereBoxIntersection(gridSide*info[i*4],gridSide*info[i*4+1],gridSide*info[i*4+2],info[i*4+3],x,y,z))
						sphGrid[z*gridSide*gridSide+y*gridSide+x].push_back(i);

				}
			}


	cout<< sphGrid.size() << endl;

//	sphereBoxIntersection(float Cx, float Cy, float Cz, float r, int x, int y, int z){

	// create texture to store the info on the GRID.

	// we will use a INT RGBA, storing the "adress" of 4 spheres on each pixel, this way with a simple texture fetch, we can get the pointers to 4 spheres at a
	// time, if there are no spheres -1 is set.

	int texPositions = 0;

	for(unsigned int i = 0; i < sphGrid.size(); i++){
		texPositions += (sphGrid[i].size()+1)/4;

		if((sphGrid[i].size()+1) % 4 != 0)
			texPositions +=1;
	}

	texSide = ceil(Ogre::Math::Sqrt((Ogre::Real)texPositions));

	int texSize = texSide*texSide;

	short *indexTexture = new short[texSize*4];

	gGridTextureSide = ceil(Ogre::Math::Sqrt(sphGrid.size()));
	int gGridTextureSize = gGridTextureSide*gGridTextureSide;

	short *gridTexture = new short[gGridTextureSize];

	// initialize the texture to -1
	for(int i=0;i<texSize*4; i++)
		indexTexture[i] = 65535;

	// load the data onto the texture
	int count = 0;
	for(unsigned int i = 0; i < sphGrid.size(); i++){
		
		if(sphGrid[i].size() > 0){
			gridTexture[i] = count/4;

			for(unsigned int k = 0; k < sphGrid[i].size(); k++){
				indexTexture[count+k] = sphGrid[i][k];
			}

			count += ((sphGrid[i].size()+1)/4)*4;

			if((sphGrid[i].size()+1) % 4 != 0)
				count +=4;
		}else
			gridTexture[i] = 65535;


	}

	Ogre::String indexTextureName("SpheresIndexTexture");


	if(Ogre::TextureManager::getSingleton().resourceExists(indexTextureName)){
		Ogre::TextureManager::getSingleton().remove(indexTextureName);
	}

	Ogre::DataStreamPtr iTextureStream = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(indexTexture,4* texSide * texSide * sizeof(short)));
	
	Ogre::Image *iTextureImage = new Ogre::Image();
	
	iTextureImage->loadRawData(iTextureStream,texSide,texSide,1,Ogre::PF_SHORT_RGBA,1,0);

	Ogre::TextureManager::getSingleton().loadImage(indexTextureName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		*iTextureImage,
		Ogre::TEX_TYPE_2D,
		0,
		1.0f,
		true,
		Ogre::PF_SHORT_RGBA);



	// create 2D grid texture, with a reference position on the other texture

	Ogre::String gridTextureName("SpheresGridTexture");


	if(Ogre::TextureManager::getSingleton().resourceExists(gridTextureName)){
		Ogre::TextureManager::getSingleton().remove(gridTextureName);
	}

	Ogre::DataStreamPtr gTextureStream = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(gridTexture, gGridTextureSide * gGridTextureSide * sizeof(short)));
	
	Ogre::Image *gTextureImage = new Ogre::Image();
	
	gTextureImage->loadRawData(gTextureStream,gGridTextureSide,gGridTextureSide,1,Ogre::PF_SHORT_L,1,0);

	Ogre::TextureManager::getSingleton().loadImage(gridTextureName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		*gTextureImage,
		Ogre::TEX_TYPE_2D,
		0,
		1.0f,
		true,
		Ogre::PF_SHORT_L);


//	imdebug("rgba b=16 w=%d h=%d %p", texSide, texSide, indexTexture);
//	imdebug("lum b=16 w=%d h=%d %p", gGridTextureSide, gGridTextureSide, gridTexture);

	// clean plenty of stuff!!!

	for(unsigned int i = 0; i < sphGrid.size(); i++)
		sphGrid[i].clear();

	sphGrid.clear();

	delete []indexTexture;
	delete []gridTexture;
	delete[]info;

	// set the information to the display shader
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("spheresViewer");

	Ogre::TextureUnitState *heightTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState(textureName);
	heightTexture->setTextureFiltering(Ogre::TFO_NONE);

	heightTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("SpheresIndexTexture");
	heightTexture->setTextureFiltering(Ogre::TFO_NONE);

	heightTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("SpheresGridTexture");
	heightTexture->setTextureFiltering(Ogre::TFO_NONE);


	Ogre::GpuProgramParametersSharedPtr gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParam->setNamedConstant("texWidth",(Ogre::Real)texWidth);
//	gpuParam->setNamedConstant("nSpheres",(Ogre::Real)nSpheres);

	gpuParam->setNamedConstant("data",0);
	gpuParam->setNamedConstant("index",1);
	gpuParam->setNamedConstant("grid",2);


	gpuParam->setNamedConstant("width",gridSide);
	gpuParam->setNamedConstant("height",gridSide);
	gpuParam->setNamedConstant("depth",gridSide);

	gpuParam->setNamedConstant("indexWidth",(Ogre::Real)texSide);
	gpuParam->setNamedConstant("gridWidth",(Ogre::Real)gGridTextureSide);


}


void 
SpheresBRDF::writeXML(QDomDocument *doc, QDomElement *parent){
	QDomElement material = doc->createElement("Material");

	QDomElement type = doc->createElement("Type");
	type.appendChild(doc->createTextNode(brdfType));

	QDomElement params = doc->createElement("Parameters");
	params.setAttribute("nSpheres",nSpheres);
	params.setAttribute("radius",radius);

	material.appendChild(type);
	material.appendChild(params);

	parent->appendChild(material);
}

void 
SpheresBRDF::guiUpdated(){

	float radiusNew;
	int nSpheresNew;
	bool overlapNew;

	// we already have the QtVariantPropertyManager
	QList<QtProperty*> p = variantEditor->properties();

	for (unsigned int i=0; i<p.size(); i++){

		QString n(brdfType);
		n.append(" simulation");

		if(p[i]->propertyName() == n){

			QList<QtProperty *> childs = p[i]->subProperties();

			for (int j = 0; j < childs.size(); ++j) {

				if(childs.at(j)->propertyName() == "GPU Simulation Parameters"){

						QList<QtProperty *> values = childs.at(j)->subProperties();

						updateGPUSimulationParameters(values);

				}else if(childs.at(j)->propertyName() == "Model parameters"){

						QList<QtProperty *> values = childs.at(j)->subProperties();

						radiusNew = dynamic_cast<QtVariantProperty*>(values.at(0))->value().toDouble();
						nSpheresNew = dynamic_cast<QtVariantProperty*>(values.at(1))->value().toInt();
						overlapNew = dynamic_cast<QtVariantProperty*>(values.at(2))->value().toBool();

				}//else active

			}
		}
	}

	if((radius != radiusNew)||(nSpheres != nSpheresNew)||(overlap != overlapNew)){
		// update variables
		radius = radiusNew;
		nSpheres = nSpheresNew;
		overlap = overlapNew;

		// create spheres
//		createSpheres();
		constructGrid();

		// refresh.
		updateMaterial();
	}

	emit brdfModified();
	
}


void
SpheresBRDF::updateMaterial(){


	// display spheres
	switch(materialToUpdate)
	{
	case -2: 
		emit updateSurfaceViewer(1);	
		break;
	case -1: // brdf viewer
		Ogre::MaterialManager::getSingleton().getByName("Display2DTextureBRDF")->reload(); // TODO: really needed?
		emit updateBRDFViewer();
		break;
	case 0: // point light rendering

		break;
	case 1: // env. map rendering

		break;
	}



}

void SpheresBRDF::recreateParameterGUI()
{
	// we already have the QtVariantPropertyManager
	variantManager->clear();
	variantEditor->clear();

	QString n(brdfType);
	n.append(" simulation");

	QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
		n);

	// create each lobe on the GUI.
	setupGPUSimulationParameters(topItem);

	QtVariantProperty *item;

	item = variantManager->addProperty(QVariant::Char, "Model parameters");
	item->setValue("");
//	item->setEnabled(enabled);
	topItem->addSubProperty(item);

	QtVariantProperty *subItems;
	subItems = variantManager->addProperty(QVariant::Double, "Radius");
	subItems->setValue(radius);
	subItems->setAttribute("singleStep", 0.01);
	subItems->setAttribute("decimals", 3);
	subItems->setAttribute("minimum", 0.001);
	subItems->setAttribute("maximum", 1.);
	item->addSubProperty(subItems);

	subItems = variantManager->addProperty(QVariant::Int, "Num Spheres");
	subItems->setValue(nSpheres);
	subItems->setAttribute("singleStep", 1.);
	subItems->setAttribute("minimum", 0.);
	subItems->setAttribute("maximum", 100000.);
	item->addSubProperty(subItems);

	subItems = variantManager->addProperty(QVariant::Bool, "Allow Overlap");
	subItems->setValue(overlap);
	item->addSubProperty(subItems);
	
	variantEditor->addProperty(topItem);
	
	connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(guiUpdated()));
}