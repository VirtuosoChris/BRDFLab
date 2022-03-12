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

using namespace std;

ObjectRenderer::ObjectRenderer(QWidget *parent):
	QWidget( parent ),
	originalMaterialType(-1),
	currentLightType(0)
 {
//			setupUi(this);

			ogreWindow = new OgreObjectRenderWindow(this);

			// tone mapping options

			QLabel *keyLabel = new QLabel("Key:");
			QLabel *gammaLabel = new QLabel("Gamma:");

			keyDoubleSpinBox = new QDoubleSpinBox();
			gammaDoubleSpinBox = new QDoubleSpinBox();


			toneMapGroupBox = new QGroupBox(tr("Tone Mapping"),this);
			toneMapGroupBox->setMaximumHeight(80);
			toneMapGroupBox->setMaximumWidth(130);

			QGridLayout *toneMapGridLayout = new QGridLayout();

			toneMapGridLayout->addWidget(keyLabel,0,0,1,1);
			toneMapGridLayout->addWidget(gammaLabel,1,0,1,1);
			toneMapGridLayout->addWidget(keyDoubleSpinBox,0,1,1,1);
			toneMapGridLayout->addWidget(gammaDoubleSpinBox,1,1,1,1);

			toneMapGroupBox->setLayout(toneMapGridLayout);	

			materialGroupBox = new QGroupBox(tr("Material"),this);
			materialGroupBox->setMaximumHeight(80);
			materialGroupBox->setMaximumWidth(90);

			originalRadioButton = new QRadioButton("Original",materialGroupBox);
			fittedRadioButton =  new QRadioButton("Fitted",materialGroupBox);

			QVBoxLayout *materialLayout = new QVBoxLayout();

			materialLayout->addWidget(originalRadioButton);
			materialLayout->addWidget(fittedRadioButton);

			materialGroupBox->setLayout(materialLayout);


			parametersGroupBox = new QGroupBox(tr("Parameters"),this);
			parametersGroupBox->setMaximumHeight(80);
			parametersGroupBox->setMaximumWidth(500);


			lightLabel = new QLabel("Light:");
			lightLabel->setAlignment(Qt::AlignRight | Qt::AlignHCenter);

			lightsComboBox = new QComboBox(parametersGroupBox);

			modelLabel = new QLabel("Models:");
			modelLabel->setAlignment(Qt::AlignRight | Qt::AlignHCenter);

			probeLabel = new QLabel("Probe:");
			probeLabel->setAlignment(Qt::AlignRight | Qt::AlignHCenter);

			probeComboBox = new QComboBox(parametersGroupBox);

			modelsComboBox = new QComboBox(parametersGroupBox);

//			ambientSpinBox = new QDoubleSpinBox(parametersGroupBox);
//			ambientSpinBox->setValue(1.);
//			ambientSpinBox->setSingleStep(0.1);
		
			QGridLayout *parametesrGridLayout = new QGridLayout();
			
			parametesrGridLayout->addWidget(lightLabel,0,0,1,1);
			parametesrGridLayout->addWidget(modelLabel,1,0,1,1);
			parametesrGridLayout->addWidget(probeLabel,0,2,1,1);
			parametesrGridLayout->addWidget(lightsComboBox,0,1,1,1);
			parametesrGridLayout->addWidget(modelsComboBox,1,1,1,1);
			parametesrGridLayout->addWidget(probeComboBox,0,3,1,1);

			parametersGroupBox->setLayout(parametesrGridLayout);


			QHBoxLayout *hlayout = new QHBoxLayout();

			hlayout->addWidget(materialGroupBox);
			hlayout->addWidget(toneMapGroupBox);
			hlayout->addWidget(parametersGroupBox);



/*
//			textSamples = new QLabel("NSamples: ");
//			textSamples->setAlignment(Qt::AlignRight | Qt::AlignHCenter);

//			nSamplesSpinBox = new QSpinBox();
*/

			QVBoxLayout *vLayout = new QVBoxLayout();
			vLayout->addWidget(ogreWindow);
			vLayout->addLayout(hlayout);

			setLayout(vLayout);

			ogreWindow->paintEvent();

			// add lights

			lightsComboBox->addItem("Point Light");
//			lightsComboBox->addItem("Mirror Environment Map");
			lightsComboBox->addItem("Filtered Environment Map");
//			lightsComboBox->addItem("PointLight3DTextureBRDF");

			// add models
			modelsComboBox->addItem("Sphere");
			modelsComboBox->addItem("Dragon");
			modelsComboBox->addItem("Happy");
			modelsComboBox->addItem("XYZ-RGB Dragon");
			modelsComboBox->addItem("Armadillo");
			modelsComboBox->addItem("Bunny");

			// add probes
			probeComboBox->addItem("Ennis");
			probeComboBox->addItem("Cathedral");
			probeComboBox->addItem("Kitchen");
			probeComboBox->addItem("Campus");
			probeComboBox->addItem("Beach");
			probeComboBox->addItem("Building");
			probeComboBox->addItem("Galileo");
			probeComboBox->addItem("Uffizi");
			probeComboBox->addItem("St Peters");

			//connect QCombo signals to OgreObjectRenderWindow slots
			connect(lightsComboBox, SIGNAL(activated(int)), this, SLOT(setLightType(int)));
			connect(lightsComboBox, SIGNAL(activated(int)), this, SLOT(adaptGUI(int)));
			connect(modelsComboBox, SIGNAL(activated(int)), ogreWindow, SLOT(setModel(int)));
			connect(probeComboBox, SIGNAL(activated(int)), ogreWindow, SLOT(setProbe(int)));
			//connect(nSamplesSpinBox, SIGNAL(valueChanged(int)), ogreWindow, SLOT(setNSamples(int)));
			connect(keyDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setKeyGamma()));
			connect(gammaDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setKeyGamma()));		
			

			connect(originalRadioButton, SIGNAL(clicked(bool)), this, SLOT(setOriginalMaterial()));
			connect(fittedRadioButton, SIGNAL(clicked(bool)), this, SLOT(setFittedMaterial()));
	
			//set default values to show
			ogreWindow->setModel(1);	
			modelsComboBox->setCurrentIndex(1);
			ogreWindow->setLightType(0); //todo change to 0
			ogreWindow->setLightType(1); //todo change to 0
			// set the default radio button active.
			originalRadioButton->setChecked(true);
			fittedRadioButton->setDisabled(true);

			//nSamplesSpinBox->setMaximum(10000);
			//nSamplesSpinBox->setValue(100);
