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

#include "AnalyticBRDF.h"
#include "imdebug.h"
using namespace std;

std::vector<ParametricLOBE*> AnalyticBRDF::avaliableLobes;

#define EPSILON 1.0e-5


AnalyticBRDF::AnalyticBRDF(int channels, LobeCombination lc, QString name):
	BRDF(channels,QString("Analytic")),
	materialName(name),
	combinationType(lc),
	nParameters(0),
	lb(NULL),
	ub(NULL),
	params(NULL),
	scaleFactors(NULL),
	fitted(false)
{
	if(avaliableLobes.size() < 1){
		// load XML of the models

		// look at all the files in the path: ../Data/analyticModels
		
		QDir directory("../Data/analyticModels/");
		
		QStringList filters;
		filters << "*.xml";

		QStringList models = directory.entryList(filters);

		QStringList::const_iterator constIterator;
		for (constIterator = models.constBegin(); constIterator != models.constEnd(); ++constIterator){
			QString fileName = directory.path();
			fileName.append("/");
			fileName.append(*constIterator);

			cout << fileName.toStdString() << endl;

			//set the lambertian at the first position.
			if(*constIterator == "Lambertian.xml"){
				avaliableLobes.insert(avaliableLobes.begin(),new ParametricLOBE(fileName));
			}else{
				avaliableLobes.push_back(new ParametricLOBE(fileName));
			}
		}
	}

	setCubeRoot(cubeRoot);
}


AnalyticBRDF::~AnalyticBRDF()
{
	
	clear();

}

void
AnalyticBRDF::resetMaterial(){

	// clean lobe visualization material

	Ogre::String materialNameToClean("DisplayAnalyticBRDF");

	if(fitted){ // if this brdf is for fitting..
		materialNameToClean = Ogre::String("DisplayFittedBRDF");
	}

	// material cleanup
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialNameToClean);

	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuParameters->setNamedConstant("fixed",(Ogre::Real)0.);
	gpuParameters->setNamedConstant("channels",(Ogre::Real)3.);
	gpuParameters->setNamedConstant("lobes",(Ogre::Real)1.);

	float *vsParams = new float[5];
	vsParams[0] = 0.;
	vsParams[1] = 0.;
	vsParams[2] = 0.;
	vsParams[3] = 0.;
	vsParams[4] = 1.;

	gpuParameters->setNamedConstant("params",vsParams,5,1);


	// clean point light material
	materialNameToClean = Ogre::String("PointLightAnalyticBRDF");

	mat = Ogre::MaterialManager::getSingleton().getByName(materialNameToClean);
	gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("fixed",(Ogre::Real)0.);
	gpuParameters->setNamedConstant("channels",(Ogre::Real)3.);
	gpuParameters->setNamedConstant("lobes",(Ogre::Real)1.);
	gpuParameters->setNamedConstant("params",vsParams,5,1);
	

	// clean env map material
	mat = Ogre::MaterialManager::getSingleton().getByName("EnvMapAnalyticBRDF");

	// get the fragment shader parameters
	gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("diffuse",Ogre::Vector3(0.,0.,0.));
	gpuParameters->setNamedConstant("lobes",(Ogre::Real)0.);

	delete []vsParams;

}

void
AnalyticBRDF::clear(){
	
	lobes.clear();

	if(params!=NULL){
		delete []params;
		params = NULL;
	}
	if(lb!=NULL){
		delete []lb;
		lb = NULL;
	}
	if(ub!=NULL){
		delete []ub;
		ub = NULL;
	}
	if (scaleFactors!=NULL){
		delete []scaleFactors;
		scaleFactors = NULL;
	}

	nParameters = 0;

	resetMaterial();
}

void 
AnalyticBRDF::setCubeRoot(bool cr){

	cubeRoot = cr;

	Ogre::MaterialPtr mat;
	
	if(fitted)
		mat = Ogre::MaterialManager::getSingleton().getByName("DisplayFittedBRDF");
	else
		mat = Ogre::MaterialManager::getSingleton().getByName("DisplayAnalyticBRDF");
		
	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	if(cubeRoot)
		gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)0.33);
	else
		gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)1.);

}

void
AnalyticBRDF::updateParameterVector(int nParametersOld){

	// update the parameters vector

	if(params != NULL){

		// copy the pointer to the data array
		double *pTemp = params; 

		// create and copy the old information to the new one
		params = new double[nParameters];

		for(int i=0; i<nParametersOld; i++)
			params[i] = pTemp[i];

		// delete the old array information
		if(pTemp != NULL){
			delete []pTemp;
			pTemp = NULL;
		}
	}else{
		if(nParameters > 0)
			params = new double[nParameters];
	}
}


/**

This method is used in order to add a Lobe to the Analytic BRDF definition. Depending on the CombinationType of the BRDF,
will also add the nChannels Scalar values corresponding to the lobe.

*/
void 
AnalyticBRDF::addLobe(QString name){

	// find the position in the avaliableLobes vector of the desired Lobe
	bool found = false;
	
	if(avaliableLobes.size() < 1)
		return;

	int pos = 0;
	while(!found){
		if(avaliableLobes[pos]->getLobeName() == name)
			found = true;
		else
			pos++;
	}

	// if the name does not exist, return
	if (!found)
		return;


	//if the object uses linear combination, update the scaling factors vector
	if(combinationType == Linear){

		// create the scaling factors
		int scaleFactorsSizeOld = nChannels * lobes.size();
		int scaleFactorsSize = nChannels * (lobes.size()+1);

		if(scaleFactors != NULL){
				
			double *sTemp = scaleFactors;
			
			scaleFactors = new double[scaleFactorsSize];

			for(int i=0; i < scaleFactorsSizeOld; i++)
				scaleFactors[i] = sTemp[i];

			delete []sTemp;

		}else{
			scaleFactors = new double[nChannels];
		}
	
		// set the new scaleFactors to 1.
		for(int i=scaleFactorsSizeOld; i < scaleFactorsSize; i++)
			scaleFactors[i] = 1.;

		// get the pointer to the lobe
		ParametricLOBE *lobe = avaliableLobes[pos];

		lobes.push_back(lobe);

		int nParametersOld = nParameters;
		nParameters += lobe->getNParameters(combinationType);
	
		updateParameterVector(nParametersOld);

		lobe->setDefaultParameters(params+nParametersOld, combinationType);

	}else{

		for(int i=0; i<nChannels; i++){

			ParametricLOBE *lobe = avaliableLobes[pos];

			lobes.push_back(lobe);

			int nParametersOld = nParameters;
			nParameters += lobe->getNParameters(combinationType);
		
			updateParameterVector(nParametersOld);

			lobe->setDefaultParameters(params+nParametersOld, combinationType);

		}

	}

	recreateParameterGUI(true);

	generateShaders();

	updateMaterial();

	brdfModified();
//	setMaterialLobeViewer();
//	setMaterialObjectRenderer();
}

