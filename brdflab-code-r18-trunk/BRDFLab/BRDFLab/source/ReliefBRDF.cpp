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

#include "ReliefBRDF.h"




int 
ReliefBRDF::setMaterialInfo(Ogre::String materialName){

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);

	Ogre::TextureUnitState *heightTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState(textureName.toStdString());
	heightTexture->setTextureFiltering(Ogre::TFO_BILINEAR);

	Ogre::GpuProgramParametersSharedPtr gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParam->setNamedConstant("map",0);
	
	if(materialName == "FirstHeightFieldSimulation")
		gpuParam->setNamedConstant("tilt",tilt);

	gpuParam->setNamedConstant("depth",(Ogre::Real)depth);

	return 1;

}


void 
ReliefBRDF::writeXML(QDomDocument *doc, QDomElement *parent){
	QDomElement material = doc->createElement("Material");

	QDomElement type = doc->createElement("Type");
	type.appendChild(doc->createTextNode(brdfType));

	QDomElement texName = doc->createElement("TextureName");
	texName.appendChild(doc->createTextNode(textureName));

	material.appendChild(type);
	material.appendChild(texName);

	parent->appendChild(material);
}


/*
void
ReliefBRDF::initViewer(){

	// create mipmapped texture

	// open viewer
	//renderWindow = new OgreSurfaceViewer();

}
*/

/*
void
ReliefBRDF::updateTexture(){

	float *prova = simulationWindow->readTexture();

	simulationWindow->processTexture(prova);

//	simulationWindow->mipmapping();

	delete []prova;
}*/



// guiupdated amb -> emit brdfModified();

void 
ReliefBRDF::guiUpdated(){

	int tiltNew;
	float depthNew;

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


					tiltNew = dynamic_cast<QtVariantProperty*>(values.at(0))->value().toInt();
					depthNew = dynamic_cast<QtVariantProperty*>(values.at(1))->value().toDouble();

				}//else active

			}
		}
	}

	if((depth != depthNew)||(tilt != tiltNew)){
		// update variables
		depth = depthNew;
		tilt = tiltNew;

		// refresh.
		updateMaterial();
	}

	emit brdfModified();

}

void 
ReliefBRDF::updateMaterial(){

	Ogre::MaterialPtr mat;
	Ogre::GpuProgramParametersSharedPtr gpuParam;
	switch(materialToUpdate)
	{
		case -2: 
			// activate when using relief maching approach for display surface material
			//		createTextures();
			
			mat = Ogre::MaterialManager::getSingleton().getByName("reliefMappingViewer");

			gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

			gpuParam->setNamedConstant("tilt",tilt);
			gpuParam->setNamedConstant("depth",(Ogre::Real)depth);

			break;
		case -1: // brdf viewer
			//createTextures();
			//Ogre::MaterialManager::getSingleton().getByName("Display2DTextureBRDF")->reload(); // TODO: really needed?
	//		emit updateBRDFViewer();
			break;
		case 0: // point light rendering

			break;
		case 1: // env. map rendering

			break;
	}


}

void ReliefBRDF::recreateParameterGUI()
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
	subItems = variantManager->addProperty(QVariant::Int, "Tilt");
	subItems->setValue(tilt);
	subItems->setAttribute("singleStep", 1);
	subItems->setAttribute("minimum", 1);
	subItems->setAttribute("maximum", 1000);
	item->addSubProperty(subItems);

	subItems = variantManager->addProperty(QVariant::Double, "Depth");
	subItems->setValue(depth);
	subItems->setAttribute("singleStep", 0.01);
	subItems->setAttribute("decimals", 3);
	subItems->setAttribute("minimum", 0.);
	subItems->setAttribute("maximum", 100.);
	item->addSubProperty(subItems);

	variantEditor->addProperty(topItem);

	connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(guiUpdated()));


}