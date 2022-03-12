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

#include "BRDFViewer.h"
#include <QGridLayout>

using namespace std;

BRDFViewer::BRDFViewer(QWidget *parent, BRDFLab *main):
 QWidget( parent ),
 brdfLab(main),
 sliderTexSize(NULL),
 displayPoints(false)
 
 {
	
	window = new OgreWidget(this);
	
	// for the channel selection option

	BRDF* mainBRDF = brdfLab->getMainBRDF();

	displayGroupBox = new QGroupBox(tr("Display"),this);
	displayGroupBox->setMaximumHeight(75);//100
	displayGroupBox->setMaximumWidth(200);

	QGridLayout *displayGridLayout = new QGridLayout();


	QLabel* lobeLabel = new QLabel("Lobe:",displayGroupBox);
	QLabel* channelLabel = new QLabel("Channel:",displayGroupBox);
	QLabel* modeLabel = new QLabel("Mode:",displayGroupBox);

	modeLabel->hide();

	lobeComboBox = new QComboBox(displayGroupBox);

	lobeComboBox->addItem("Both");
	lobeComboBox->addItem("Original");
	lobeComboBox->addItem("Fitted");

	channelComboBox = new QComboBox(displayGroupBox);
	QComboBox* modeComboBox =  new QComboBox(displayGroupBox);

	modeComboBox->addItem("Default");
	modeComboBox->addItem("Halfway vector");
	modeComboBox->addItem("3D Plot");

	modeComboBox->hide();

	displayGridLayout->addWidget(modeLabel,0,0,1,1);
	displayGridLayout->addWidget(lobeLabel,1,0,1,1);
	displayGridLayout->addWidget(channelLabel,2,0,1,1);

	displayGridLayout->addWidget(modeComboBox,0,1,1,1);
	displayGridLayout->addWidget(lobeComboBox,1,1,1,1);
	displayGridLayout->addWidget(channelComboBox,2,1,1,1);


	displayGroupBox->setLayout(displayGridLayout);	


	QGroupBox* optionsGroupBox = new QGroupBox(tr("Options"),this);
	optionsGroupBox->setMaximumHeight(75);//100
	optionsGroupBox->setMaximumWidth(200);

	QGridLayout *optionsGridLayout = new QGridLayout();


	QLabel* transformLabel = new QLabel("Transform:");

	transformComboBox = new QComboBox(optionsGroupBox);

	transformComboBox->addItem("None");
	transformComboBox->addItem("Cube Root");
	transformComboBox->addItem("Log");

	optionsGridLayout->addWidget(transformLabel,0,0,1,1);
	optionsGridLayout->addWidget(transformComboBox,0,1,1,1);


	QVBoxLayout  *optionsVBoxLayout = new QVBoxLayout();

	transparentCheckBox = new QCheckBox("Transparent Objects",optionsGroupBox);


	optionsVBoxLayout->addLayout(optionsGridLayout);
	optionsVBoxLayout->addWidget(transparentCheckBox);

	if(mainBRDF!=NULL && dynamic_cast<AnalyticBRDF*>(mainBRDF) != NULL){
		displayImportanceSamplesCheckBox = new QCheckBox("Display importance sampling directions",optionsGroupBox);
		optionsVBoxLayout->addWidget(displayImportanceSamplesCheckBox);	
		displayImportanceSamplesCheckBox->hide();
	}

	optionsGroupBox->setLayout(optionsVBoxLayout);	


 	// fill up the combo box with the number of channels that the BRDF has.
	for(int i=0; i<mainBRDF->getNChannels(); i++){
		channelComboBox->addItem(Ogre::StringConverter::toString(i).c_str());
	}

	channelComboBox->setCurrentIndex(0);



	if(mainBRDF!=NULL && dynamic_cast<MeasuredBRDF*>(mainBRDF) != NULL){
		displayPointsGroupBox = new QGroupBox(tr("Display using"),this);
		displayPointsGroupBox->setMaximumHeight(80);
		displayPointsGroupBox->setMaximumWidth(90);

		pointsRadioButton = new QRadioButton("Points",displayGroupBox);
		meshRadioButton =  new QRadioButton("Mesh",displayGroupBox);

		QVBoxLayout *displayLayout = new QVBoxLayout();

		displayLayout->addWidget(meshRadioButton);
		displayLayout->addWidget(pointsRadioButton);

		displayPointsGroupBox->setLayout(displayLayout);

		meshRadioButton->setChecked(true);

	}




	//connect the channel display with the brdf

	// layout things
	QHBoxLayout *channelSet = new QHBoxLayout();
	channelSet->addWidget(displayGroupBox);
	channelSet->addWidget(optionsGroupBox);

	incidentDirectionGroupBox = new QGroupBox(tr("Incident Direction"),this);
	incidentDirectionGroupBox->setMaximumHeight(70);

	QLabel *text1 = new QLabel(incidentDirectionGroupBox);
	text1->setText("Theta");

	sliderTheta = new QSlider(Qt::Horizontal,incidentDirectionGroupBox);
	if(dynamic_cast<MeasuredBRDF*>(brdfLab->getMainBRDF()) != NULL)
		sliderTheta->setRange(0, dynamic_cast<MeasuredBRDF*>(mainBRDF)->getThetaResolution());
	else
		sliderTheta->setRange(0, 90);
	sliderTheta->setValue(0);
	sliderTheta->setPageStep(5);
	sliderTheta->show();

	QLabel *text2 = new QLabel(incidentDirectionGroupBox);
	text2->setText("Phi");

	sliderPhi = new QSlider(Qt::Horizontal,incidentDirectionGroupBox);
	if(dynamic_cast<MeasuredBRDF*>(brdfLab->getMainBRDF()) != NULL)
		sliderPhi->setRange(0, dynamic_cast<MeasuredBRDF*>(mainBRDF)->getPhiResolution());
	else
		sliderTheta->setRange(0, 180);
	sliderPhi->setValue(0);
	sliderPhi->setPageStep(10);
	sliderPhi->show();

	QGridLayout *gridLayout = new QGridLayout();

	gridLayout->addWidget(text1,0,0,1,1);
	gridLayout->addWidget(text2,1,0,1,1);
	gridLayout->addWidget(sliderTheta,0,1,1,1);
	gridLayout->addWidget(sliderPhi,1,1,1,1);

	incidentDirectionGroupBox->setLayout(gridLayout);	


	QLabel *text3;
	QHBoxLayout *texSizeLayout;

	if((main!=NULL)&&(dynamic_cast<SimulatedBRDF*>(mainBRDF) != NULL)){
		text3 = new QLabel("Lobe Quality",this);
		texSizeLayout = new QHBoxLayout();

		sliderTexSize = new QSlider(Qt::Horizontal,this);
		sliderTexSize->setRange(4, 90);
		sliderTexSize->setValue(10);
		sliderTexSize->show(); // used for lobe quality
		
		texSizeLayout->addWidget(text3);
		texSizeLayout->addWidget(sliderTexSize);

	}

	QHBoxLayout *displayIncident = new QHBoxLayout();
	if(mainBRDF!=NULL && dynamic_cast<MeasuredBRDF*>(mainBRDF) != NULL)
		displayIncident->addWidget(displayPointsGroupBox);
	displayIncident->addWidget(incidentDirectionGroupBox);



	QVBoxLayout *layout = new QVBoxLayout();

	layout->addWidget(window);
	layout->addLayout(displayIncident);
	if((main!=NULL)&&(dynamic_cast<SimulatedBRDF*>(mainBRDF) != NULL)){
		layout->addLayout(texSizeLayout);
	}
	layout->addLayout(channelSet);


	setLayout(layout);


	connect(sliderTheta, SIGNAL(valueChanged(int)), this, SLOT(slidersChanged()));
	//connect(sliderTheta, SIGNAL(sliderReleased()), this, SLOT(slidersChanged()));
	
	connect(sliderPhi, SIGNAL(valueChanged(int)), this, SLOT(slidersChanged()));
	//connect(sliderPhi, SIGNAL(sliderReleased()), this, SLOT(slidersChanged()));

	if(mainBRDF!=NULL && dynamic_cast<MeasuredBRDF*>(mainBRDF) != NULL){
		connect(pointsRadioButton, SIGNAL(clicked(bool)), this, SLOT(setPointsDisplay()));
		connect(meshRadioButton, SIGNAL(clicked(bool)), this, SLOT(setMeshDisplay()));
	}

//	connect(cubeRoot, SIGNAL(stateChanged(int)), this, SLOT(cubeRootStateChanged(int)));
	connect(transparentCheckBox,SIGNAL(stateChanged(int)), this, SLOT(transparentObjectsStateChanged(int)));
	
	if((main!=NULL)&&(dynamic_cast<GPUSimulationBRDF*>(mainBRDF) != NULL))
		connect(sliderTexSize, SIGNAL(valueChanged(int)), dynamic_cast<GPUSimulationBRDF*>(mainBRDF), SLOT(reprocessTexture(int)));
	else if((main!=NULL)&&(dynamic_cast<SimulatedBRDF*>(mainBRDF) != NULL))
		connect(sliderTexSize, SIGNAL(valueChanged(int)), dynamic_cast<SimulatedBRDF*>(mainBRDF), SLOT(displayBRDF(int)));

	//connect the channel display with the brdf
	connect(channelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setDisplayChannel(int)));
	connect(lobeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(displayLobe(int)));
	connect(transformComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setTransformation(int)));
	connect(modeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setDisplayMode(int)));


	if(main!=NULL && dynamic_cast<AnalyticBRDF*>(mainBRDF) != NULL)
		connect(displayImportanceSamplesCheckBox,SIGNAL(stateChanged(int)), this, SLOT(updateImportanceSampleDirections()));