void
AnalyticBRDF::addLobe(){

	QStringList items;
	
	for(unsigned int i=0; i<avaliableLobes.size(); i++ ){
		items << avaliableLobes[i]->getLobeName();
	}

	bool ok;
	QString item = QInputDialog::getItem(this, tr("BRDFLab: New Lobe"),
                                          tr("Model:"), items, 0, false, &ok);

	if (!ok)
		return;

	this->addLobe(item);


}

/**

This method is with linear combination to compute least squares.

*/

void 
AnalyticBRDF::evalBRDFLobes(double *p, Ogre::Vector3 u, Ogre::Vector3 v, double *vArray){
	if (!linearCombination()){
		cerr << "At this time evalBRDFLobes() is called only for Linear Combination." << endl; 
		exit(1);
	}

	int index = 0;

	int size = lobes.size(); // todo remove this.

	for (unsigned int i=0; i < lobes.size(); i++){
			vArray[i]=lobes[i]->evalLOBE(p+index,u,v,combinationType);
			index+=lobes[i]->getNParameters(combinationType);
	}
}


/**

This method evaluates the BRDF.

*/

void 
AnalyticBRDF::evalBRDF(double *p, Ogre::Vector3 u, Ogre::Vector3 v, double *value){
	for (int k=0; k< nChannels; k++) value[k] = 0.0;
	int index = 0;
	if (linearCombination()){
		for (unsigned int i=0; i < lobes.size(); i++){ 
			double lobeValue=lobes[i]->evalLOBE(p+index,u,v,combinationType);
			for (int k=0; k< nChannels; k++) //todo this is probably not correct
				value[k] += scaleFactors[i*nChannels+k]*lobeValue;
			index+=lobes[i]->getNParameters(combinationType);
		}
	}else{ // Fixed Combination
		for (unsigned int i=0; i < lobes.size()/nChannels; i++){ 
			for (int k=0; k< nChannels; k++){
				value[k] += lobes[i*nChannels+k]->evalLOBE(p+index,u,v,combinationType);
				index+=lobes[i*nChannels+k]->getNParameters(combinationType);
			}
		}
	}
/*
	for (int k=0; k< nChannels; k++){
		if(value[k] != value[k])
			value[k] = 100000.;	
	}*/
}

/**

This method prints the parameters 

*/

void 
AnalyticBRDF::showParameters(){
	int index = 0;
	if (linearCombination()){
		for (unsigned int i=0; i < lobes.size(); i++) {
			cout << "scaling factors: ";
			for (int k=0; k< nChannels; k++)
				cout << scaleFactors[i*nChannels+k] << " ";
			cout << endl;
			cout << "Lobe " << lobes[i]->getLobeName().toStdString() << " parameters:"<< endl;
			lobes[i]->showParameters(params+index, combinationType);
			index+=lobes[i]->getNParameters(combinationType);
		}
	}else{ // Fixed Combination
		for (unsigned int i=0; i < lobes.size(); i++){
			cout << "Lobe " << lobes[i]->getLobeName().toStdString() << " parameters:"<< endl;
		//	for (int k=0; k< nChannels; k++){
				lobes[i]->showParameters(params+index, combinationType);
				index+=lobes[i]->getNParameters(combinationType);
		//	}
		}
	}
	cout << endl;
}
void 
AnalyticBRDF::initForOptimize(){
	if (lb == NULL)
		lb = new double[nParameters];
	if (ub == NULL)
		ub = new double[nParameters];
	int index = 0;
	if (linearCombination()){
		for (unsigned int i=0; i < lobes.size(); i++){
			lobes[i]->initForOptimize(lb+index,ub+index,combinationType);
			index+=lobes[i]->getNParameters(combinationType); 			
		}
	}else{
		for (unsigned int i=0; i < lobes.size(); i++){ 
			//for (int k=0; k< nChannels; k++){
				lobes[i]->initForOptimize(lb+index,ub+index,combinationType);
				index+=lobes[i]->getNParameters(combinationType);
			//}
		}
	}
}

void 
AnalyticBRDF::writeXML(QDomDocument *doc, QDomElement *parent){
	writeMaterialXML(doc,parent);
}

void 
AnalyticBRDF::writeMaterialXML(QDomDocument *doc, QDomElement *parent){
	QDomElement material = doc->createElement("Material");

	QDomElement name = doc->createElement("Name");
	name.appendChild(doc->createTextNode(materialName));

	QDomElement type = doc->createElement("Type");
	type.appendChild(doc->createTextNode(brdfType));

	QDomElement combination = doc->createElement("LobeCombination");
	
	if(combinationType == Linear)
		combination.appendChild(doc->createTextNode("Linear"));
	else
		combination.appendChild(doc->createTextNode("Fixed"));



	material.appendChild(name);
	material.appendChild(type);
	material.appendChild(combination);

	QDomElement parameters = doc->createElement("Parameters");
	parameters.setAttribute("nChannels",nChannels);
	parameters.setAttribute("nLobes",lobes.size());

	int index = 0;
	if (linearCombination()){
		for (unsigned int i=0; i < lobes.size(); i++) {

			QDomElement scalingFactors = doc->createElement("ScalingFactors");
			for (int k=0; k< nChannels; k++){
				Ogre::String channelName("channel");
				channelName += Ogre::StringConverter::toString(k);
				scalingFactors.setAttribute(QString(channelName.c_str()),scaleFactors[i*nChannels+k]);

			}
			parameters.appendChild(scalingFactors);

			QDomElement lobe = doc->createElement("Lobe");

			lobes[i]->writeXML(doc,&lobe,params+index,combinationType); 
			index+=lobes[i]->getNParameters(combinationType);

			parameters.appendChild(lobe);
		}
	}else{
		for (unsigned int i=0; i < lobes.size(); i++){ 
//			for (int k=0; k< nChannels; k++){
				QDomElement lobe = doc->createElement("Lobe");
				lobe.setAttribute("channel",i%nChannels);

				lobes[i]->writeXML(doc,&lobe,params+index,combinationType);
				index+=lobes[i]->getNParameters(combinationType);

				parameters.appendChild(lobe);

//			}
		}
	}

	material.appendChild(parameters);

	parent->appendChild(material);
}


