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

#include "PropertyEditorWidget.h"

using namespace std;

PropertyEditorWidget::PropertyEditorWidget(BRDF *source):
	src(source),
	optimizing(false),
	actualBRDF(-1),
	//brdfOptions(NULL),
	brdfsCreated(0),
	currentMaterial(-1),
	brdfStatus(false)
{
	// combo box to select the BRDF
	comboBox = new QComboBox(this);

	addBRDFButton = new QPushButton("+", this);
	addBRDFButton->setMaximumWidth(25);
	delBRDFButton = new QPushButton("-", this);
	delBRDFButton->setMaximumWidth(25);

	QHBoxLayout *brdfsLayout = new QHBoxLayout();
	brdfsLayout->addWidget(comboBox);
	brdfsLayout->addWidget(addBRDFButton);
	brdfsLayout->addWidget(delBRDFButton);

	addLobeButton = new QPushButton("Add Lobe", this);
	clearBRDFButton = new QPushButton("Clear", this);
	clearBRDFButton->setMaximumWidth(56);

	QHBoxLayout *addLobeClearLayout = new QHBoxLayout();
	addLobeClearLayout->addWidget(addLobeButton);
	addLobeClearLayout->addWidget(clearBRDFButton);

	variantManager = new QtVariantPropertyManager();
	variantFactory = new QtVariantEditorFactory();
	variantEditor = new QtTreePropertyBrowser();
	variantEditor->setFactoryForManager(variantManager, variantFactory);
	variantEditor->setPropertiesWithoutValueMarked(true);
	variantEditor->setRootIsDecorated(false);

	QLabel *errorLabel = new QLabel("Error:");

	errorSpinBox = new QDoubleSpinBox();
	errorSpinBox->setMaximum(10000000000000.);
	errorSpinBox->setReadOnly(true);
	errorSpinBox->setDecimals(6);
	errorSpinBox->setValue(-1.);
//	errorSpinBox->setDisabled(true);

	QHBoxLayout *errorDisplayLayout = new QHBoxLayout();
	errorDisplayLayout->addWidget(errorLabel);
	errorDisplayLayout->addWidget(errorSpinBox);

	optimizeButton = new QPushButton("Optimize Parameters", this);

	QVBoxLayout *layout = new QVBoxLayout();

	layout->addLayout(brdfsLayout);
	layout->addLayout(addLobeClearLayout);
	layout->addWidget(variantEditor);
	layout->addLayout(errorDisplayLayout);
	layout->addWidget(optimizeButton);

	setLayout(layout);

	//make connections
	connect(addBRDFButton, SIGNAL(clicked()), this, SLOT(addBRDF()));	
	connect(delBRDFButton, SIGNAL(clicked()), this, SLOT(delBRDF()));	
	connect(addLobeButton, SIGNAL(clicked()), this, SLOT(addLobe()));	
	connect(clearBRDFButton, SIGNAL(clicked()), this, SLOT(clearBRDF()));	
	connect(optimizeButton, SIGNAL(clicked()), this, SLOT(optimize()));	
	connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(brdfChanged(int)));	
	

	setFittingGUI();



/*
	comboBox = new QComboBox(this);



	comboBox->addItem(dst->getMaterialName());

	newFittingButton = new QPushButton("Clear", this);
	
	QHBoxLayout *fittingsLayout = new QHBoxLayout();
	fittingsLayout->addWidget(comboBox);
	fittingsLayout->addWidget(newFittingButton);

	brdfOptions = new QPropertyEditorWidget(this);
	dst->setQPropertyEditorWidget(brdfOptions);

	addLobeButton = new QPushButton("Add Lobe", this);

	QLabel *errorLabel = new QLabel("Error:");

	errorSpinBox = new QDoubleSpinBox();
	errorSpinBox->setMaximum(10000000000000.);
	errorSpinBox->setReadOnly(true);
	errorSpinBox->setDecimals(6);
	errorSpinBox->setValue(-1.);

	QHBoxLayout *errorDisplayLayout = new QHBoxLayout();
	errorDisplayLayout->addWidget(errorLabel);
	errorDisplayLayout->addWidget(errorSpinBox);

	optimizeButton = new QPushButton("Optimize Parameters", this);

	connect(newFittingButton, SIGNAL(clicked()), this, SLOT(clear()));	
	connect(addLobeButton, SIGNAL(clicked()), destination, SLOT(addLobe()));	
	connect(optimizeButton, SIGNAL(clicked()), this, SLOT(optimize()));	

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(fittingsLayout);
	layout->addWidget(addLobeButton);
	layout->addWidget(brdfOptions);
	layout->addLayout(errorDisplayLayout);
	layout->addWidget(optimizeButton);


	lmfit = new LMFitting(src,dst);



	setLayout(layout);

	*/
}