//			ogreWindow->setNSamples(nSamplesSpinBox->value());

			//keyDoubleSpinBox values
			keyDoubleSpinBox->setMaximum(3.);
			keyDoubleSpinBox->setMinimum(-3.);
			keyDoubleSpinBox->setSingleStep(0.10);
			keyDoubleSpinBox->setValue(0.);

			//gammaDoubleSpinBox values 0.5f, 3.f,
			gammaDoubleSpinBox->setMaximum(3.);
			gammaDoubleSpinBox->setMinimum(0.5);
			gammaDoubleSpinBox->setSingleStep(0.10);
			gammaDoubleSpinBox->setValue(1.8);

/* 
//keyDoubleSpinBox values
keyDoubleSpinBox->setMaximum(1000.);
keyDoubleSpinBox->setMinimum(0.);
keyDoubleSpinBox->setSingleStep(0.10);
keyDoubleSpinBox->setValue(1.);

//gammaDoubleSpinBox values 0.5f, 3.f,
gammaDoubleSpinBox->setMaximum(2.);
gammaDoubleSpinBox->setMinimum(0.3);
gammaDoubleSpinBox->setSingleStep(0.10);
gammaDoubleSpinBox->setValue(0.5);

*/
			
	

 }
/*
void 
ObjectRenderer::ambientLightChanged(double x){

	// set the new value to all the point light shaders!

	// Analytical Materials
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("PointLightAnalyticBRDF");
	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("ambient",(Ogre::Real)x);


	// 3D texture point light
	mat = Ogre::MaterialManager::getSingleton().getByName("PointLight3DTextureBRDF");
	gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("ambient",(Ogre::Real)x);

 }
 */


 void