void 
AnalyticBRDF::updateMaterial(){
	
	if(active){

		switch(materialToUpdate){
			
			case -1: // lobe display
				setMaterialLobeViewer();
				break;
			case 0: // point light
				setLightParameters("PointLightAnalyticBRDF");
				break;
			case 1: // env. map. rendering
				setEnvMapParameters("EnvMapAnalyticBRDF");
				break;
		}
	}
}


void 
AnalyticBRDF::changeDisplayChannel(int x){

	displayChannel = x;

	updateMaterial();
}


void
AnalyticBRDF::setMaterialLobeViewer(){


	Ogre::String materialName("DisplayAnalyticBRDF");

	if(fitted){ // if this brdf is for fitting..
		materialName = Ogre::String("DisplayFittedBRDF");
	}


	// get the material
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);

	// get the vertex shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();


	setParametersLobeDisplayShader(gpuVertexParam);


	//emit updateImportanceSampleDirections();

}


void
AnalyticBRDF::setMaterialObjectRenderer(){

	setLightParameters("PointLightAnalyticBRDF");

	// filtered env map
	setEnvMapParameters("EnvMapAnalyticBRDF");

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

void
AnalyticBRDF::setEnvMapParameters(Ogre::String materialName){

		
	Ogre::Vector3 diffuse(0.,0.,0.);
	
	int vsParamsSize = 0;
	int nonLambertianLobes = 0;
	int pCount = 0;

	//search all the Lambert lobes.
	for(unsigned int i=0; i < lobes.size(); i++){
	
		if(lobes[i]->getLobeName() == "Lambertian"){

			if(combinationType == Linear){
				for(int j=0; j<nChannels; j++){
					diffuse[j] += scaleFactors[i*nChannels+j];
				}
			}else{
				
				diffuse[i%nChannels] += (float)params[pCount++];
			}
		}else{
			vsParamsSize += lobes[i]->getNParameters(combinationType);
			pCount += lobes[i]->getNParameters(combinationType);
			nonLambertianLobes++;
		}
	}

	// send the information of the specular lobes to the fragment shader.
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);
	
	// get the fragment shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("diffuse",diffuse);

	gpuParameters->setNamedConstant("lobes",(Ogre::Real)nonLambertianLobes);

	if(nonLambertianLobes > 0){

		// set the parameters
		if(combinationType == Fixed)
			gpuParameters->setNamedConstant("fixed",(Ogre::Real)1.);
		else
			gpuParameters->setNamedConstant("fixed",(Ogre::Real)0.);

		gpuParameters->setNamedConstant("channels",(Ogre::Real)nChannels);

		//cout << "nLobeS: " << lobes.size() << endl;

		if(combinationType == Linear) // add the scaling factor
			vsParamsSize += nonLambertianLobes*nChannels;

		float *vsParams = new float[vsParamsSize];

		//cout << "parametres a l'array del shader: " << vsParamsSize << endl;

		int index = 0;
		pCount = 0.;

		for(unsigned int i=0; i<lobes.size(); i++){

			if(lobes[i]->getLobeName() != "Lambertian"){
				// set the brdf identifier.
				//vsParams[index++] = getLobeIndex(lobes[i]->getLobeName());
				
				if(combinationType == Linear){ // add the scale factors..
					for(int j=0; j < nChannels; j++){
						vsParams[index++] = scaleFactors[i*nChannels+j];
					}
				}

				// add the parameters of the model.
				for(int j=0; j<lobes[i]->getNParameters(combinationType); j++)
					vsParams[index+j] = (float)params[pCount+j];

				index += lobes[i]->getNParameters(combinationType);
			}

			pCount += lobes[i]->getNParameters(combinationType);
		}

		gpuParameters->setNamedConstant("params",vsParams,vsParamsSize,1);

/*
		for (int i=0; i<vsParamsSize; i++)
		{
			cout << vsParams[i] << " ";
		}
 		cout << endl;
*/


		delete []vsParams; // to do, activate this
	

		int nSamples = 50;

		int nSamplesPerLobe = nSamples/nonLambertianLobes;

		gpuParameters->setNamedConstant("nSamplesPerLobe",nSamplesPerLobe);

		float *samples = new float[nSamplesPerLobe*nonLambertianLobes*2];

		//set uniform random numbers to the samples vector
		float minrnd=0.000001f;
		float maxrnd=1-minrnd;	

		// void genHaltonSequence(int n) {
		for (int i=0; i < nSamplesPerLobe*nonLambertianLobes; i++) {
			samples[i*2] = (float) RadicalInverse(i+1.,2);
			if(samples[i*2]<minrnd) samples[i]=minrnd;
			if(samples[i*2]>maxrnd) samples[i]=maxrnd;

			samples[i*2+1] = RadicalInverse(i+1.,3);
		}
/*
		// if this is commented, the importance sampling is computed on the GPU.

	
		pCount = 0;
		
		index = 0;

		for(unsigned int i=0; i<lobes.size(); i++){
			
			if(lobes[i]->getLobeName() != "Lambertian"){
				
				lobes[i]->computeNImportanceSamples(params+pCount,samples+index,nSamplesPerLobe);

				index += nSamplesPerLobe*2;
			}

			pCount += lobes[i]->getNParameters(combinationType);
		}
*/
		gpuParameters->setNamedConstant("samples",samples, nSamplesPerLobe*nonLambertianLobes, 2);

		delete []samples;

	}

}