void 
PropertyEditorWidget::setDatabaseGUI()
{
	addBRDFButton->show();
	delBRDFButton->hide();
	addLobeButton->hide();
	clearBRDFButton->hide();

	optimizeButton->hide();

}

void 
PropertyEditorWidget::setFittingGUI()
{
	addBRDFButton->show();
	delBRDFButton->show();
	addLobeButton->show();
	clearBRDFButton->show();
	optimizeButton->show();
}

/**
Method to add a BRDF to fit with user interaction.

*/
void 
PropertyEditorWidget::addBRDF(){

	// create new BRDF

	// ask for a name for the new BRDF
	bool ok;

	Ogre::String brdfName("Fit-");

	brdfName += Ogre::StringConverter::toString(brdfsCreated++);

	QString name = QInputDialog::getText(this, tr("BRDFLab"),
										  tr("Parametric model name:"), QLineEdit::Normal,
										  QString(brdfName.c_str()), &ok);
	if (!ok || name.isEmpty())
		return;

	int nChannels = src->getNChannels();

	// ask for Linear or Fixed
	 QStringList items;
	 items << tr("Linear") << tr("Fixed");

	 QString combinationType = QInputDialog::getItem(this, tr("BRDFLab"),
										  tr("Combination:"), items, 0, false, &ok);

	 if (!ok)
		 return;

	FittedBRDF *fittedBRDF;

	// create analytic BRDF
	if(combinationType == "Linear")
		fittedBRDF = new FittedBRDF(nChannels, AnalyticBRDF::Linear,name);
	else
		fittedBRDF = new FittedBRDF(nChannels, AnalyticBRDF::Fixed,name);


	// set the BRDF to the window
	fittedBRDF->setGUIManagers(variantManager,variantEditor);

	// set the new brdf to the vector and also to the dst pointer
//	actualBRDF = brdfs.size();
	brdfs.push_back(fittedBRDF);

	comboBox->addItem(fittedBRDF->getMaterialName());
	comboBox->setCurrentIndex(brdfs.size()-1);

	connect(fittedBRDF,SIGNAL(brdfModified()),this,SLOT(brdfModified()));

	emit brdfModified();

}

/**
Method to add a BRDF from file.

*/
void
PropertyEditorWidget::addBRDF(QDomElement *e){

	QString name;
	QString type;
	QString lobeCombination;
	QDomElement parameters;
	int nChannels;

	QDomNode ii = e->firstChild(); // fitting stuff..
	QDomElement iie = ii.toElement();

	QDomNode i = iie.firstChild();
	
	while(!i.isNull()){
		QDomElement ie = i.toElement();
		
		if(!ie.isNull()){

			if(ie.tagName() == "Name")
				name = ie.text();
			else if( ie.tagName() == "Type")
				type = ie.text();
			else if( ie.tagName() == "LobeCombination")
				lobeCombination = ie.text();
			else if( ie.tagName() == "Parameters"){

				parameters = ie;

				// analytic
				if(ie.hasAttribute("nChannels")){
					nChannels = ie.attribute("nChannels").toInt();
				}

			}else{
				cout << "Warning information of tag " << ie.tagName().toStdString() << " not parsed." << endl;
			}
		}

		i = i.nextSibling();
	}

	// crear la BRDF
	FittedBRDF *fittedBRDF;

	if(type == "Analytic"){
		if(lobeCombination == "Linear")
			fittedBRDF = new FittedBRDF(nChannels, AnalyticBRDF::Linear,name);
		else
			fittedBRDF = new FittedBRDF(nChannels, AnalyticBRDF::Fixed,name);
		

	}else{
		cout << "Error, is not possible to fit to a non analytical model. File error!" << endl;
		return;
	}

	fittedBRDF->setGUIManagers(variantManager,variantEditor);

	// load parameter information
	fittedBRDF->loadXML(parameters);
	
	// load fitting data from file
	fittedBRDF->loadFittingParametersXML(*e);
	
//	actualBRDF = brdfs.size();
	brdfs.push_back(fittedBRDF);

	comboBox->addItem(fittedBRDF->getMaterialName());
	comboBox->setCurrentIndex(brdfs.size()-1);

	emit brdfModified();
}


