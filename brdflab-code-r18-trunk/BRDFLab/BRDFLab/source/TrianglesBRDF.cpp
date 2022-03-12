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

#include "TrianglesBRDF.h"

#include <iostream>
using namespace std;


TrianglesBRDF::TrianglesBRDF():
	GPUSimulationBRDF(1,QString("Triangles"),"FirstTrianglesSimulation","IterationTrianglesSimulation",10),
	vertexTexture("vertexTexture"),
	indexTexture("indexTexture"),
	points(NULL),
	vertex(NULL),
	triangleNormal(NULL),
	vertexNormal(NULL),
	index(NULL),
	maxHeight(0),
	distribution(3),
	width(21),
	scale(0.1)

{
	brdfType = QString("Triangles");
	
	// create gui Parameters
	//guiParams.push_back(new TrianglesSimulationParameters());
	


	float min = createDistribution();
	createTrianglesFromDistribution(min);
	createTextures();
/*
	cout << "--------------------------------------" << endl;
	cout << "vertexWidth: " << width << endl;
	cout << "indexWidth: " << indexTexWidth << endl;
	cout << "nTriangles: " << nTriangles << endl;
	cout << "--------------------------------------" << endl;
*/
}

	TrianglesBRDF::~TrianglesBRDF(){
		if(Ogre::TextureManager::getSingleton().resourceExists(vertexTexture)){
			Ogre::TextureManager::getSingleton().remove(vertexTexture);
		}

		if(Ogre::TextureManager::getSingleton().resourceExists(indexTexture)){
			Ogre::TextureManager::getSingleton().remove(indexTexture);
		}

		if(points != NULL){
			delete []points;
			points = NULL;
		}

		if(vertex != NULL){
			delete []vertex;
			vertex = NULL;
		}

		if(triangleNormal != NULL){
			delete []triangleNormal;
			triangleNormal = NULL;
		}

		if(vertexNormal != NULL){
			delete []vertexNormal;
			vertexNormal = NULL;
		}

		if(index != NULL){
			delete []index;
			index = NULL;
		}

	}

	inline double RadicalInverse(int n, int base) {
		double val = 0;
		double invBase = 1. / base, invBi = invBase;
		while (n > 0) {
			// Compute next digit of radical inverse
			int d_i = (n % base);
			val += d_i * invBi;
			n /= base;
			invBi *= invBase;
		}
		return val;
	}


	// returns the minimum point generated.
	float
	TrianglesBRDF::createDistribution(){

		// clear variables, if any

		if(points != NULL)
			delete []points;
			
		points = new float[width*width];

		if(vertex != NULL)
			delete []vertex;

		int nVertex = width*width;
		vertex = new Ogre::Vector3[nVertex];

		nTriangles = (width-1)*(width-1)*2;
		
		float min = 1000000000000.;

		for(int i=0; i<nVertex; i++){
			switch(distribution){
		
				case 0: // flat
					points[i] = 0.;
					break;
				case 1: // random
					points[i] = Ogre::Math::UnitRandom();
					break;
				case 2: // uniform distribution
					points[i] = RadicalInverse(i,2);
					break;
				case 3: // gausian
					points[i] = Ogre::Math::gaussianDistribution(Ogre::Math::UnitRandom());
					break;
				case 4:
					cout << "not implemented yet!" << endl;
					break;
			}

			if(points[i] < min)
				min = points[i];
		}

		return min;
	}

int 
TrianglesBRDF::setMaterialInfo(Ogre::String materialName){
	
	
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);

	Ogre::TextureUnitState *vertexTexturePtr = mat->getTechnique(0)->getPass(0)->createTextureUnitState(vertexTexture);
	vertexTexturePtr->setTextureFiltering(Ogre::TFO_NONE);

	Ogre::TextureUnitState *indexTexturePtr = mat->getTechnique(0)->getPass(0)->createTextureUnitState(indexTexture);
	indexTexturePtr->setTextureFiltering(Ogre::TFO_NONE);

	Ogre::GpuProgramParametersSharedPtr gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParam->setNamedConstant("vertexWidth",(Ogre::Real)width);
//	cout << "vertexWidth: " << width << endl;
	gpuParam->setNamedConstant("indexWidth",(Ogre::Real)indexTexWidth);
//	cout << "indexWidth: " << indexTexWidth << endl;
	gpuParam->setNamedConstant("nTriangles",(Ogre::Real)nTriangles);
//	cout << "nTriangles: " << nTriangles << endl;
	gpuParam->setNamedConstant("maxHeight",(Ogre::Real)maxHeight);
	
//	if(materialName=="FirstTrianglesSimulation"){
		gpuParam->setNamedConstant("width",width-1);
//		cout << "width: " << width-1 << endl;
		gpuParam->setNamedConstant("height",width-1);
//		cout << "heightf:l " << width-1 << endl;
//	}

	gpuParam->setNamedConstant("vertexTexture",0);
	gpuParam->setNamedConstant("indexTexture",1);

	// number of textures added.
	return 2;
}