void
AnalyticBRDF::setLightParameters(Ogre::String materialName){

	// get the material
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);

	// get the fragment shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	setParametersLobeDisplayShader(gpuParameters);

	gpuParameters->setNamedConstant("scale",Ogre::Vector3(1.,1.,1.)); // this is different from the display shader config.

}


void 
AnalyticBRDF::setParametersToGUI(){

	disconnect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), 0, 0);

	int nlobe = 0;
	int paramIndex = 0;

	// we already have the QtVariantPropertyManager
	QList<QtProperty*> p = variantEditor->properties();

	for (unsigned int i=0; i<(unsigned)p.size(); i++){

	//	cout << (*i)->propertyName().toStdString() << endl;
	//	cout << lobes[nlobe]->getLobeName().toStdString() << endl;

		if((unsigned)nlobe < lobes.size() && p[i]->propertyName() == lobes[nlobe]->getLobeName()){
			
//			cout << "lobe " << lobes[nlobe]->getLobeName().toStdString() << ":" << endl;

			// gather the childs of the property. (BRDF Models)
			QList<QtProperty *> childs = p[i]->subProperties();

			for (int j = 0; j < childs.size(); ++j) {
				
				if(linearCombination()){
							
					if(childs.at(j)->propertyName() == "Scale factors"){
						
						QList<QtProperty *> values = childs.at(j)->subProperties();

						for (int k=0; k<values.size(); ++k)
						{
							dynamic_cast<QtVariantProperty*>(values.at(k))->setValue(scaleFactors[nlobe*nChannels+k]);
//							cout << scaleFactors[nlobe*nChannels+k] << endl;
						}
				
					}else if(childs.at(j)->propertyName() == "Model parameters"){
						
						int nparam = lobes[nlobe]->getNParameters(combinationType);
						
						QList<QtProperty *> values = childs.at(j)->subProperties();

						if(nparam == values.size()){
							for (int k=0; k<values.size(); ++k)
							{
								dynamic_cast<QtVariantProperty*>(values.at(k))->setValue(params[paramIndex]);
//								cout << params[paramIndex] << endl;
								paramIndex++;

							}
						}else{
							qDebug("Error: The number of lobe parameters does not match the GUI.");
						}
					}//else active

				}else{
	
					QString n("Channel ");
					n.append(Ogre::StringConverter::toString((int)nlobe%nChannels).c_str());
					n.append(" model parameters");
					
//					cout << n.toStdString() << endl;
//					cout << childs.at(j)->propertyName().toStdString() << endl;

					if(childs.at(j)->propertyName() == n){

						int nparam = lobes[nlobe]->getNParameters(combinationType);

						QList<QtProperty *> values = childs.at(j)->subProperties();

						if(nparam == values.size()){
							for (int k=0; k<values.size(); ++k)
							{
								dynamic_cast<QtVariantProperty*>(values.at(k))->setValue(params[paramIndex++]);
							}
						}

						nlobe++;
					}

					
				}

			}
			if (linearCombination())
				nlobe++;
		}
	}

	
	
	/*
	int index = 0;

	for(unsigned int i=0; i<lobes.size(); i++){ // for each lobe on the GUI
		paramLobes[i]->setParameters(params+index);
		index += lobes[i]->getNParameters(combinationType);
	}


	if(combinationType == Linear){ //take in care the scaling factors vector

		index = 0;

		for(unsigned int i=0; i<paramScaleFactors.size(); i++){ // for each lobe on the GUI
			paramScaleFactors[i]->setParameters(scaleFactors+index);
			index += nChannels;
		}	
	}

	propertyEditor->expandAll();
*/
//	showParameters();

	connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(guiUpdated()));
	
	updateMaterial();

	emit brdfModified();
}


void 
AnalyticBRDF::loadXML(QDomElement param){


	if(combinationType == Linear){

		int nScalingFactors = nChannels*param.attribute("nLobes").toInt();
		double *tempScalingFactors = new double[nScalingFactors];
		int countScalingFactors = 0;
		int index = 0;


		QDomNode n = param.firstChild();
		while(!n.isNull()) {  // BRDF file
			QDomElement e = n.toElement();
			
			if(!e.isNull()) {

				if(e.tagName() == "ScalingFactors"){
				
					for(int i=0; i<nChannels; i++){
						Ogre::String channelName("channel");
						channelName += Ogre::StringConverter::toString(i);
						
						tempScalingFactors[countScalingFactors++] = e.attribute(QString(channelName.c_str())).toFloat();
					}

				}

				if(e.tagName() == "Lobe"){

					QDomNode i = e.firstChild();

					while(!i.isNull()){

						QDomElement ie = i.toElement();

						if(ie.tagName() == "Model")
							addLobe(ie.text());

						if(ie.tagName() == "Parameters"){
							lobes[lobes.size()-1]->loadXML(ie,params+index,combinationType);
							index += lobes[lobes.size()-1]->getNParameters(combinationType);
						}
						
						i = i.nextSibling();
					}


				}

//				cout << e.tagName().toStdString() << endl;

			}
			n = n.nextSibling();
		}

		for(int i=0; i<nScalingFactors; i++){
			scaleFactors[i] = tempScalingFactors[i];
		}
		
		delete []tempScalingFactors;
	}else{ // fixed

		int index = 0;
		int nLobe = 0;


		QDomNode n = param.firstChild();
		while(!n.isNull()) {  // BRDF file
			QDomElement e = n.toElement();
			
			if(!e.isNull()) {


				if(e.tagName() == "Lobe"){

					QDomNode i = e.firstChild();

					while(!i.isNull()){

						QDomElement ie = i.toElement();

						if(ie.tagName() == "Model")
							if(nLobe % nChannels == 0.)
								addLobe(ie.text());

						if(ie.tagName() == "Parameters"){
							lobes[lobes.size()-(nChannels - nLobe % nChannels)]->loadXML(ie,params+index,combinationType);
							index += lobes[lobes.size()-(nChannels - nLobe % nChannels)]->getNParameters(combinationType);
						}
						

						i = i.nextSibling();
					}

					nLobe++;
				}

//				cout << e.tagName().toStdString() << endl;

			}
			n = n.nextSibling();
		}
	}

//	showParameters();

	setParametersToGUI();

}