ObjectRenderer::activeFittedRadioButton(bool active){

//	originalRadioButton->setEnabled(active);
	fittedRadioButton->setEnabled(active);

	if(!active)
		originalRadioButton->click();


}


 void
ObjectRenderer::clickOriginalRadioButton(){

		 originalRadioButton->click();

 }


void
ObjectRenderer::setKeyGamma(){

	Ogre::Vector3 value(keyDoubleSpinBox->value(), gammaDoubleSpinBox->value(),0.);
	ogreWindow->setKeyGamma(value);

}

void 
ObjectRenderer::adaptGUI(int x){

//	cout << "hola" << endl;
		/*
	switch(x){

		case 0: // point light
//			nSamplesSpinBox->hide();
			//textSamples->hide();
		case 1: // simple env map
			//nSamplesSpinBox->hide();
			//textSamples->hide();
		case 2: // env map
			//nSamplesSpinBox->show();
			//textSamples->show();


	}*/



}


/**

Materials:

Point Light rendering:
 - Analytical: PointLightAnalyticBRDF
 - Measured: PointLight3DTextureBRDF

Env. Map. rendering:
 - Analytical: EnvMapAnalyticBRDF
 
*/



void 
ObjectRenderer::setOriginalMaterial(){

	originalRadioButton->setDisabled(false);
	originalRadioButton->setChecked(true);

	emit setActiveBRDF(0);

	setLightMaterials(originalMaterialType);
}

void 
ObjectRenderer::setFittedMaterial(){
	emit setActiveBRDF(1);

	setLightMaterials(0);
}

int
ObjectRenderer::getLightType(){
	
	return lightsComboBox->currentIndex();

}

void
ObjectRenderer::setLightType(int l){

	currentLightType = l;

	if(l == 1){
		probeComboBox->show();
		probeLabel->show();
	}else{
		probeComboBox->hide();
		probeLabel->hide();
	}

	// TODO: remove this when we have rendering for measured data and simulated data with env. map. lighting.
	if(l == 1){

		// get the material to render
		Ogre::String name = ogreWindow->getEnvMapMaterialName();

		// disable original rendering, check fitted.
		if(name == "black"){

			originalRadioButton->setDisabled(true);
			originalRadioButton->update();
			fittedRadioButton->click();

		}

	}else{

			originalRadioButton->setDisabled(false);

	}


/*
	if(l == 0){ // point light rendering
		ambientLabel->show();
		ambientSpinBox->show();
	}else{// env map rendering
		ambientLabel->hide();
		ambientSpinBox->hide();
	}
*/

	ogreWindow->setLightType(currentLightType);
	emit lightTypeChanged(currentLightType);
/*
	if(isOriginalBRDFRendering())
		setLightMaterials(originalMaterialType);
	else
		setLightMaterials(0);
	*/
//	connect(lightsComboBox, SIGNAL(activated(int)), ogreWindow, SLOT(setLightType(int)));
//	connect(lightsComboBox, SIGNAL(activated(int)), this, SIGNAL(lightTypeChanged(int)));
}

void
ObjectRenderer::setOriginalMaterialType(int t){
	
	if(t == -1)// reset the interface
		originalMaterialType = -1;

	if(originalMaterialType == -1) // define the original material scheme
		setLightMaterials(t);
	
	originalMaterialType = t;	

}

void
ObjectRenderer::setLightMaterials(int i){

	switch(i){

		case 0: // analytic models.
			ogreWindow->setMaterials("PointLightAnalyticBRDF","EnvMapAnalyticBRDF");
			break;
		case 1: // measured data.
			ogreWindow->setMaterials("PointLight3DTextureBRDF","black");
			break;
		case 2: // simulated data.
			ogreWindow->setMaterials("PointLight3DTextureBRDF","black");
			break;

	}
	setLightType(currentLightType);

}