void 
TrianglesBRDF::normalDistribution(float mu, float sigma, float *A, float length)
  // mu is the mean (in our case it will be zero) of the normal
//distribution, and sigma^2 is the standard deviation.
  // array A  returns random numbers sampled from uniform distribution.
 //
{
   int count = 2;
   int n = (length+1)/2;
   for (int i = 0; i < n; i++){
//	   float u = Ogre::Math::Sqrt(-2.*Ogre::Math::Log(radicalInverse(count++)));
	   float u = Ogre::Math::Sqrt(-2.*Ogre::Math::Log(Ogre::Math::UnitRandom()));
		float v = 2.*Ogre::Math::PI*Ogre::Math::UnitRandom(); // in place of rand() use your uniformn random number generator
//		Ogre::Math::gaussianDistribution
		A[2*i] = mu + sigma*u*Ogre::Math::Cos(v);
        if ((2*i+1) < length)
			A[2*i+1] = mu + sigma*u*Ogre::Math::Sin(v);
   }
}


double 
TrianglesBRDF::radicalInverse(int i)
{
  const  int base = 2;
  double val = 0;
  double invBase = 1.0/base, invBi = invBase;
  int n = i;
  while (n > 0){
       int d_i = (n % base);
       val += d_i * invBi;
       n /= base;
       invBi *= invBase;
  }
  return val;
}



void
	TrianglesBRDF::createTrianglesFromDistribution(float min){

		if(triangleNormal != NULL){
			delete []triangleNormal;
			triangleNormal = NULL;
		}

		if(vertexNormal != NULL){
			delete []vertexNormal;
			vertexNormal = NULL;
		}

		if(index != NULL){
			delete []index;
			index = NULL;
		}

		maxHeight = 0.;

		// create vertices with i,j,height as a position
		int count = 0;
		for(float i=0; i<width; i++){
			for(float j=0; j<width; j++){

				float p = points[count]*scale;

				vertex[count] = Ogre::Vector3((float)j/(float)(width-1),(float)i/(float)(width-1),p);

				if(p > maxHeight)
					maxHeight = p;

				count++;
			}
		}

		// create the triangles from the vertices. Computing the indices.
		index = new int[nTriangles*3];
		triangleNormal = new Ogre::Vector3[nTriangles];

		count = 0;

		for(int i=0; i<width*width-width; i++){
			if(i%width < width-1){
				
//				if(i/width%2==0){
					if(i%2 == 0){
						// triangle 1
						index[count++] = i+width; 
						index[count++] = i; 
						index[count++] = i+1;

						// triangle 2
						index[count++] = i+width; 
						index[count++] = i+1;
						index[count++] = i+width+1;
				}else{

						// triangle 1
						index[count++] = i; 
						index[count++] = i+1; 
						index[count++] = i+width+1;

						// triangle 2
						index[count++] = i; 
						index[count++] = i+width+1;
						index[count++] = i+width;

					}
/*				}else{

					if(i%2 != 0){
						// triangle 1
						index[count++] = i+width; 
						index[count++] = i; 
						index[count++] = i+1;

						// triangle 2
						index[count++] = i+width; 
						index[count++] = i+1;
						index[count++] = i+width+1;
					}else{

						// triangle 1
						index[count++] = i; 
						index[count++] = i+1; 
						index[count++] = i+width+1;

						// triangle 2
						index[count++] = i; 
						index[count++] = i+width+1;
						index[count++] = i+width;

					}
			

				}	*/
			}
		}

		//compute triangle normals
		count = 0;
		for(int i=0; i<nTriangles; i++){

		//	cout << index[count+0] << "/ " << index[count+1] << "/ " << index[count+2] << endl;
			triangleNormal[i] = computeNormal(index[count+0],index[count+1],index[count+2]);
			count += 3;

		}


		//compute vertex normals
		vertexNormal = new Ogre::Vector3[width*width];

		for(int i=0; i<width*width; i++){ // for each vertex

			Ogre::Vector3 normal(0.,0.,0.);
			int vertexInTriangle = 0;

			count = 0;
			//look on to all triagles, if it is included, add its normal
			for(int j=0; j<nTriangles; j++){

				if((index[count+0] == i)||(index[count+1] == i)||(index[count+2] == i)){

					normal += triangleNormal[j];
					vertexInTriangle++;
				}
				count += 3;
			}

			if(vertexInTriangle == 0)
				cout << "Something is not going well here!!" << endl;

//			cout << "Vertex num: " << i << endl;
			normal /= vertexInTriangle;
			
			normal.normalise();
//			cout << "Vertex: " << vertex[i] << endl; 
//			cout << "Normal: " << normal << endl; 
			vertexNormal[i] = normal ;

		}


	}

Ogre::Vector3
TrianglesBRDF::computeNormal(int v0, int v1, int v2){

	Ogre::Vector3 vec1(vertex[v1] - vertex[v0]);
	vec1.normalise();
	
	Ogre::Vector3 vec2(vertex[v2] - vertex[v0]);
	vec2.normalise();

	Ogre::Vector3 perp = vec1.crossProduct(vec2);

	perp.normalise();

	return perp;
}