int 
AnalyticBRDF::getLobeIndex(QString name){
/*
	// set the brdf identifier.
	if(name == "Lambertian")
		return 0;
	else if(name == "BlinnPhong")
		return 1;
	else if(name == "LafortuneIso")
		return 2;
	else if(name == "LafortuneAniso")
		return 3;
	else if(name == "AshikhminShirley")
		return 4;
	else if(name == "Ward")
		return 5;
	else if(name == "Li_sparse")
		return 6;
	else if(name == "Ross_thick")
		return 7;
	else if(name == "CookTorrance")
		return 8;
	else if(name == "WardDuer")
		return 9;
	else 
		return -1;
*/
	return 0;
}

void
AnalyticBRDF::generateSampleDirections(Ogre::Vector3 u, std::vector<Ogre::Vector3> *directions){
/*
	int nonLambertianLobes = 0;

	//search all the Lambert lobes.
	for(unsigned int i=0; i < lobes.size(); i++){
		if(lobes[i]->getLobeName() != "Lambertian")
			nonLambertianLobes++;
	}
	
	if(nonLambertianLobes > 0){

		int nSamples = 100;

		int nSamplesPerLobe = nSamples/nonLambertianLobes;

		float *samples = new float[nSamplesPerLobe*nonLambertianLobes*2];

		//set uniform random numbers to the samples vector
		float minrnd=0.000001;
		float maxrnd=1-minrnd;	
		
		
		for(int k=0; k<nonLambertianLobes; k++){
			float startValue = 1., deltaValue = 1./nSamplesPerLobe;

			for (int i=0; i < nSamplesPerLobe; i++) {
				samples[i*2] = (float) RadicalInverse(i,2);
				if(samples[i*2]<minrnd) samples[i]=minrnd;
				if(samples[i*2]>maxrnd) samples[i]=maxrnd;

				samples[i*2+1] = startValue-i*deltaValue;

			}
		}

		int pCount = 0;

		int index = 0;

		for(unsigned int i=0; i<lobes.size(); i++){

			if(lobes[i]->getLobeName() != "Lambertian"){

				lobes[i]->computeNImportanceSamples(params+pCount,samples+index,nSamplesPerLobe);

				lobes[i]->computeImportanceSamplingOutgoingVectors(params+pCount,samples+index,nSamplesPerLobe, directions, u);

				index += nSamplesPerLobe*2;
			}

			pCount += lobes[i]->getNParameters(combinationType);
		}
		
		delete []samples;
	}

	*/
}

void 
AnalyticBRDF::unlinkGUI(){
	disconnect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(guiUpdated()));
}

void 
AnalyticBRDF::linkGUI(){
	connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(guiUpdated()));
}

void AnalyticBRDF::recreateParameterGUI( bool enabled /*= true*/ )
{

	disconnect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), 0,0);

	// we already have the QtVariantPropertyManager
	variantManager->clear();
	variantEditor->clear();

	// create each lobe on the GUI.

	int index = 0;
	if (linearCombination()){
		for (unsigned int i=0; i < lobes.size(); i++) {

			QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
				lobes[i]->getLobeName());

			QtVariantProperty *item;

/*
			item = variantManager->addProperty(QVariant::Bool, "Active");
			item->setValue(true);
			topItem->addSubProperty(item);
*/

			item = variantManager->addProperty(QVariant::Char, "Scale factors");
			item->setValue("");
			item->setEnabled(enabled);
			topItem->addSubProperty(item);

			for(unsigned int j=0; j<(unsigned)nChannels; j++){
				QString n("channel ");
				n.append(Ogre::StringConverter::toString(j).c_str());
				QtVariantProperty *subItems = variantManager->addProperty(QVariant::Double, n);
				subItems->setValue(scaleFactors[i*nChannels+j]);
				subItems->setAttribute("singleStep", 0.1);
				subItems->setAttribute("decimals", 5);
				subItems->setAttribute("minimum", 0.);
				subItems->setAttribute("maximum", 100.);
				item->addSubProperty(subItems);
			}

			item = variantManager->addProperty(QVariant::Char, "Model parameters");
			item->setValue("");
			item->setEnabled(enabled);
			topItem->addSubProperty(item);

			for(unsigned int j=0; j<(unsigned)lobes[i]->getNParameters(combinationType); j++){

				LobeParameter parameter = lobes[i]->getLobeParameter(j);
				
				QtVariantProperty *subItems = variantManager->addProperty(QVariant::Double, parameter.getName());
				subItems->setValue(params[index++]);
				subItems->setAttribute("singleStep", 0.1);
				subItems->setAttribute("decimals", 5);
				subItems->setAttribute("minimum", parameter.getLowBound());
				subItems->setAttribute("maximum", parameter.getHighBound());
				item->addSubProperty(subItems);
			}
			variantEditor->addProperty(topItem);
		}
	}else{ // Fixed Combination
		for (unsigned int i=0; i < lobes.size()/nChannels; i++) {

			QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
				lobes[i*nChannels]->getLobeName());

			QtVariantProperty *item;

/*
			item = variantManager->addProperty(QVariant::Bool, "Active");
			item->setValue(true);
			topItem->addSubProperty(item);
*/

			for (int j=0; j<nChannels; j++){

				QString n("Channel ");
				n.append(Ogre::StringConverter::toString(j).c_str());
				n.append(" model parameters");

				item = variantManager->addProperty(QVariant::Char, n);
				item->setValue("");
				item->setEnabled(enabled);
				topItem->addSubProperty(item);

				for(unsigned int k=0; k<(unsigned)lobes[i*nChannels+j]->getNParameters(combinationType); k++){

					LobeParameter parameter = lobes[i*nChannels+j]->getLobeParameter(k);

					QtVariantProperty *subItems = variantManager->addProperty(QVariant::Double, parameter.getName());
					subItems->setValue(params[index++]);
					subItems->setAttribute("singleStep", 0.1);
					subItems->setAttribute("decimals", 5);
					subItems->setAttribute("minimum", parameter.getLowBound());
					subItems->setAttribute("maximum", parameter.getHighBound());
					item->addSubProperty(subItems);
				}
			}
			variantEditor->addProperty(topItem);
		}
	}

	connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(guiUpdated()));