void 
PropertyEditorWidget::brdfChanged(int index){

// check this!
//	brdfs[actualBRDF]->setDisplayMaterial(false);
	for(unsigned int i=0; i<brdfs.size();i++){
		brdfs[i]->setActiveBRDF(false);
		disconnect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)), brdfs[i], 0);
	}
	
	actualBRDF = index;
	
	if(index != -1)
		brdfs[actualBRDF]->linkGUI();
	
	if(index != -1){
		if(brdfs[index]->isReadOnly()){
			setDatabaseGUI();
			brdfs[index]->recreateParameterGUI(false);
		}else{
			setFittingGUI();
			brdfs[index]->recreateParameterGUI(true);
		}

		brdfs[index]->generateShaders();

		errorSpinBox->setValue(brdfs[index]->getError());
//		brdfs[index]->guiUpdated();
		setActiveBRDF(true);
		brdfs[index]->setMaterial(currentMaterial);
	

	}
}

/**
Method to delete the current BRDF

*/
void
PropertyEditorWidget::delBRDF(){

	if(actualBRDF  != -1){

		disconnect(brdfs[actualBRDF],SIGNAL(brdfModified()),this,SLOT(brdfModified()));

		// delete the BRDF
		delete brdfs[actualBRDF];

		// erase the brdf from the vector
		brdfs.erase (brdfs.begin()+actualBRDF);

		variantManager->clear();
		variantEditor->clear();

		// remove from the comboBox
		comboBox->removeItem(actualBRDF);

		emit brdfModified();

	}

}



void
PropertyEditorWidget::addLobe(){
	if(actualBRDF != -1){
		brdfs[actualBRDF]->addLobe();

		emit brdfModified();
	}
}

void 
PropertyEditorWidget::clearBRDF(){
	
	if(actualBRDF != -1){
		brdfs[actualBRDF]->clear();

		emit brdfModified();
	}
	// delete actual
//	brdfs[actualBRDF]->clearBRDF()
	// new with the same name!
}

Fitting*
PropertyEditorWidget::createOptimizer(QString optimizer, bool nonLinearOptimization){
	
	if(optimizer == "Levenberg-Marquardt")
		return new LMFitting(src,brdfs[actualBRDF],nonLinearOptimization);
	else
		return new NLoptFitting(src,brdfs[actualBRDF],nonLinearOptimization, optimizer);
	

/*
	else if(optimizer == "COBYLA")
		return new NLoptFitting(src,brdfs[actualBRDF],nonLinearOptimization, NLOPT_LN_COBYLA);
	else if(optimizer == "BOBYQA")
		return new NLoptFitting(src,brdfs[actualBRDF],nonLinearOptimization, NLOPT_LN_BOBYQA);
	else if(optimizer == "NEWUOA")
		return new NLoptFitting(src,brdfs[actualBRDF],nonLinearOptimization, NLOPT_LN_NEWUOA_BOUND);
	else if(optimizer == "PRAXIS")
		return new NLoptFitting(src,brdfs[actualBRDF],nonLinearOptimization, NLOPT_LN_PRAXIS);
	else if(optimizer == "Nelder-Mead Simplex")
		return new NLoptFitting(src,brdfs[actualBRDF],nonLinearOptimization, NLOPT_LN_NELDERMEAD);
	else if(optimizer == "Sbplx")
		return new NLoptFitting(src,brdfs[actualBRDF],nonLinearOptimization, NLOPT_LN_SBPLX);
//	else if(optimizer == "Augmented Lagrangian algorithm")
//		return new NLoptFitting(src,brdfs[actualBRDF],nonLinearOptimization, NLOPT_LN_SBPLX);
*/
}