//	setCentralWidget(window);
	window->paintEvent();


	transparentObjectsStateChanged(0);
	setTransformation(0);
	setDisplayChannel(0);			

			

	// to do
//	window->setPrimaryMaterial("AnalyticBRDF");


//	this->parent();

//	resize(650,900);

	window->displayPrimaryMaterial(true);
	window->displaySecondaryMaterial(true);
}

BRDFViewer::~BRDFViewer(){

	delete window;
//	delete sliderTheta;
//	delete sliderPhi;
//	delete cubeRoot;
//	delete transparentCheckBox;

//	if(sliderTexSize != NULL)
//		delete sliderTexSize;

}

#include <iostream>
using namespace std;


/*

This method is used to view the original lobe, the fitted one, or both at the same time.

0-> both
1-> original
2-> fitted

*/
void 
BRDFViewer::displayLobe(int x){

	
	switch(x){

		case 0:

			if(!displayPoints)
				window->displayPrimaryMaterial(true);
			else
				window->displayPrimaryMaterial(false);
				
			window->displaySecondaryMaterial(true);

			break;

		case 1:

			if(!displayPoints)
				window->displayPrimaryMaterial(true);
			else
				window->displayPrimaryMaterial(false);
				
			window->displaySecondaryMaterial(false);

			break;
			
		case 2:

			if(displayPoints)
				window->cleanMeasuredPoints();
			window->displayPrimaryMaterial(false);
			window->displaySecondaryMaterial(true);

			break;

	}

	slidersChanged();
}