//	showParameters();

	updateMaterial();

	emit brdfModified();

}


void 
AnalyticBRDF::guiUpdated()
{

//	showParameters();
	
	// update parameters + scalefactors (if needed)
	int nlobe = 0;
	int paramIndex = 0;

	// we already have the QtVariantPropertyManager
	QList<QtProperty*> p = variantEditor->properties();

	for (unsigned int i=0; i<(unsigned)p.size(); i++){


		//	cout << p[i]->propertyName().toStdString() << endl;
		//	cout << lobes[nlobe]->getLobeName().toStdString() << endl;

		if((unsigned)nlobe < lobes.size() && p[i]->propertyName() == lobes[nlobe]->getLobeName()){

			//			cout << "lobe " << lobes[nlobe]->getLobeName().toStdString() << ":" << endl;

			// gather the childs of the property. (BRDF Models)
			QList<QtProperty *> childs = p[i]->subProperties();

			for (int j = 0; j < childs.size(); ++j) {

				if(linearCombination()){

					if(childs.at(j)->propertyName() == "Scale factors"){

						QList<QtProperty *> values = childs.at(j)->subProperties();

						for (int k=0; k<values.size(); ++k)
						{
							scaleFactors[nlobe*nChannels+k] = dynamic_cast<QtVariantProperty*>(values.at(k))->value().toDouble();
							//							cout << scaleFactors[nlobe*nChannels+k] << endl;
						}

					}else if(childs.at(j)->propertyName() == "Model parameters"){

						int nparam = lobes[nlobe]->getNParameters(combinationType);

						QList<QtProperty *> values = childs.at(j)->subProperties();

						if(nparam == values.size()){
							for (int k=0; k<values.size(); ++k)
							{
								params[paramIndex] = dynamic_cast<QtVariantProperty*>(values.at(k))->value().toDouble();
								//								cout << params[paramIndex] << endl;
								paramIndex++;

							}
						}else{
							qDebug("Error: The number of lobe parameters does not match the GUI.");
						}
					}//else active

				}else{

					QString n("Channel ");
					n.append(Ogre::StringConverter::toString((int)nlobe%nChannels).c_str());
					n.append(" model parameters");

					//					cout << n.toStdString() << endl;
					//					cout << childs.at(j)->propertyName().toStdString() << endl;

					if(childs.at(j)->propertyName() == n){

						int nparam = lobes[nlobe]->getNParameters(combinationType);

						QList<QtProperty *> values = childs.at(j)->subProperties();

						if(nparam == values.size()){
							for (int k=0; k<values.size(); ++k)
							{
								params[paramIndex++] = dynamic_cast<QtVariantProperty*>(values.at(k))->value().toDouble();
							}
						}

						nlobe++;
					}


				}

			}
			if (linearCombination())
				nlobe++;
		}
	}

//	showParameters();



	updateMaterial();

	emit brdfModified();
		//	this->showParameters();


}

void AnalyticBRDF::setGUIManagers( QtVariantPropertyManager *vM,QtTreePropertyBrowser *vE )
{
	variantManager = vM;
	variantEditor = vE;

//	connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
	connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), this, SLOT(guiUpdated()));
}



void
AnalyticBRDF::addFileContent(QTextStream *out, QString path){

	QFile file(path);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QTextStream stream(&file);

	while (!stream.atEnd()) {
		(*out) << stream.readLine() << "\n";
	}

	file.close();
}


void
AnalyticBRDF::generateDisplayLobeShader(bool returnBlack){

	// create file
	QFile file("../Data/shaders/DisplayAnalyticBRDF_VS.glsl");

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);

	// add header
	addFileContent(&out,"../Data/shaders/parts/headerDisplay.glsl");

	//// add coordinate transformation functions (not needed as we attach the file on the material script)
	//addFileContent(&out,"../Data/shaders/coordinateTransformations.glsl");

	if(returnBlack){
		out << "void main()\n";
		out << "{\n";
		out << "\tgl_Position = vec4(0.,0.,0.,0.);\n";
		out << "}\n";
	}else{
		writeGLSLEquations(&out);
		writeComputeBRDFFunction(&out);

		// add footer
		addFileContent(&out,"../Data/shaders/parts/footerDisplay.glsl");
	}

	// close file
	file.close();

	// compile shader
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("DisplayAnalyticBRDF");

	mat->getTechnique(0)->getPass(0)->getVertexProgram()->reload();
	mat->getTechnique(0)->getPass(0)->getFragmentProgram()->reload();
	//mat->_notifyNeedsRecompile();
	mat->reload();
}

void
AnalyticBRDF::generatePointLightShader(bool returnBlack){
	// create file
	QFile file("../Data/shaders/PointLightAnalyticBRDF_FS.glsl");

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);

	// add header
	addFileContent(&out,"../Data/shaders/parts/headerPointLight.glsl");

	if(returnBlack){
		out << "void main()\n";
		out << "{\n";
		out << "gl_FragColor = vec4(0.,0.,0.,0.);\n";
		out << "}\n";
	}else{
		writeGLSLEquations(&out);
		writeComputeBRDFFunction(&out);

		// add footer
		addFileContent(&out,"../Data/shaders/parts/footerPointLight.glsl");

	}

	// close file
	file.close();

	// compile shader
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("PointLightAnalyticBRDF");

	mat->getTechnique(0)->getPass(0)->getVertexProgram()->reload();
	mat->getTechnique(0)->getPass(0)->getFragmentProgram()->reload();
	//mat->_notifyNeedsRecompile();
	mat->reload();

}