void
TrianglesBRDF::createTextures(){

	// vertex texture
	if(Ogre::TextureManager::getSingleton().resourceExists(vertexTexture)){
		Ogre::TextureManager::getSingleton().remove(vertexTexture);
	}

	float *vertexInfo = new float[3 * width * width];

	for(int i=0; i<width*width; i++){
		vertexInfo[i*3+0] = vertex[i][0];
		vertexInfo[i*3+1] = vertex[i][1];
		vertexInfo[i*3+2] = vertex[i][2];
	}

	Ogre::DataStreamPtr vertexStream = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(vertex,3 * width * width * sizeof(float)));
	
	Ogre::Image *vertexImage = new Ogre::Image();
	
	vertexImage->loadRawData(vertexStream,width,width,1,Ogre::PF_FLOAT32_RGB,1,0);

	Ogre::TextureManager::getSingleton().loadImage(vertexTexture,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		*vertexImage,
		Ogre::TEX_TYPE_2D,
		0,
		1.0f,
		true,
		Ogre::PF_FLOAT32_RGB);

	delete []vertexInfo;

	// index texture
	
	indexTexWidth = ceil(sqrt((float)nTriangles));
	
	float *indexInfo = new float[indexTexWidth*indexTexWidth*3];

	for(int i=0; i<indexTexWidth*indexTexWidth; i++){
			
		if(i<nTriangles){
			indexInfo[i*3+0] = index[i*3+0];
			indexInfo[i*3+1] = index[i*3+1];
			indexInfo[i*3+2] = index[i*3+2];
		}else{
			indexInfo[i*3+0] = -1;
			indexInfo[i*3+1] = -1;
			indexInfo[i*3+2] = -1;
		}
	}
	
	if(Ogre::TextureManager::getSingleton().resourceExists(indexTexture)){
		Ogre::TextureManager::getSingleton().remove(indexTexture);
	}

	Ogre::DataStreamPtr indexStream = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(indexInfo,3* indexTexWidth * indexTexWidth * sizeof(float)));
	
	Ogre::Image *indexImage = new Ogre::Image();
	
	indexImage->loadRawData(indexStream,indexTexWidth,indexTexWidth,1,Ogre::PF_FLOAT32_RGB,1,0);

	Ogre::TextureManager::getSingleton().loadImage(indexTexture,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		*indexImage,
		Ogre::TEX_TYPE_2D,
		0,
		1.0f,
		true,
		Ogre::PF_FLOAT32_RGB);

	delete []indexInfo;

}

void 
TrianglesBRDF::guiUpdated(){

	int widthNew;
	int distributionNew;
	float scaleNew;

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
				
					
					widthNew = dynamic_cast<QtVariantProperty*>(values.at(0))->value().toInt();
					distributionNew = dynamic_cast<QtVariantProperty*>(values.at(1))->value().toInt();
					scaleNew = dynamic_cast<QtVariantProperty*>(values.at(2))->value().toDouble();

				}//else active

			}
		}
	}
	
	if((scale != scaleNew)||(width != widthNew)||(distribution != distributionNew)){
		// update variables
		scale = scaleNew;
		width = widthNew;
		distribution = distributionNew;

		// refresh.
		updateMaterial();
	}

	emit brdfModified();

}

void 
TrianglesBRDF::updateMaterial(){

	float min = createDistribution();
	createTrianglesFromDistribution(min);

	switch(materialToUpdate)
	{
	case -2: 
		// activate when using relief maching approach for display surface material
		//		createTextures();
		emit updateSurfaceViewer(2);	
		break;
	case -1: // brdf viewer
		createTextures();
		Ogre::MaterialManager::getSingleton().getByName("Display2DTextureBRDF")->reload(); // TODO: really needed?
		emit updateBRDFViewer();
		break;
	case 0: // point light rendering

		break;
	case 1: // env. map rendering

		break;
	}


}

void TrianglesBRDF::recreateParameterGUI()
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
	subItems = variantManager->addProperty(QVariant::Int, "Grid resolution");
	subItems->setValue(width);
	subItems->setAttribute("singleStep", 1);
	subItems->setAttribute("minimum", 2);
	subItems->setAttribute("maximum", 1000);
	item->addSubProperty(subItems);

	subItems = variantManager->addProperty(QVariant::Int, "Distribution");
	subItems->setValue(distribution);
	item->addSubProperty(subItems);

	subItems = variantManager->addProperty(QVariant::Double, "Scale");
	subItems->setValue(scale);
	subItems->setAttribute("singleStep", 0.1);
	subItems->setAttribute("decimals", 3);
	subItems->setAttribute("minimum", 0.001);
	subItems->setAttribute("maximum", 100.);
	item->addSubProperty(subItems);

	variantEditor->addProperty(topItem);

	connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(guiUpdated()));

}