void 
BRDFViewer::setTransformation(int x){

	float cubeRoot = 1.f;
	float log = 0.f;

	switch(x){

		case 0: // normal

			cubeRoot = 1.f;
			log = 0.f;

			break;

		case 1:

			cubeRoot = 0.33f;
			log = 0.f;

			break;
			
		case 2:

			cubeRoot = 1.f;
			log = 1.f;

			break;
	}

	// update the materials

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("DisplayAnalyticBRDF");

	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)cubeRoot);
	gpuVertexParam->setNamedConstant("applyLog",(Ogre::Real)log);

	mat = Ogre::MaterialManager::getSingleton().getByName("DisplayFittedBRDF");

	gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)cubeRoot);
	gpuVertexParam->setNamedConstant("applyLog",(Ogre::Real)log);

	mat = Ogre::MaterialManager::getSingleton().getByName("Display2DTextureBRDF");

	gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)cubeRoot);
	gpuVertexParam->setNamedConstant("applyLog",(Ogre::Real)log);

	mat = Ogre::MaterialManager::getSingleton().getByName("Display3DTextureBRDF");

	gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)cubeRoot);
	gpuVertexParam->setNamedConstant("applyLog",(Ogre::Real)log);


	if(displayPoints)
		slidersChanged();	


}


void 
BRDFViewer::cubeRootStateChanged(int x){

	//recover the primary BRDF
	if(cubeRoot->isChecked())
		brdfLab->getMainBRDF()->setCubeRoot(true);
	else
		brdfLab->getMainBRDF()->setCubeRoot(false);

	//if measured, repaint the points
	if(dynamic_cast<MeasuredBRDF*>(brdfLab->getMainBRDF()) != NULL)
		slidersChanged();

	// do the same for the fitted BRDF
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("DisplayFittedBRDF");

	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	if(cubeRoot->isChecked())
		gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)0.33);
	else
		gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)1.);



	mat = Ogre::MaterialManager::getSingleton().getByName("DisplayFittedBRDF");

	gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)0.33);
	gpuVertexParam->setNamedConstant("cubeRoot",(Ogre::Real)1.);




}