void AnalyticBRDF::generateShaders()
{

	// display BRDF Lobe
	generateDisplayLobeShader();	

	// point light shader
	generatePointLightShader();

	// env map shader
	generateEnvMapShader();

}


void
AnalyticBRDF::setShadersToDefault(){

	// display BRDF Lobe
	generateDisplayLobeShader(true);	

	// point light shader
	generatePointLightShader(true);

	// env map shader
	generateEnvMapShader(true);


}

void AnalyticBRDF::setParametersLobeDisplayShader( Ogre::GpuProgramParametersSharedPtr gpuParam )
{

	// set the parameters
	if(combinationType == Fixed)
		gpuParam->setNamedConstant("fixed",(Ogre::Real)1.);
	else
		gpuParam->setNamedConstant("fixed",(Ogre::Real)0.);

	gpuParam->setNamedConstant("channels",(Ogre::Real)nChannels);

	gpuParam->setNamedConstant("lobes",(Ogre::Real)lobes.size());

	// the scaling factor + lobe parameters
	int vsParamsSize = 0;

	if(combinationType == Linear) // add the scaling factor
		vsParamsSize += lobes.size()*nChannels; 

	for(unsigned int i=0; i < lobes.size(); i++){
		vsParamsSize += lobes[i]->getNParameters(combinationType); // PROBLEM ON LINEAR IMPLEMENTATION!!!!! -> hardcoded...
	}

	float *vsParams = new float[vsParamsSize];

	int index = 0;
	int pCount = 0;

	for(unsigned int i=0; i<lobes.size(); i++){

		//vsParams[index++] = getLobeIndex(lobes[i]->getLobeName());

		if(combinationType == Linear){ // add the scale factors..
			for(int j=0; j < nChannels; j++){
				vsParams[index++] = scaleFactors[i*nChannels+j];
			}
		}

		// add the parameters of the model.
		for(int j=0; j<lobes[i]->getNParameters(combinationType); j++)
			vsParams[index+j] = (float)params[pCount+j];

		pCount += lobes[i]->getNParameters(combinationType);
		index += lobes[i]->getNParameters(combinationType);
	}

	gpuParam->setNamedConstant("params",vsParams,vsParamsSize,1);
/*
	for (int i=0; i<vsParamsSize; i++)
	{
		cout << vsParams[i] << " ";
	}
	cout << endl;
*/
	delete []vsParams;

}

void AnalyticBRDF::writeGLSLEquations( QTextStream* out, bool writeLambertian, bool writeOtherEquations)
{
	// first add common BRDF functions (Fresnel, etc.)
	addFileContent(out,"../Data/shaders/parts/BRDFfunctions.glsl");

	// add lobe functions
	// make sure that we do not repeat the function name on the shader.

	std::vector<QString> writedEquations;
	for(unsigned int i=0; i<lobes.size(); i++){

		if(lobes[i]->getLobeName() != "Lambertian" || writeLambertian){
			std::vector<QString>::iterator result = std::find(writedEquations.begin(), writedEquations.end(), lobes[i]->getLobeName());

			if(result == writedEquations.end()){
				(*out) << lobes[i]->getEquationGLSL(combinationType) << "\n";
				if(writeOtherEquations){
					(*out) << lobes[i]->getPdfGLSL() << "\n";
					(*out) << lobes[i]->getImportanceSamplingGLSL() << "\n";
					if(lobes[i]->getMirrorDirectionGLSL() != "")
						(*out) << lobes[i]->getMirrorDirectionGLSL() << "\n";
				}
				writedEquations.push_back(lobes[i]->getLobeName());
			}
		}
	}

	writedEquations.clear();
}

void
AnalyticBRDF::writeComputeBRDFFunction( QTextStream* out)
{
	// write computeBRDF function
	addFileContent(out,"../Data/shaders/parts/headerComputeBRDF.glsl");

	// manage scaling factors.
	for (unsigned int i=0; i<lobes.size(); i++)
	{
		//scaling factors...

		if(!linearCombination()){
			// fixed
			if (nChannels == 1)
			{
				(*out) << "\t" << "sfactor = vec3(1.,1.,1.);" << "\n";
			}else{
				switch(i%nChannels){
					case 0: // red
						(*out) << "\t" << "sfactor = vec3(1.,0.,0.);" << "\n";
						break;
					case 1: // green
						(*out) << "\t" << "sfactor = vec3(0.,1.,0.);" << "\n";
						break;
					case 2: // blue
						(*out) << "\t" << "sfactor = vec3(0.,0.,1.);" << "\n";
						break;
				}
			}
		}else{
			if (nChannels == 1)
			{
				(*out) << "\t" << "sfactor.x = params[j];" << "\n";
				(*out) << "\t" << "sfactor.y = params[j];" << "\n";
				(*out) << "\t" << "sfactor.z = params[j];" << "\n";
				(*out) << "\t" << "j++;" << "\n";
			}else{
				(*out) << "\t" << "sfactor.r = params[j++];" << "\n";
				(*out) << "\t" << "sfactor.g = params[j++];" << "\n";
				(*out) << "\t" << "sfactor.b = params[j++];" << "\n";
			}
		}

		// brdf evaluation

		// brdf function name
		QString equationName(lobes[i]->getLobeName());
		equationName.append("BRDF");

		// Evaluate lobe		
		(*out) << "\t" << "value = " << equationName << "(u, v, j);" << "\n";

		// compute brdf*sfactor*scale
		(*out) << "\t" << "color += sfactor * value * scale;" << "\n";

		(*out) << "\t" << "j += " << lobes[i]->getNParameters(combinationType) << ";" << "\n\n";

	}

	addFileContent(out,"../Data/shaders/parts/footerComputeBRDF.glsl");

	(*out) << "\n\n";
}