void
PropertyEditorWidget::optimize(){
	


	if(!optimizing){

		if(actualBRDF != -1 && actualBRDF < brdfs.size()){
			
			// ask for the Optimizer

			QStringList optimizersAvaliable;
			optimizersAvaliable	<< tr("Levenberg-Marquardt")
				<< tr("COBYLA")
				<< tr("BOBYQA")
				<< tr("NEWUOA")
				<< tr("PRAXIS")
				<< tr("Nelder-Mead Simplex")
				<< tr("Sbplx");
//				<< tr("Augmented Lagrangian algorithm");


			bool okOptimizer;
			QString optimizerName = QInputDialog::getItem(this, tr("BRDFLab: Non linear optimization routine selection"),
				tr("Name:"), optimizersAvaliable, 0, false, &okOptimizer);

			if (!okOptimizer)
				return;

			// ask for the Error metric
			QStringList errorMetrics;
			errorMetrics << tr("L^2")
				<< tr("L^2 cosine weight")
				<< tr("Ngan et al.");

			bool okErrorMetric;
			QString errorMetricName = QInputDialog::getItem(this, tr("BRDFLab: Non linear optimization routine selection"),
				tr("Name:"), errorMetrics, 0, false, &okErrorMetric);

			if (!okErrorMetric)
				return;

			int errorMetric = 0;

			if (errorMetricName == "L^2 cosine weight")
				errorMetric = 1;
			else if(errorMetricName == "Ngan et al.")
				errorMetric = 2;
			

			cout << "Optimizer: " << optimizerName.toStdString() << endl;
			cout << "Error Metric: " << errorMetricName.toStdString() << endl;

			if(brdfs[actualBRDF]->linearCombination()){

				QStringList items;
				items << tr("Linear") << tr("NonLinear");

				bool ok;
				QString item = QInputDialog::getItem(this, tr("BRDFLab: Select optimization mode"),
											  tr("Mode:"), items, 0, false, &ok);

				if (!ok)
					return;

				disableGui();
				optimizing = true;
				
				// TODO: solve the Nlopt optimizers to work with non linear + least squares on optimization
				if(optimizerName != "Levenberg-Marquardt"){
					item = QString("NonLinear");
				}


				if(item == "Linear")
					fit = createOptimizer(optimizerName);//new LMFitting(src,brdfs[actualBRDF]);
				else
					fit = createOptimizer(optimizerName,true);//new LMFitting(src,brdfs[actualBRDF],true);

			}else{
				optimizing = true;
				fit = createOptimizer(optimizerName);//new LMFitting(src,brdfs[actualBRDF]);
			}
			
			optimizeButton->setText("Stop");

			fit->setErrorGUI(errorSpinBox);
			fit->setErrorMetric(errorMetric);
			
			
			fit->optimize();
			brdfs[actualBRDF]->setOptimizerName(optimizerName);

			switch(fit->getStatus()){
	/*
				case 1:
					QMessageBox::warning(this,tr("BRDFLab"),tr("Optimizer stopped by small gradient J^T e"));
					break;
				case 2:
					QMessageBox::warning(this,tr("BRDFLab"),tr("Optimizer stopped by small Dp"));
					break;
				case 3:
					QMessageBox::warning(this,tr("BRDFLab"),tr("Optimizer stopped by itmax"));
					break;
				case 4:
					QMessageBox::warning(this,tr("BRDFLab"),tr("Singular matrix. Restart from current p with increased mu "));
					break;
				case 5:
					QMessageBox::warning(this,tr("BRDFLab"),tr("No further error reduction is possible. Restart with increased mu"));
					break;
				case 6:
					QMessageBox::warning(this,tr("BRDFLab"),tr("Optimizer stopped by small ||e||_2"));
					break;
	*/
				case 7:
					QMessageBox::warning(this,tr("BRDFLab"),tr("Optimizer stopped by invalid (i.e. NaN or Inf) func values. This is a user error"));
					break;

			}

	//		if(fit->getStatus() == 7)
	//			QMessageBox::warning(this,tr("BRDFLab"),tr("Optimizer stopped by invalid (i.e. NaN or Inf) func values."));

			
			delete fit;

			//brdfOptions->expandAll();
			optimizeButton->setText("Optimize Parameters");
			enableGui();
			optimizing = false;
		
			brdfs[actualBRDF]->setError(errorSpinBox->value());
		}
	}else{
	
		fit->stop();
		
	}

	emit brdfModified();
}