/** 
Paint transparent or solid lobes.
*/

void 
BRDFViewer::transparentObjectsStateChanged(int x){

	/**

	setSceneBlending(source,dest);

	Enumerator:
	SBF_ONE 	
	SBF_ZERO 	
	SBF_DEST_COLOUR 	
	SBF_SOURCE_COLOUR 	
	SBF_ONE_MINUS_DEST_COLOUR 	
	SBF_ONE_MINUS_SOURCE_COLOUR 	
	SBF_DEST_ALPHA 	
	SBF_SOURCE_ALPHA 	
	SBF_ONE_MINUS_DEST_ALPHA 	
	SBF_ONE_MINUS_SOURCE_ALPHA 	


	**********************************************************************************************************************

	setSceneBlending(type);

	Enumerator:
	SBT_TRANSPARENT_ALPHA 	Make the object transparent based on the final alpha values in the texture.
	SBT_TRANSPARENT_COLOUR 	Make the object transparent based on the colour values in the texture (brighter = more opaque).
	SBT_ADD 	Add the texture values to the existing scene content.
	SBT_MODULATE 	Multiply the 2 colours together.
	SBT_REPLACE 	The default blend mode where source replaces destination. 

	*/
	//	mat->setSceneBlending(Ogre::SBF_SOURCE_ALPHA ,Ogre::SBF_ONE_MINUS_DEST_COLOUR );
	//	mat->setSceneBlending(Ogre::SBT_ADD);


	if(x == 2){
		// activate blending on all display materials!
	
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("DisplayAnalyticBRDF");
		mat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
		mat->setSceneBlending(Ogre::SBT_ADD);

		mat = Ogre::MaterialManager::getSingleton().getByName("DisplayFittedBRDF");
		mat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
		mat->setSceneBlending(Ogre::SBT_ADD);

		mat = Ogre::MaterialManager::getSingleton().getByName("Display3DTextureBRDF");
		mat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
		mat->setSceneBlending(Ogre::SBT_ADD);

		mat = Ogre::MaterialManager::getSingleton().getByName("Display2DTextureBRDF");
		mat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
		mat->setSceneBlending(Ogre::SBT_ADD);

	}else{
		// desactivate blending on all display materials.
			
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("DisplayAnalyticBRDF");
		mat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(true);
		mat->setSceneBlending(Ogre::SBT_REPLACE);

		mat = Ogre::MaterialManager::getSingleton().getByName("DisplayFittedBRDF");
		mat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(true);
		mat->setSceneBlending(Ogre::SBT_REPLACE);

		mat = Ogre::MaterialManager::getSingleton().getByName("Display3DTextureBRDF");
		mat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(true);
		mat->setSceneBlending(Ogre::SBT_REPLACE);

		mat = Ogre::MaterialManager::getSingleton().getByName("Display2DTextureBRDF");
		mat->getTechnique(0)->getPass(0)->setDepthCheckEnabled(true);
		mat->setSceneBlending(Ogre::SBT_REPLACE);


	}


}


void
BRDFViewer::setDisplayChannel(int x){

	cout << "selected channel: " << x << endl;

	if(dynamic_cast<MeasuredBRDF*>(brdfLab->getMainBRDF()) != NULL)
		brdfLab->getMainBRDF()->changeDisplayChannel(x);

	// set the correct channel to display to the materials!
	Ogre::String materialName("DisplayAnalyticBRDF");

	// get the material
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);

	// get the vertex shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();
	

	Ogre::Vector3 scale;

	switch(x){
		case 0:
			scale = Ogre::Vector3(1.,0.,0.); // todo: for now we can see only the red channel
			break;
		case 1:
			scale = Ogre::Vector3(0.,1.,0.);
			break;
		case 2:
			scale = Ogre::Vector3(0.,0.,1.);
			break;
	}

	gpuVertexParam->setNamedConstant("scale",scale);
	
	// now for the fitted lobe
	materialName = Ogre::String("DisplayFittedBRDF");

	mat = Ogre::MaterialManager::getSingleton().getByName(materialName);

	gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuVertexParam->setNamedConstant("scale",scale);


/*

	brdfLab->getMainBRDF()->changeDisplayChannel(x);

	// to solve!!!	
	BRDF* fitting = brdfLab->getFittingBRDF();
	if(fitting != NULL)
		fitting->changeDisplayChannel(x);
*/
	slidersChanged();
}