void AnalyticBRDF::generateEnvMapShader(bool returnBlack)
{
	// create file
	QFile file("../Data/shaders/EnvMapAnalyticBRDF_FS.glsl");

	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);

	// add header
	addFileContent(&out,"../Data/shaders/parts/headerEnvMap.glsl");


	if(returnBlack){
		out << "void main()\n";
		out << "{\n";
		out << "gl_FragColor = vec4(0.,0.,0.,0.);\n";
		out << "}\n";
	}else{
		writeGLSLEquations(&out,false,true);

		addFileContent(&out,"../Data/shaders/parts/bodyEnvMap.glsl");

		// add all the stuff...
		writeEnvMapLobes(&out);

		// add footer
		addFileContent(&out,"../Data/shaders/parts/footerEnvMap.glsl");
	}
	
	// close file
	file.close();

	// compile shader
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("EnvMapAnalyticBRDF");

//	mat->getTechnique(0)->getPass(0)->getFragmentProgram()->setSourceFile("EnvMapAnalyticBRDF_FS2.glsl");
	mat->getTechnique(0)->getPass(0)->getVertexProgram()->reload();
	mat->getTechnique(0)->getPass(0)->getFragmentProgram()->reload();
//	mat->_notifyNeedsRecompile();
	mat->reload();

}

void AnalyticBRDF::writeEnvMapLobes( QTextStream *out )
{
	int nLobe = 0;

	// manage scaling factors.
	for (unsigned int i=0; i<lobes.size(); i++)
	{
		if (lobes[i]->getLobeName() != "Lambertian")
		{
			//scaling factors...

			if(!linearCombination()){
				// fixed
				if (nChannels == 1)
				{
					(*out) << "\t" << "sfactor = vec3(1.,1.,1.);" << "\n";
				}else{
					switch(i%nChannels){
					case 0: // red
						(*out) << "\t" << "sfactor = vec3(1.,0.,0.);" << "\n";
						break;
					case 1: // green
						(*out) << "\t" << "sfactor = vec3(0.,1.,0.);" << "\n";
						break;
					case 2: // blue
						(*out) << "\t" << "sfactor = vec3(0.,0.,1.);" << "\n";
						break;
					}
				}
			}else{
				if (nChannels == 1)
				{
					(*out) << "\t" << "sfactor.x = params[count];" << "\n";
					(*out) << "\t" << "sfactor.y = params[count];" << "\n";
					(*out) << "\t" << "sfactor.z = params[count];" << "\n";
					(*out) << "\t" << "count++;" << "\n";
				}else{
					(*out) << "\t" << "sfactor.r = params[count++];" << "\n";
					(*out) << "\t" << "sfactor.g = params[count++];" << "\n";
					(*out) << "\t" << "sfactor.b = params[count++];" << "\n";
				}
			}

			(*out) << "\n";

			// brdf evaluation
			if (lobes[i]->getSpecularLobeDirection() == "Mirror")//other is .. HalfVector
			{
				// get the matrix to transform from reflection space to tangent space for the Lafortune isotropic lobe
//				(*out) << "\t" << " dir = normalize(eyeVectorTangent * vec3(params[count+0], params[count+0], params[count+1])); \n";
				QString functionName(lobes[i]->getLobeName());
				functionName.append("ComputeReflectionDirection(eyeVectorTangent, count);\n");
				
				(*out) << "\t" << " dir = " << functionName;
				(*out) << "\t" << " specularToTangent = constructCoordFrame(dir); \n";
			}


			(*out) << "\t" << " for(int j=0; j<nSamplesPerLobe; j++){ \n";

			(*out) << "\t\t" << "float brdf = 0.;\n";
			(*out) << "\t\t" << "float pdf = 0.;\n";
			(*out) << "\t\t" << "vec3 u = vec3(0.,0.,0.); \n";

			(*out) << "\t\t" << "// accessing the correct sample: [nLobe*nSamplesPerLobe+j]\n";

			QString importanceSamplingFuncName(lobes[i]->getLobeName());
			importanceSamplingFuncName.append("ImportanceSampling");

			//(*out) << "\t\t" << "vec3 s = spherical2cart(" << importanceSamplingFuncName << "(samples[" << nLobe << "*nSamplesPerLobe+j].xy,count));\n";
			(*out) << "\t\t" << "vec3 s = spherical2cart(" << importanceSamplingFuncName << "(samples[" << nLobe << "*nSamplesPerLobe+j].xy,count));\n";

			//importance sampling ...

			// pdf function name
			QString pdfName(lobes[i]->getLobeName());
			pdfName.append("PDF");

			// compute BRDF.
			if (lobes[i]->getSpecularLobeDirection() == "Mirror")
			{
				(*out) << "\t\t" << "u = s;\n";
				
				// pdf equation
				(*out) << "\t\t" << "pdf = " << pdfName << "(u, eyeVectorTangent, count);\n";	
	
				(*out) << "\t\t" << "// transform from specular space to tangent space\n";
				(*out) << "\t\t" << "u = specularToTangent*u;\n";
			}else{
				(*out) << "\t\t" << "vec3 h = s;\n";

				(*out) << "\t\t" << "if(dot(eyeVectorTangent,h) < 0.)\n";
				(*out) << "\t\t\t" << "h = -h;\n";

				(*out) << "\t\t" << "// compute the incident direction by reflecting about h\n";
				(*out) << "\t\t" << "u = -eyeVectorTangent +2. * dot(eyeVectorTangent, h) * h;\n";

				// get parameter used
				QChar paramName(getPDFVectorParameter(lobes[i]->getPdfGLSL()));

				(*out) << "\t\t" << "pdf = " << pdfName << "(" << paramName << ", eyeVectorTangent, count);\n";		

			}

			//brdf function name
			QString brdfName(lobes[i]->getLobeName());
			brdfName.append("BRDF");

			// brdf equation
			(*out) << "\t\t" << "brdf = " << brdfName << "(u, eyeVectorTangent, count);\n";

			addFileContent(out,"../Data/shaders/parts/bodyEnvMap2.glsl");
			(*out) << "\t" << "count += " << lobes[i]->getNParameters(combinationType) << ";\n";

			nLobe++;
		}
	}
}

QChar AnalyticBRDF::getPDFVectorParameter( QString pdfGLSL )
{

	QString searchFor("(vec3 ");
	int pos = pdfGLSL.indexOf(searchFor);

	return QChar(pdfGLSL[pos+6]);

	
}