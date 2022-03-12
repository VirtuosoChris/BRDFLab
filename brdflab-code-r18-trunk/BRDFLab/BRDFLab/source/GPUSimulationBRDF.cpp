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

#include "GPUSimulationBRDF.h"

using namespace std;

void
GPUSimulationBRDF::initComputationScene(){

	simulationWindow = new OgreSimulationWindow(this, firstMaterialName, iterateMaterialName, nIterations,simulationResolution);

	simulationWindow->paintEvent();
	simulationWindow->hide();
}

GPUSimulationBRDF::~GPUSimulationBRDF(){
		
	if(outgoingDirections != NULL)
		delete []outgoingDirections;
	
	delete simulationWindow;

}

void 
GPUSimulationBRDF::computeBRDF(float* &texInfo, int &x, int &y, float &min, float &max){

	minValue = min;
	maxValue = max;

	QTime *timer = new QTime();

	timer->start();
	cout << "--------------- GPU Simulation Started ------------------" << endl;

	if(outgoingDirections != NULL)
		delete []outgoingDirections;

	outgoingDirections = simulationWindow->compute(incidentVector);

	cout << "Computation time: " << (float)timer->elapsed()/1000. << endl;
	
	timer->restart();

	texInfo = simulationWindow->processTexture(outgoingDirections,  x, y, min, max);

	cout << "Processing time: " << (float)timer->elapsed()/1000. << endl;
	cout << "--------------------------------------------------------------" << endl;
	//simulationWindow->mipmapping();

}

void
GPUSimulationBRDF::changeSimulationTexturesResolution(int width, int height){
	
	if(width != simulationResolution){
		simulationWindow->createTextures(width,height);
		simulationResolution = width;
	}

	if(materialToUpdate == -1)
		displayBRDF(textureResolution[0]);

}

void
GPUSimulationBRDF::reprocessTexture(int x){

	int y = 0;

	float min = 0.;
	float max = 0.;

	float *texInfo = simulationWindow->processTexture(outgoingDirections,  x, y, min, max);

	setDisplayTexture(texInfo, x, y, minValue, maxValue);

	maxValue = max;
	minValue = min;

	textureResolution[0] = x;
	textureResolution[1] = y;

	delete []texInfo;



}

void GPUSimulationBRDF::setupGPUSimulationParameters( QtProperty *topItem )
{
	QtVariantProperty *item = variantManager->addProperty(QVariant::Char, "GPU Simulation Parameters");
	item->setValue("");
//	item->setEnabled(enabled);
	topItem->addSubProperty(item);

	QtVariantProperty *subItems = variantManager->addProperty(QVariant::Int, "nRays");
	subItems->setValue(simulationResolution*simulationResolution);
	subItems->setAttribute("singleStep",1);
	subItems->setAttribute("minimum", 10);
	subItems->setAttribute("maximum", 1000000);
	item->addSubProperty(subItems);
}

void GPUSimulationBRDF::updateGPUSimulationParameters( QList<QtProperty*> values )
{
	int simulationResolutionNew;

	for (int k=0; k<values.size(); ++k)
	{
		if(values.at(k)->propertyName() == "nRays")
			simulationResolutionNew = floor(sqrt((float)dynamic_cast<QtVariantProperty*>(values.at(k))->value().toInt()));
	}

	if(simulationResolutionNew != simulationResolution)
		changeSimulationTexturesResolution(simulationResolutionNew,simulationResolutionNew);

}