void 
BRDFViewer::slidersChanged(){

//	if((!sliderTheta->isSliderDown())&&(!sliderPhi->isSliderDown()))

/*		if((previousThetaValue != sliderPhi->value())||(previousPhiValue != sliderTheta->value())){

			// update values
			previousThetaValue = sliderPhi->value();
			previousPhiValue = sliderTheta->value();
*/
			float phi; 

			float theta;// = (float)sliderTheta->value()*PI/(2*100);
			
			// look if the primary BRDF is measured
			
				// get the constrains

				
			
			//recover the primary BRDF
			BRDF* mainBRDF = brdfLab->getMainBRDF();
			
			//QString mainMaterialName(mainBRDF->getMaterialType());

			if(dynamic_cast<MeasuredBRDF*>(mainBRDF) != NULL){// mainMaterialName == "Measured" ){

				window->setPrimaryMaterial("Display3DTextureBRDF");

				//compute the incident vector given by the sliders
				phi = ((float)sliderPhi->value()*2*Ogre::Math::PI)/((float)(sliderPhi->maximum()+1));
				theta = ((float)sliderTheta->value()*Ogre::Math::PI)/(2*((float)(sliderTheta->maximum()+1)));
				
				//convert to spherical coordinates
				incidentVector = BRDF::spherical2cart(Ogre::Vector2(phi,theta));


				// Set the incident vector to the actual material
				Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Display3DTextureBRDF");

				Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

				gpuVertexParam->setNamedConstant("incidentVector",incidentVector);


				if(displayPoints && lobeComboBox->currentIndex() < 2){
					phi = ((float)sliderPhi->value()*2*Ogre::Math::PI)/((float)((dynamic_cast<MeasuredBRDF*>(mainBRDF))->getPhiResolution()+1));

					theta = ((float)sliderTheta->value()*Ogre::Math::PI)/(2*((float)((dynamic_cast<MeasuredBRDF*>(mainBRDF))->getThetaResolution()+1)));
					
					//convert to spherical coordinates
					incidentVector = BRDF::spherical2cart(Ogre::Vector2(phi,theta));

					std::vector<Ogre::Vector3> points;
					
					(dynamic_cast<MeasuredBRDF*>(mainBRDF))->setMeasuredPoints(phi,theta, &points,transformComboBox->currentIndex()); 

					window->paintMeasuredPoints(&points);

					points.clear();

				}

				
				//if we paint 2 lobes, do the same for the second one.
			}else if (dynamic_cast<SimulatedBRDF*>(mainBRDF) != NULL){

				window->setPrimaryMaterial("Display2DTextureBRDF");


				//compute the incident vector given by the sliders
				phi = ((float)sliderPhi->value()*2*Ogre::Math::PI)/((float)(sliderPhi->maximum()+1));
				theta = ((float)sliderTheta->value()*Ogre::Math::PI)/(2*((float)(sliderTheta->maximum()+1)));
				
				//convert to spherical coordinates
				incidentVector = BRDF::spherical2cart(Ogre::Vector2(phi,theta));


				//TODO: angles in inverse order, solve
				dynamic_cast<SimulatedBRDF*>(brdfLab->getMainBRDF())->setIncidentVector(Ogre::Vector2(phi,theta));

				dynamic_cast<SimulatedBRDF*>(brdfLab->getMainBRDF())->displayBRDF(sliderTexSize->value());
			}else if(dynamic_cast<RawDataBRDF*>(mainBRDF) != NULL){

/*
				Ogre::Vector2 sphC = BRDF::cart2spherical(dynamic_cast<FromPicturesBRDF*>(mainBRDF)->getIncidentAvg());
				phi = sphC[0];
				theta = sphC[1];
*/

				// allow normal movement on the viewer
				phi = ((float)sliderPhi->value()*2*Ogre::Math::PI)/((float)(sliderPhi->maximum()+1));
				theta = ((float)sliderTheta->value()*Ogre::Math::PI)/(2*((float)(sliderTheta->maximum()+1)));
				
				//convert to spherical coordinates
				incidentVector = BRDF::spherical2cart(Ogre::Vector2(phi,theta));


				if(lobeComboBox->currentIndex() < 2){

					std::vector<Ogre::Vector3> *u = dynamic_cast<RawDataBRDF*>(mainBRDF)->getIncidentDirections(); // not required
					std::vector<Ogre::Vector3> *v = dynamic_cast<RawDataBRDF*>(mainBRDF)->getOutgoingDirections();
					std::vector<Ogre::Vector3> *y = dynamic_cast<RawDataBRDF*>(mainBRDF)->getIntensities();

					std::vector<Ogre::Vector3> points;

					
					float cubeRoot = 1.f;
					float applyLog = 0.f;

					switch(transformComboBox->currentIndex()){

						case 1:
							cubeRoot = 0.33f;
							break;
						case 2:
							applyLog = 1.f;
							break;
					}

					
					for(unsigned int i=0; i<v->size(); i++){

						Ogre::Vector2 inci(BRDF::cart2spherical((*u)[i]));

						if(inci[1] > theta-0.05 && inci[1] < theta+0.05){
							float mainIntensity =(*y)[i][channelComboBox->currentIndex()];///cos(theta); // important: remove the cos(theta) from the input data

							mainIntensity = ((1.-applyLog)*pow(mainIntensity,cubeRoot)+applyLog*log(mainIntensity+1.));

							points.push_back((*v)[i]*mainIntensity); // to contrarest the cost(theta) in the pictures
						}	

					}

					window->paintMeasuredPoints(&points);

					points.clear();

					displayPoints = true;
				}

			}else{ // otherwise, analytic
			

				window->setPrimaryMaterial("DisplayAnalyticBRDF");

				//compute the incident vector given by the sliders
				phi = ((float)sliderPhi->value()*2*Ogre::Math::PI)/((float)(sliderPhi->maximum()+1));
				theta = ((float)sliderTheta->value()*Ogre::Math::PI)/(2*((float)(sliderTheta->maximum()+1)));

				//convert to spherical coordinates
				incidentVector = BRDF::spherical2cart(Ogre::Vector2(phi,theta));

				// Set the incident vector to the actual material
				Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("DisplayAnalyticBRDF");

				Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

				gpuVertexParam->setNamedConstant("incidentVector",incidentVector);

				// debug feature, paint important sampled directions!
			
				updateImportanceSampleDirections();


			}
			

			window->setIncidentVector(phi, theta);
			
		//	BRDF *fitted =  brdfLab->getFittingBRDF();

		//	if(fitted != NULL){ //if something has been fitted... THIS WILL BE ANALYTIC.

		//		QString fittedBRDFMaterialName(fitted->getMaterialName());

		//		dynamic_cast<AnalyticBRDF*>(fitted)->setMaterialParameters();

				// TODO: not so sure about that.


			window->setSecondaryMaterial("DisplayFittedBRDF");

			// Set the incident vector to the actual material
			Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("DisplayFittedBRDF");

			Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

			gpuVertexParam->setNamedConstant("incidentVector",incidentVector);

//		}


}