void
PropertyEditorWidget::writeXML(QDomDocument *doc, QDomElement *parent){

	// for each BRDF in the vector, store it to disk.
	for(unsigned int i=0; i<brdfs.size(); i++){
		brdfs[i]->writeXML(doc,parent);
	}

}
/*
void 
PropertyEditorWidget::clear(){
		
	if(fit != NULL)
		delete fit;

	AnalyticBRDF *analytic;

	if(dst->linearCombination())
		analytic = new AnalyticBRDF(dst->getNChannels(), AnalyticBRDF::Linear,dst->getMaterialName());
	else
		analytic = new AnalyticBRDF(dst->getNChannels(), AnalyticBRDF::Fixed,dst->getMaterialName());

	disconnect(addLobeButton, SIGNAL(clicked()), dst, SLOT(addLobe()));	
	delete dst;
	
	dst = analytic;

	dst->setBRDFAsFitted(true);
	connect(addLobeButton, SIGNAL(clicked()), dst, SLOT(addLobe()));	
	brdfOptions = new QPropertyEditorWidget(this);
	dst->setQPropertyEditorWidget(brdfOptions);

	errorSpinBox->setValue(0.);
	fit = new LMFitting(src,dst);

	QLayout *layout = this->layout();

	dynamic_cast<QVBoxLayout*>(layout)->insertWidget(2,brdfOptions);



}
*/
void 
PropertyEditorWidget::setError(double e){
	errorSpinBox->setValue(e);
}

void 
PropertyEditorWidget::setMaterial(int m){

	currentMaterial = m;

	if(actualBRDF != -1)
		brdfs[actualBRDF]->setMaterial(currentMaterial);

}

void 
PropertyEditorWidget::setActiveBRDF(bool a){

	brdfStatus = a;
	
	if(actualBRDF != -1)
		brdfs[actualBRDF]->setActiveBRDF(brdfStatus);
	else
		cleanBRDFs();


}

void 
PropertyEditorWidget::updateMaterial(){

	if(actualBRDF != -1)
		brdfs[actualBRDF]->setMaterial(currentMaterial);
}

void
PropertyEditorWidget::cleanBRDFs(){
	
	Ogre::String materialNameToClean("DisplayFittedBRDF");

	// material cleanup
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialNameToClean);

	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuParameters->setNamedConstant("fixed",(Ogre::Real)0.);
	gpuParameters->setNamedConstant("channels",(Ogre::Real)3.);
//	gpuParameters->setNamedConstant("scale",Ogre::Vector3(1.,1.,1.)); 
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
//	gpuParameters->setNamedConstant("scale",Ogre::Vector3(1.,1.,1.)); 
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
PropertyEditorWidget::optimizeAll(){
	if(!optimizing){
		if(actualBRDF != -1 && actualBRDF < brdfs.size()){

			QStringList items;
			items << tr("Linear") << tr("NonLinear");

			bool ok;
			QString item = QInputDialog::getItem(this, tr("BRDFLab: Select optimization mode"),
				tr("Mode:"), items, 0, false, &ok);

			if (!ok)
				return;

			optimizing = true;
			optimizeButton->setDisabled(true);
			disableGui();

			int i=0;
			while(i<brdfs.size()){

				comboBox->setCurrentIndex(i);
				bool forceStopped = false;

				if(!brdfs[actualBRDF]->isReadOnly()){

					if(brdfs[actualBRDF]->linearCombination()){
					
						if(item == "Linear")
							fit = new LMFitting(src,brdfs[actualBRDF]);
						else
							fit = new LMFitting(src,brdfs[actualBRDF],true);

					}else{
						fit = new LMFitting(src,brdfs[actualBRDF]);
					}
					
					optimizeButton->setText("Stop");
					fit->setErrorGUI(errorSpinBox);
					forceStopped = fit->optimize();

					delete fit;

					//brdfOptions->expandAll();
					optimizeButton->setText("Optimize Parameters");

					brdfs[actualBRDF]->setError(errorSpinBox->value());

					emit brdfModified();
				
				}

				if(forceStopped)
					i = brdfs.size();
				else
					i++;
			}

			optimizeButton->setEnabled(true);
			enableGui();
			optimizing = false;
		}
	}else{

		fit->stop();

	}
}


void
PropertyEditorWidget::disableGui(){

	addBRDFButton->setDisabled(true);
	delBRDFButton->setDisabled(true);
	addLobeButton->setDisabled(true);
	clearBRDFButton->setDisabled(true);
	comboBox->setDisabled(true);
	variantEditor->setDisabled(true);

}


void
PropertyEditorWidget::enableGui(){

	addBRDFButton->setEnabled(true);
	delBRDFButton->setEnabled(true);
	addLobeButton->setEnabled(true);
	clearBRDFButton->setEnabled(true);
	comboBox->setEnabled(true);
	variantEditor->setEnabled(true);

}