void
BRDFViewer::sliderTexSizeChanged(){
	
	
//	window->createIntensityTexture(sliderTexSize->value());

}
/*
void
BRDFViewer::resizeEvent( QResizeEvent * event ){



}*/


void 
BRDFViewer::initSliders(){

	sliderTheta->setValue(0);
	sliderPhi->setValue(0);


}

void 
BRDFViewer::updateImportanceSampleDirections(){

	if(displayImportanceSamplesCheckBox->isChecked()){
		std::vector<Ogre::Vector3> directions;

		dynamic_cast<AnalyticBRDF*>(brdfLab->getMainBRDF())->generateSampleDirections(incidentVector, &directions);

		window->drawImportanceSamplingDirections(&directions);

		directions.clear();
	}else{
		window->drawImportanceSamplingDirections(NULL);
	}

}

void 
BRDFViewer::setMeshDisplay(){

	displayPoints = false;

	// hide points if exist
	window->cleanMeasuredPoints();

	// show original mesh
	window->displayPrimaryMaterial(true);
}

void 
BRDFViewer::setPointsDisplay(){

	displayPoints = true;

	// hide original mesh
	window->displayPrimaryMaterial(false);

	// create points
	slidersChanged();
}

void
BRDFViewer::setDisplayMode(int x){

	switch(x){

		case 0: // default, spheres

			// set spheres
			window->setObjects(0);

			// set the right parameter to use the default parameterization

			break;
		case 1: // half way vector, spheres but other parameterization

			// set spheres
			window->setObjects(1);
			// set the right parameter to use the halfway parameterization

			break;
		case 2: // 3D plot, 

			// set quads
			window->setObjects(2);

			// set the right parameter to use the quads

			break;
	}

	slidersChanged();
}