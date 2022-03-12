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

#include "BRDFLab.h"

#include <iostream>

using namespace std;


BRDFLab::BRDFLab():
	window(NULL),
	surfaceViewer(NULL),
	mainBRDF(NULL),
	fittedBRDF(NULL),
//	simulationWindow(NULL),
	pew(NULL),
	pew2(NULL),
	objectWindow(NULL),
	ogw(NULL),
	brdfModified(false)


{

	setupUi(this);

	//connect signals to slots
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));

	connect(actionXMLView, SIGNAL(triggered()), this, SLOT(xmlView()));
	connect(actionSurfaceVisualization, SIGNAL(triggered()), this, SLOT(surfaceVisualizationView()));
	connect(actionBRDFView, SIGNAL(triggered()), this, SLOT(brdfView()));
	connect(actionObjectRenderer, SIGNAL(triggered()), this, SLOT(objectRenderer()));

	//save as XML the current BRDF
	connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

	// TOOLS -> fitting
	connect(actionFitting, SIGNAL(triggered()), this, SLOT(newFitting()));
	connect(actionExportBRDFData, SIGNAL(triggered()), this, SLOT(exportBRDFData()));
	
//	connect(actionFit, SIGNAL(triggered()), this, SLOT(fit()));


	// New models to load.
	connect(actionNewMERL, SIGNAL(triggered()), this, SLOT(newMERL()));
	connect(actionNewCarPaint, SIGNAL(triggered()), this, SLOT(newCarPaint()));
	connect(actionRawData, SIGNAL(triggered()), this, SLOT(newRawData())); 
	
	connect(actionNewParametric, SIGNAL(triggered()), this, SLOT(newParametric()));
	connect(actionNewScatmech, SIGNAL(triggered()), this, SLOT(newScatmech()));
	connect(actionHeightField, SIGNAL(triggered()), this, SLOT(newHeightField()));
	connect(actionSpheres, SIGNAL(triggered()), this, SLOT(newSpheres()));
	connect(actionTriangles, SIGNAL(triggered()), this, SLOT(newTriangles()));
	connect(actionNewGrooves, SIGNAL(triggered()), this, SLOT(newGrooves()));
	
	//about
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	
	//disable actions where no model is loaded
	disableBRDFOptions();


	//dock moves... resizes the mainWindow
//	connect(propertyEditorDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(objectRenderer()));
//	connect(fittingEditorDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(objectRenderer()));
//	connect(consoleDock, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(objectRenderer()));



	// widget that will handle the different view windows.
	mainWidget = new QWidget(this);

	setCentralWidget(mainWidget);	if(pew2 != NULL)
	mainWidget->show();

	textEdit = new QTextEdit(mainWidget);
	
//	Ogre::String lobeWindowName("BRDFViewer");
//	window = new OgreWidget(mainWidget,lobeWindowName);

	
	// danger!! WINDOWS code only!


	//copy default shaders, erasing previous ones (they could not be in good shape).
	system("copy /y ..\\Data\\shaders\\default\\ ..\\Data\\shaders\\");

	objectWindow = new ObjectRenderer(mainWidget);

	connect(objectWindow, SIGNAL(setActiveBRDF(int)), this, SLOT(setActiveBRDF(int)));
	connect(objectWindow, SIGNAL(lightTypeChanged(int)),this, SLOT(lightTypeChanged(int)));

	propertyEditorDock->hide();
	
	fittingEditorDock->hide();
/*

	//initialize brdf property editor.
	pew = new PropertyBRDFEditorWidget(); 

	propertyEditorDock->setWidget(pew);
	mainBRDFOptions = pew->getQPropertyEditorWidget();
	propertyEditorDock->hide();
*/

	//initializing the fitting editor



	//initializing the console dock
	Console *console = new Console();
	console->setReadOnly(true);
	consoleDock->setWidget(console);

	consoleDock->hide();

	hideWindows();

	


//	newMERL("../Data/measured/MERL/blue-metallic-paint.binary");
	
/*

	mainBRDF = new AnalyticBRDF(3,AnalyticBRDF::Linear,"prova",ed);

	dynamic_cast<AnalyticBRDF*>(mainBRDF)->addLobe(ParametricLOBE::Lambertian);
	dynamic_cast<AnalyticBRDF*>(mainBRDF)->addLobe(ParametricLOBE::BlinnPhong);
	dynamic_cast<AnalyticBRDF*>(mainBRDF)->addLobe(ParametricLOBE::LafortuneIso);
	dynamic_cast<AnalyticBRDF*>(mainBRDF)->addLobe(ParametricLOBE::LafortuneAniso);
	dynamic_cast<AnalyticBRDF*>(mainBRDF)->addLobe(ParametricLOBE::AshikhminShirley);

	cout << "model created" << endl;

//	b->initForOptimize();

	dynamic_cast<AnalyticBRDF*>(mainBRDF)->showParameters();

	*/
	
	//consoleDock->hide();
	
	/*
	LafortuneAnisoParameters *v3 = new LafortuneAnisoParameters(this);
	m_propertyEditor->addObject(v3);
*/



//	PropertyEditorWidget *propertyEditorWidget = new PropertyEditorWidget(propertyEditorDock);
//	propertyEditorDock->show(


	//m_propertyEditor = new QPropertyEditorWidget();

/*
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(brdfs);//,200,Qt::AlignLeft);
	layout->setSpacing(1);
	layout->addWidget(lb);//,600,Qt::AlignRight);

	setLayout(layout);
	*/

//	mainBRDF = new LafortuneSpecularLobeBRDF(BRDF::All);


/*

*/
//		window->paintEvent();


/*
	cout << "begin test " << endl;

	Ogre::Vector3 v0(0,0,0);
	Ogre::Vector3 v1(0,1,0);
	Ogre::Vector3 v2(0,0,1);
	
	Ogre::Vector3 vec1(v2-v0);
	Ogre::Vector3 vec2(v1-v0);

	Ogre::Vector3 perp = v1.crossProduct(v2);
	perp.normalise();
	cout << perp  << endl;
	*/
/*

	// plane normal
	//	vec3 N = vec3(0.,0.,1.);

	Ogre::Vector3 V(0.,0.,-1.);
	V.normalise();
	Ogre::Vector3 p0(0,0.,0.);
	Ogre::Vector3 N(0.,-1.,0.);
	
	float d = 1.;
	float t;
	
	float out = V.dotProduct(N);

		if(out == 0.){
			cout << "error" << endl;
		}else{
			t = -(p0.dotProduct(N)-d)/out;
			cout  <<  Ogre::Vector3(p0+t*V).length() << endl;
		}
	
	
	// final pointX
	

*/
	//newTriangles();
//	brdfView();

//	newParametric("MainBRDF", "Linear", 3);
//	dynamic_cast<AnalyticBRDF*>(mainBRDF)->addLobe("LafortuneIso");
//	objectRenderer();
//	objectWindow->setLightType(1);

//	convertBobModel();

//downloadFile("http://graphics.cs.ucf.edu/BRDF/brdfs/green-plastic.binary");
	//downloadFile("http://qt.nokia.com/images/products/Qt_logostrap_CMYK.png");


	actionNewGrooves->setDisabled(true);

	// temp
	//menuSimulated->setDisabled(true);
	//actionNewScatmech->setDisabled(true);
	
	toolBar->hide();

	// set shaders to default
	//AnalyticBRDF::setShadersToDefault();


}


BRDFLab::~BRDFLab(){

	if(surfaceViewer != NULL){
		delete surfaceViewer;
		surfaceViewer = NULL;
	}

	



}


void 
BRDFLab::open(){

/*
QMessageBox::warning(this, tr("BRDFLab"),
			tr("Not ready yet."));
*/
			

	if(!createNewModel())
		return;


	fileName = QFileDialog::getOpenFileName(this,
     tr("Open BRDF"),"../Data/brdfs/", tr("XML files (*.xml)"));

	if(fileName == "")
		return;


	QDomDocument doc("BRDF");
	QFile file(fileName);
	
	if (!file.open(QIODevice::ReadOnly))
	 return;

	if (!doc.setContent(&file)) {
	 file.close();
	 return;
	}
	file.close();

	// print out the element names of all elements that are direct children
	// of the outermost element.
	QDomElement docElem = doc.documentElement();

	QDomNode n = docElem.firstChild();
	while(!n.isNull()) {  // BRDF file
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if(!e.isNull()) { // Root -> childs: info, material and fitting

		//	cout << "begin " << qPrintable(e.tagName()) << endl; // the node really is an element.
			if(e.tagName() == "Material"){ 

				QString name;
				QString type;
				QString lobeCombination;
				QDomElement parameters;
				int nChannels;

				//relief
				QString textureName;
			
				// grooves
				double width;
				double spacing;
				double botAngle;
				double angle;
				int groovesPerSide;
				int texelsPerGroove;

				// spheres
				int nSpheres;
				float radius;

				QDomNode i = e.firstChild();
				
				while(!i.isNull()){
					QDomElement ie = i.toElement();
					
					if(!ie.isNull()){

//						cout << "TagName: " << ie.tagName().toStdString() << endl;

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

							//spheres
							if(ie.hasAttribute("nSpheres")){ 
								nSpheres = ie.attribute("nSpheres").toInt();
							}

							if(ie.hasAttribute("radius")){ 
								radius = ie.attribute("radius").toFloat();
							}

							// grooves
							if(ie.hasAttribute("Width")){ 
								width = ie.attribute("Width").toDouble();
							}

							if(ie.hasAttribute("Spacing")){ 
								spacing = ie.attribute("Spacing").toDouble();
							}

							if(ie.hasAttribute("BotAngle")){ 
								botAngle = ie.attribute("BotAngle").toDouble();
							}

							if(ie.hasAttribute("Angle")){ 
								angle = ie.attribute("Angle").toDouble();
							}

							if(ie.hasAttribute("GroovesPerSide")){ 
								groovesPerSide = ie.attribute("GroovesPerSide").toInt();
							}

							if(ie.hasAttribute("TexelsPerGroove")){ 
								texelsPerGroove = ie.attribute("TexelsPerGroove").toInt();
							}

						}else if(ie.tagName() == "TextureName")
							textureName = ie.text();
						else{
							cout << "Warning information of tag " << ie.tagName().toStdString() << " not parsed." << endl;
						}



					}

					i = i.nextSibling();
				}

/*
				cout << "Name: " << name.toStdString() << endl;
				cout << "Type: " << type.toStdString() << endl;
				cout << "LobeCombination: " << lobeCombination.toStdString() << endl;
				cout << "nChannels: " << nChannels << endl;
*/

				if(type == "Analytic"){
					newParametric(name,lobeCombination,nChannels);
					// set lobes and parameters
					dynamic_cast<AnalyticBRDF*>(mainBRDF)->loadXML(parameters);
				}else if( type == "Relief"){
					newHeightField(textureName);
				}else if( type == "Spheres"){
					newSpheres(nSpheres,radius);
				}else if( type == "Grooves"){
					newGrooves(width, spacing, botAngle, angle, groovesPerSide, texelsPerGroove);
				}else if( type == "DoubleInteraction"){
					newScatmech("Double_Interaction_BRDF_Model");
					dynamic_cast<DoubleInteractionBRDF*>(mainBRDF)->loadXML(parameters);
				}else if( type == "SubsurfaceParticle"){
					newScatmech("Subsurface_Particle_BRDF_Model");
					dynamic_cast<SubsurfaceParticleBRDF*>(mainBRDF)->loadXML(parameters);
				}else if( type == "MERLBRDFDatabase"){
					newMERL(name);
				}else if( type == "CarPaintBRDFDatabase"){
					newCarPaint(name);
				}else{
					QMessageBox::warning(this, tr("BRDFLab"),
						tr("Material type not recognized."));
					return;
				}
			}

			// fitting.

			if(e.tagName() == "Fitting"){ 

				newFitting(&e);
/*
				QDomNode fi = e.firstChild();
				double error = e.attribute("error").toDouble();
					
				if(!fi.isNull()){
					QDomElement fee = fi.toElement();
	
					if(!fee.isNull()){
						if(fee.tagName() == "Material"){
						
							QString name;
							QString type;
							QString lobeCombination;
							QDomElement parameters;
							int nChannels;

							QDomNode i = fee.firstChild();
							
							while(!i.isNull()){
								QDomElement ie = i.toElement();
								
								if(!ie.isNull()){

			//						cout << "TagName: " << ie.tagName().toStdString() << endl;

									if(ie.tagName() == "Name")
										name = ie.text();
									else if( ie.tagName() == "Type")
										type = ie.text();
									else if( ie.tagName() == "LobeCombination")
										lobeCombination = ie.text();
									else if( ie.tagName() == "Parameters"){
										nChannels = ie.attribute("nChannels").toInt();
										parameters = ie;
									}else{
										cout << "Warning information of tag " << ie.tagName().toStdString() << " not parsed." << endl;
									}



								}

								i = i.nextSibling();
							}

							if(type == "Analytic"){
								newFitting(name,lobeCombination,nChannels);
								// set lobes and parameters
								dynamic_cast<AnalyticBRDF*>(fittedBRDF)->loadXML(parameters);
								pew2->setError(error);
							}else{
								QMessageBox::warning(this, tr("BRDFLab"),
									tr("Material type not recognized."));
								return;
							}


						}
					}
				}*/
			}

//			cout << qPrintable(e.tagName()) << " parsed" << endl << endl; // the node really is an element.
		}
		n = n.nextSibling();
	}

/*
	//enable save option
	enableBRDFOptions();


	xmlView();
		

	//todo: parse and create the mainBRDF and possible fittings that the file contains!
*/

	// after loading the XML, set that the BRDF has not been modified
	brdfModified = false;

}

/**

This method displays the XML file

*/
void
BRDFLab::xmlView(){

	hideWindows();

	// backup the current fileName
	QString old(fileName);
	fileName = QString("temp.xml");

	// save the current BRDF to disk
	writeXML();

	// restore
	fileName = old;

	QFile input("temp.xml");
	if (input.open(QFile::ReadOnly)) {

		QTextStream out(&input);
	    QString text = out.readAll();

		textEdit->setPlainText(text);

		highlighter = new XmlHighlighter(textEdit);
		textEdit->setReadOnly(true);
		textEdit->resize(mainWidget->size());

		textEdit->show();



	}

}

/**

This method allows the lobe visualization of the actual BRDF

*/
void
BRDFLab::brdfView(){

	hideWindows();

	if(window == NULL){
		window = new BRDFViewer(mainWidget,this);
		window->initSliders();

		if(mainBRDF != NULL && dynamic_cast<GPUSimulationBRDF*>(mainBRDF) != NULL)
			connect(mainBRDF,SIGNAL(updateBRDFViewer()),this,SLOT(updateBRDFViewer()));

		if(mainBRDF != NULL && dynamic_cast<AnalyticBRDF*>(mainBRDF) != NULL)
			connect(mainBRDF,SIGNAL(updateImportanceSampleDirections()),window,SLOT(updateImportanceSampleDirections()));

	}


	if(mainBRDF != NULL){
		mainBRDF->setActiveBRDF(true);
		mainBRDF->setMaterial(-1);
	}

	if(pew2 != NULL){
		pew2->setActiveBRDF(true);
		pew2->setMaterial(-1);
	}

	window->resize(mainWidget->size());




	window->show();
	//if(dynamic_cast<AnalyticBRDF*>(mainBRDF) != NULL)
//		window->setpri		->setPrimaryMaterial("AnalyticBRDFMaterial"); // to do here. Set the material to brdfview





	//send the BRDF to visualize

/*	if(window == NULL){
		window = new OgreWidget(mainWidget,lobeWindowName);
		window->paintEvent();
	}*/
	//setCentralWidget(window);




	/*
	OgreWidget *og = new OgreWidget(this,RenderWindowName);
	og->paintEvent();
	*/


}

void
BRDFLab::surfaceVisualizationView(){

	hideWindows();

	if(surfaceViewer == NULL){
		surfaceViewer = new OgreSurfaceViewer(mainWidget);
		surfaceViewer->paintEvent();
	}
	
	
	surfaceViewer->resize(mainWidget->size());

	if(mainBRDF != NULL){
		mainBRDF->setActiveBRDF(true);
		mainBRDF->setMaterial(-2);
	}


	surfaceViewer->show();

}

/**

This method allows the rendering of generic objects by using the actual BRDF

*/
void 
BRDFLab::objectRenderer(){

	hideWindows();

	//

	//mainBRDF = new AshikhminShirleyBRDF(BRDF::All);
	
	//dynamic_cast<AnalyticBRDF*>(mainBRDF)->initForOptimize();

//	if(dynamic_cast<MERLStructureBRDF*>(mainBRDF) != NULL)
//		objectWindow->setMaterial("PointLight3DTextureBRDF");


	if(dynamic_cast<AnalyticBRDF*>(mainBRDF) != NULL)
		objectWindow->setOriginalMaterialType(0);
	else if(dynamic_cast<MeasuredBRDF*>(mainBRDF) != NULL)
		objectWindow->setOriginalMaterialType(1);
	else if(dynamic_cast<SimulatedBRDF*>(mainBRDF) != NULL)
		objectWindow->setOriginalMaterialType(2);


	// todo, complet this

	//todo temporal
	if(dynamic_cast<SimulatedBRDF*>(mainBRDF) != NULL)
		dynamic_cast<SimulatedBRDF*>(mainBRDF)->set3DTexture();

	// if there are not fitting data, always render original
	if(pew2 == NULL)
		objectWindow->setOriginalMaterial();


	lightTypeChanged(objectWindow->getLightType());

	objectWindow->show();



	this->resize(this->size()+QSize(1,1));
}

void 
BRDFLab::newRawData(QString modelName)
{
	if(modelName == ""){
		if(!createNewModel())
			return;

		//select the file to open
		modelName = QFileDialog::getOpenFileName(this,
			tr("Open Raw Data"),"../Data/measured/Raw/", tr("Text files (*.txt)"));

		if(modelName == "") // if the user push esc or cancel...
			return;
		
		QDir path(".");

		// store as a relative path from the BRDFLab executable
		modelName = path.relativeFilePath(modelName);


	}else{
/*
		//check that the file exists!
		QFile testIfExists(modelName);

		bool exists = testIfExists.open(QIODevice::ReadOnly);

		if(!exists){

			int pos = modelName.lastIndexOf("/");

			// get the file name
			QString name(modelName);

			name.remove(0,pos+1);

			// URL of the BRDF Database
			QString url("http://graphics.cs.ucf.edu/BRDF/brdfs/");
			url.append(name);

			bool ok = downloadFile(url,modelName);

			if(!ok)
				return;

		}
*/
	
	}

	// create the BRDF
	mainBRDF = new RawDataBRDF(3,modelName);

	//read data
	(dynamic_cast<RawDataBRDF*>(mainBRDF))->readMeasuredData();

	//display the BRDF Lobe
	brdfView();
	window->initSliders();


	enableBRDFOptions();
}



void 
BRDFLab::newMERL(QString modelName){

	if(modelName == ""){
		if(!createNewModel())
			return;

		//select the file to open
		modelName = QFileDialog::getOpenFileName(this,
			tr("Open Measured Data"),"../Data/measured/MERL/", tr("Binary files (*.binary)"));

		if(modelName == "") // if the user push esc or cancel...
			return;
		
		QDir path(".");

		// store as a relative path from the BRDFLab executable
		modelName = path.relativeFilePath(modelName);


	}else{
		//check that the file exists!
		QFile testIfExists(modelName);

		bool exists = testIfExists.open(QIODevice::ReadOnly);

		if(!exists){

/*
			int pos = modelName.lastIndexOf("/");
			
//			cout << modelName.toStdString() << endl;

			QString path(modelName);

			// get the file path
			path.remove(pos+1,path.size());

//			cout << path.toStdString() << endl;

			// get the file name
			modelName.remove(0,pos+1);

//			cout << modelName.toStdString() << endl;

			QString message("Measured Data not found.\n\nThe Measured BRDF data file is required to open this BRDF.\n\nPlease download the ");
			message.append(modelName);
			message.append(" BRDF data file from http://graphics.cs.ucf.edu/BRDF/ and set it to the ");
			message.append(path);
			message.append(" folder.");

			QMessageBox::warning(this, tr("BRDFLab"),	
				message);

			return;
*/

			int pos = modelName.lastIndexOf("/");

			// get the file name
			QString name(modelName);

			name.remove(0,pos+1);

			// URL of the BRDF Database
			QString url("http://graphics.cs.ucf.edu/BRDF/brdfs/");
			url.append(name);

			bool ok = downloadFile(url,modelName);

			if(!ok)
				return;

		}

	
	}

	// create the BRDF
	mainBRDF = new MERLBRDFDatabase(modelName);

	//read data
	(dynamic_cast<MeasuredBRDF*>(mainBRDF))->readMeasuredData();

	//display the BRDF Lobe
	brdfView();
	window->initSliders();


	enableBRDFOptions();
}


void
BRDFLab::newCarPaint(QString modelName){

	if(modelName == ""){
		if(!createNewModel())
			return;

		//select the file to open
		modelName = QFileDialog::getOpenFileName(this,
			tr("Open Measured Data"),"../Data/measured/CarPaint/", tr("Binary files (*.*)"));

		if(modelName == "") // if the user push esc or cancel...
			return;
		
		QDir path(".");

		// store as a relative path from the BRDFLab executable
		modelName = path.relativeFilePath(modelName);


	}else{

		//check that the file exists!
		QFile testIfExists(modelName);

		bool exists = testIfExists.open(QIODevice::ReadOnly);

		if(!exists){

			int pos = modelName.lastIndexOf("/");

			// get the file name
			QString name(modelName);

			name.remove(0,pos+1);

			// URL of the BRDF Database
			QString url("http://www.mpi-inf.mpg.de/~guenther/carpaint/data/");
			url.append(name);
			url.append(".zip");

			QString saveName(modelName);
			saveName.append(".zip");

			bool ok = downloadFile(url,saveName);

			cout << url.toStdString() << " " <<saveName.toStdString() << endl;

			if(!ok)
				return;

		

			//This code loads a zip file, and stores in disk the file "name", located inside the zip file!
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(saveName.toStdString(),"Zip");

			Ogre::DataStreamPtr data = Ogre::ResourceGroupManager::getSingleton().openResource(name.toStdString());

			char *info = new char[data->size()];

			data->read(info,data->size());

			QFile file(modelName);
			
			if (!file.open(QIODevice::WriteOnly))
				return;

			QDataStream out(&file);   // we will serialize the data into the file
			cout << out.writeRawData(info,data->size()) << endl;

			file.close();

			delete []info;

			Ogre::ResourceGroupManager::getSingleton().removeResourceLocation("../Data/measured/CarPaint");
		}
	}



/*
	QMessageBox::warning(this, tr("BRDFLab"),
			tr("Not ready yet."));
*/
/*
	if(!createNewModel())
		return;

	//select the file to open
	QString modelName = QFileDialog::getOpenFileName(this,
		tr("Open Measured Data"),"../Data/measured/CarPaint/", tr("Binary files (*.*)"));

	if(modelName == "") // if the user push esc or cancel...
		return;
*/
	// create the BRDF
	mainBRDF = new CarPaintBRDFDatabase(3,modelName);

	//read data
	(dynamic_cast<MeasuredBRDF*>(mainBRDF))->readMeasuredData();


	//display the BRDF Lobe
	brdfView();
	window->initSliders();


	enableBRDFOptions();

}


bool 
BRDFLab::saveAs(){

	fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
               "../Data/brdfs",
               tr("BRDF files (*.xml)"));

	cout << fileName.toStdString() << endl;

	if(fileName == "")
		return false;
	else{
		save();
		return true;
	}

}


void
BRDFLab::save(){

	if(fileName == ""){
		if(saveAs())
			save();
	}else{

		if(mainBRDF != NULL){
			writeXML();
			brdfModified = false;
		}else
			QMessageBox::warning(this, tr("BRDFLab"),
			tr("First a BRDF has to be created."));

	}

}

void
BRDFLab::writeXML(){

	QDomDocument doc( "BRDF" );
	QDomElement root = doc.createElement( "BRDF" );
	doc.appendChild( root );

	writeXMLProgramInformation(&doc, &root);

	mainBRDF->writeXML(&doc, &root);

	//TODO: write the fittings too ;)
	//lmfit->writeXML(&doc,&root);
	if(pew2 != NULL)
		pew2->writeXML(&doc,&root);
	
	QFile file(fileName);
	if( !file.open( QIODevice::WriteOnly ))
		cout << "ERROR, the file can not be opened."<< endl;

	QTextStream ts( &file );
	ts << doc.toString();

	file.close();
}


void
BRDFLab::writeXMLProgramInformation(QDomDocument *doc, QDomElement *parent){

	QDomElement info = doc->createElement( "Info" );
	QDomElement program = doc->createElement( "Program");

	program.appendChild(doc->createTextNode("BRDFLab"));

	
	QDomElement version = doc->createElement( "Version");
	version.appendChild(doc->createTextNode("0.1"));

	QDomElement url = doc->createElement( "URL");
	url.appendChild(doc->createTextNode("http://ima.udg.edu/~afores/BRDFLab"));

	QDomElement contact = doc->createElement( "Contact");
	contact.appendChild(doc->createTextNode("afores@ima.udg.edu"));

	QDomElement data = doc->createElement( "Data");
	data.appendChild(doc->createTextNode("17/03/2009"));
	
	info.appendChild(program);
	info.appendChild(version);
	info.appendChild(url);
	info.appendChild(contact);
	info.appendChild(data);

	parent->appendChild(info);
}


/*
void
BRDFLab::fit(){

//	ana = new LafortuneSpecularLobeBRDF(BRDF::All);
	

	// todo mainBRDF and fittedBRDF must exist!

	mainBRDF->getNChannels();
	fittedBRDF->getNChannels();
	lmfit = new LMFitting(mainBRDF,fittedBRDF);

	lmfit->optimize();
	
	this->resize(this->size());
	//save();

	//fileName = "test.xml";
	
	//xmlView();

}
*/

/**

Resize the widgets inside the mainWidget (the one that is on setCentralWidget of BRDFLab QMainWindow)

*/
void 
BRDFLab::resizeEvent( QResizeEvent * event ){
	
//	mainWidget->resize(this->size());
	if(window != NULL)
		window->resize(mainWidget->size());
/*	if(simulationWindow != NULL)
		simulationWindow->resize(mainWidget->size());*/

	if(surfaceViewer != NULL)
		surfaceViewer->resize(mainWidget->size());
	
	if(objectWindow != NULL)
		objectWindow->resize(mainWidget->size());

	textEdit->resize(mainWidget->size());
}

void
BRDFLab::newParametric(QString name, QString combinationType, int nChannels ){

	if(name == ""){
		if(!createNewModel())
			return;

		//cout << "new parametric created " << endl;

		// ask for a name for the new BRDF
		bool ok;
		/*
		name = QInputDialog::getText(this, tr("BRDFLab"),
											  tr("Parametric model name:"), QLineEdit::Normal,
											  "TestBRDF", &ok);
		if (!ok || name.isEmpty())
			return;
		*/
		name = QString("MainBRDF");

//		cout << "valid: " << name.toStdString() << endl;

		
		// ask for the number of channels

		nChannels = QInputDialog::getInteger(this,
				tr("BRDFLab"), tr("Number of channels"),
				3, 1, 2147483647, 1, &ok);
				// default, min, max, step, 

		if (!ok)
			return;
			
		//cout << "nChannels: " << nChannels << endl;


		// ask for Linear or Fixed
		 QStringList items;
		 items << tr("Linear") << tr("Fixed");

		 combinationType = QInputDialog::getItem(this, tr("BRDFLab"),
											  tr("Combination:"), items, 0, false, &ok);

		 if (!ok)
			 return;

		 //cout << "Mode triat: " << combinationType.toStdString() << endl;

	}

	 // create analytic BRDF
	if(combinationType == "Linear")
		mainBRDF = new AnalyticBRDF(nChannels, AnalyticBRDF::Linear,name);
	else
		mainBRDF = new AnalyticBRDF(nChannels, AnalyticBRDF::Fixed,name);

	//show property editor
	pew = new PropertyBRDFEditorWidget(mainBRDF);
	propertyEditorDock->setWidget(pew);
	propertyEditorDock->show();

	mainBRDF->setActiveBRDF(true);
	dynamic_cast<AnalyticBRDF*>(mainBRDF)->setShadersToDefault();
	
	connect(mainBRDF,SIGNAL(brdfModified()), this, SLOT(setBRDFModified()));
	brdfModified = true;

/*
	dynamic_cast<AnalyticBRDF*>(mainBRDF)->setQPropertyEditorWidget(mainBRDFOptions);

	pew->setMainBRDF(dynamic_cast<AnalyticBRDF*>(mainBRDF));
	propertyEditorDock->show();
	
	window->initSliders();
	brdfView();

	cout << "Analytic BRDF created" << endl;

	//initialize brdf property editor.
	pew = new PropertyEditorWidget();

	propertyEditorDock->setWidget(pew);
	mainBRDFOptions = pew->getQPropertyEditorWidget();
	propertyEditorDock->hide();*/


	enableBRDFOptions();

	brdfView();
	window->initSliders();

	

}


void
BRDFLab::newFitting(QDomElement *e){

	if(mainBRDF == NULL)
		return;

	if(pew2 == NULL){ // create the fitting widget, only the first time or where it does not exist.
		pew2 = new PropertyEditorWidget(mainBRDF);
		fittingEditorDock->setWidget(pew2);
		fittingEditorDock->show();
		objectWindow->activeFittedRadioButton(true);
		actionOptimize_All->setEnabled(true);
		connect(pew2,SIGNAL(brdfModified()),this,SLOT(setBRDFModified()));
		connect(actionOptimize_All, SIGNAL(triggered()), this, SLOT(optimizeAll()));
	}


	if(e == NULL){ // user interaction
		if(mainBRDF == NULL){
			QMessageBox::warning(this, tr("BRDFLab"),
				tr("First a BRDF has to be created."));
			return;
		}

		// create BRDF

		pew2->addBRDF();
		//pew2->setActiveBRDF(true);

	}else{ // opening XML file, BRDF loaded from file

		pew2->addBRDF(e);
		//pew2->setActiveBRDF(true);
	}

	if(mainBRDF != NULL){

		if(mainBRDF->getMaterialToUpdate() == -1)
			pew2->setActiveBRDF(true);
	}

/*

	if(name == ""){	

		if(mainBRDF == NULL){
			QMessageBox::warning(this, tr("BRDFLab"),
				tr("First a BRDF has to be created."));
			return;
		}

		// ask for a name for the new BRDF
		bool ok;
		name = QInputDialog::getText(this, tr("BRDFLab"),
											  tr("Parametric model name:"), QLineEdit::Normal,
											  "FitTestBRDF", &ok);
		if (!ok || name.isEmpty())
			return;
			
		cout << "valid: " << name.toStdString() << endl;


		// todo by default we will fit all the channels of the original brdf

		nChannels = mainBRDF->getNChannels();
		cout << "nChannels: " << nChannels << endl;


		// ask for Linear or Fixed
		 QStringList items;
		 items << tr("Linear") << tr("Fixed");

		 combinationType = QInputDialog::getItem(this, tr("BRDFLab"),
											  tr("Combination:"), items, 0, false, &ok);

		 if (!ok)
			 return;

		 cout << "Mode triat: " << combinationType.toStdString() << endl;

	}

	 // create analytic BRDF
	if(combinationType == "Linear")
		fittedBRDF = new AnalyticBRDF(nChannels, AnalyticBRDF::Linear,name);
	else
		fittedBRDF = new AnalyticBRDF(nChannels, AnalyticBRDF::Fixed,name);


	fittedBRDF->setBRDFAsFitted(true);

	pew2 = new PropertyEditorWidget(mainBRDF, fittedBRDF);
	fittingEditorDock->setWidget(pew2);
	fittingEditorDock->show();

*/
	this->resize(this->size()+QSize(1,1));

//	pew2->setMainBRDF(dynamic_cast<AnalyticBRDF*>(fittedBRDF));
	

	cout << "Analytic BRDF created" << endl;


}


void
BRDFLab::hideWindows(){

	textEdit->hide();

	if(window != NULL)
		window->hide();

	if(objectWindow != NULL)
		objectWindow->hide();

	if(surfaceViewer != NULL)
		surfaceViewer->hide();
}


/**

This method is used when a new BRDF model is created in order to store previous BRDF used, discart or cancel the process.

*/
bool
BRDFLab::createNewModel(){

	if(mainBRDF != NULL){

		// if the user is performing an optimization, force him to stop it first!

		if(pew2 != NULL && pew2->isOptimizing()){

			QMessageBox::warning(this, tr("BRDF Lab"),
				tr("Please, stop the optimization process\n before creating a new BRDF."));

			return false;

		}

		// if the BRDF has been modified, ask the user if he wants to save the changes
		if(brdfModified){
			// ask to the user if he wants to save the actual pattern first
			int ret = QMessageBox::warning(this, tr("BRDF Lab"),
					   tr("There exists a BRDF created.\n"
						  "Do you want to save your changes?"),
					   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
					   QMessageBox::Save);

			switch(ret){
				case QMessageBox::Save:
					// save the existing BRDF to a file
					save();
				case QMessageBox::Discard:
					cleanEnvironment();
					return true;
					break;

				case QMessageBox::Cancel:
					return false;
					break;
				default:
					return false;
					break;
			}
		}else{
			cleanEnvironment();
			return true;
		}


	}else{
		return true;
	}
}



/**

As to perform those actions, a BRDF model has to be loaded. This method is called when a BRDF is created.

*/
void 
BRDFLab::enableBRDFOptions(){

	// save
	actionSave->setEnabled(true);
	actionSaveAs->setEnabled(true);

	//views
	actionXMLView->setEnabled(true);
	actionBRDFView->setEnabled(true);
	actionObjectRenderer->setEnabled(true);

	if(dynamic_cast<GPUSimulationBRDF*>(mainBRDF) != NULL)
		actionSurfaceVisualization->setEnabled(true);

	if(dynamic_cast<SimulatedBRDF*>(mainBRDF) != NULL)
		actionExportBRDFData->setEnabled(true);
	

	// tools
	actionFitting->setEnabled(true);
	
}


/**

As to perform those actions a BRDF model is needed, when no BRDF model is present on the system the actions are disabled.

*/
void 
BRDFLab::disableBRDFOptions(){


	// save
	actionSave->setDisabled(true);
	actionSaveAs->setDisabled(true);

	//views
	actionXMLView->setDisabled(true);
	actionBRDFView->setDisabled(true);
	actionObjectRenderer->setDisabled(true);

	// simulated features
	actionSurfaceVisualization->setDisabled(true);
	actionExportBRDFData->setDisabled(true);

	// tools
	actionFitting->setDisabled(true);
	actionOptimize_All->setDisabled(true);
}




void 
BRDFLab::newScatmech(QString model){

	if(model == "" ){
		if(!createNewModel())
			return;

		// ask for the Scatmech Model
		 QStringList items;
		 items 
	//		 << tr("RCW_BRDF_Model")
	//		 << tr("Two_Source_BRDF_Model")
	//		 << tr("First_Diffuse_BRDF_Model")
	//		 << tr("Focussed_Beam_Instrument_BRDF_Model")
	//		 << tr("Finite_Aperture_Instrument_BRDF_Model")
	//		 << tr("Rayleigh_Instrument_BRDF_Model")
	//		 << tr("Axisymmetric_Particle_BRDF_Model")
	//		 << tr("Bobbert_Vlieger_BRDF_Model")
			 << tr("Subsurface_Particle_BRDF_Model")
			 << tr("Double_Interaction_BRDF_Model")
	//		 << tr("OneLayer_BRDF_Model")
	//		 << tr("Rayleigh_Defect_BRDF_Model")
	//		 << tr("Diffuse_Subsurface_BRDF_Model")
	//		 << tr("Subsurface_Facet_BRDF_Model")
	//		 << tr("Shadowed_Facet_BRDF_Model")
	//		 << tr("Uncorrelated_Roughness_Stack_BRDF_Model")
	//		 << tr("Correlated_Roughness_Stack_BRDF_Model")
	//		 << tr("Two_Face_BRDF_Model")
	//		 << tr("Correlated_Roughness_BRDF_Model")
	//		 << tr("Microroughness_BRDF_Model")
			 ;

		 bool ok;
		model = QInputDialog::getItem(this, tr("BRDFLab"),
											  tr("Model:"), items, 0, false, &ok);

		 if (!ok)
			 return;
	}

	// create the desired Scatmech Model
	 if(model == "RCW_BRDF_Model"){
	 }else if(model == "Two_Source_BRDF_Model"){
	 }else if(model == "First_Diffuse_BRDF_Model"){
	 }else if(model == "Focussed_Beam_Instrument_BRDF_Model"){
	 }else if(model == "Finite_Aperture_Instrument_BRDF_Model"){
	 }else if(model == "Rayleigh_Instrument_BRDF_Model"){
	 }else if(model == "Axisymmetric_Particle_BRDF_Model"){
	 }else if(model == "Bobbert_Vlieger_BRDF_Model"){
	 }else if(model == "Subsurface_Particle_BRDF_Model"){
		mainBRDF = new SubsurfaceParticleBRDF();
	 }else if(model == "Double_Interaction_BRDF_Model"){
		 mainBRDF = new DoubleInteractionBRDF();
	 }else if(model == "OneLayer_BRDF_Model"){
	 }else if(model == "Rayleigh_Defect_BRDF_Model"){
	 }else if(model == "Diffuse_Subsurface_BRDF_Model"){
	 }else if(model == "Subsurface_Facet_BRDF_Model"){
	 }else if(model == "Shadowed_Facet_BRDF_Model"){
	 }else if(model == "Uncorrelated_Roughness_Stack_BRDF_Model"){
	 }else if(model == "Correlated_Roughness_Stack_BRDF_Model"){
	 }else if(model == "Two_Face_BRDF_Model"){
	 }else if(model == "Correlated_Roughness_BRDF_Model"){
	 }else if(model == "Microroughness_BRDF_Model"){

	 }else{
		QMessageBox::warning(this,tr("BRDFLab"),tr("Model not implemented"));
		return;
	 }

	//show property editor
	pew = new PropertyBRDFEditorWidget(mainBRDF);
	propertyEditorDock->setWidget(pew);
	propertyEditorDock->show();

	//display the BRDF Lobe
	brdfView();
	window->initSliders();

	enableBRDFOptions();

	connect(mainBRDF,SIGNAL(brdfModified()), this, SLOT(setBRDFModified()));
	brdfModified = true;
	

}

void
BRDFLab::newHeightField(QString texName){

	if(texName == ""){
		if(!createNewModel())
			return;
		
		texName = QFileDialog::getOpenFileName(this,
			tr("Open texture"),"../Data/textures/relief", tr("Texture files (*.*)"));
		
		if(texName == "")
			return;

		int slashPosition = 0;

		// search for the last '/'
		for(int i=0; i<texName.size(); i++){
			if(texName[i] == '/')
				slashPosition = i+1;
		}

		texName.remove(0,slashPosition);

		cout << texName.toStdString() << endl;
	}

//	mainBRDF = new ReliefBRDF(texName);
	mainBRDF = new ReliefBRDF(texName);



/*	OgreSurfaceViewer *renderWindow = new OgreSurfaceViewer(NULL);

	renderWindow->paintEvent();

	renderWindow->update();
	renderWindow->show();*/

	//show property editor
	pew = new PropertyBRDFEditorWidget(mainBRDF);
	propertyEditorDock->setWidget(pew);
	propertyEditorDock->show();
	
	mainWidget->show();


	surfaceVisualizationView();
	updateSurfaceViewer(0);
	

/*
	brdfView();
	window->initSliders();
*/
	connect(mainBRDF,SIGNAL(updateSurfaceViewer(int)),this,SLOT(updateSurfaceViewer(int)));
	enableBRDFOptions();
}


void
BRDFLab::newSpheres(int nSpheres, float radius){

	if(!createNewModel())
		return;

	if(nSpheres != 0)
		mainBRDF = new SpheresBRDF(nSpheres,radius);
	else
		mainBRDF = new SpheresBRDF();

	//show property editor
	pew = new PropertyBRDFEditorWidget(mainBRDF);
	propertyEditorDock->setWidget(pew);
	propertyEditorDock->show();
	
	mainWidget->show();

	surfaceVisualizationView();
	updateSurfaceViewer(1);
	

	connect(mainBRDF,SIGNAL(updateSurfaceViewer(int)),this,SLOT(updateSurfaceViewer(int)));
	enableBRDFOptions();

}


//TODO: add options to the XML File reading for triangles!
void 
BRDFLab::newTriangles(){

	if(!createNewModel())
		return;

	mainBRDF = new TrianglesBRDF();

	//show property editor
	pew = new PropertyBRDFEditorWidget(mainBRDF);
	propertyEditorDock->setWidget(pew);
	propertyEditorDock->show();

	mainWidget->show();
	
	surfaceVisualizationView();
	updateSurfaceViewer(2);

	connect(mainBRDF,SIGNAL(updateSurfaceViewer(int)),this,SLOT(updateSurfaceViewer(int)));
	enableBRDFOptions();
}


void 
BRDFLab::cleanEnvironment(){

	hideWindows();
	disableBRDFOptions();

	if(objectWindow != NULL){
		objectWindow->activeFittedRadioButton(false);
//		objectWindow->clickOriginalRadioButton();
		objectWindow->setOriginalMaterialType(-1);
	}

	fileName = QString("");

	// hide docks
	propertyEditorDock->hide();
	fittingEditorDock->hide();

//	setShadersToDefault();

	if(mainBRDF != NULL && window != NULL && dynamic_cast<AnalyticBRDF*>(mainBRDF) != NULL)
		disconnect(mainBRDF,SIGNAL(updateImportanceSampleDirections()),window,SLOT(updateImportanceSampleDirections()));


	// clean main BRDF
	if(mainBRDF != NULL){
		disconnect(mainBRDF,SIGNAL(brdfModified()),this,SLOT(setBRDFModified()));
		delete mainBRDF;
		mainBRDF = NULL;
	}

	if(pew != NULL){
		delete pew;
		pew = NULL;
	}

	// clean fitting BRDF 
	if(fittedBRDF != NULL){
		delete fittedBRDF;
		fittedBRDF = NULL;			
	}

	if(pew2 != NULL){
		disconnect(pew2,SIGNAL(brdfModified()), 0, 0);
		disconnect(actionOptimize_All, SIGNAL(triggered()), 0, 0);
		delete pew2;
		pew2 = NULL;
	}

	// close windows 

	if(surfaceViewer != NULL){
		//delete surfaceViewer;
		//surfaceViewer = NULL;
		surfaceViewer->hide();

		surfaceViewer->cleanDisplay();
	}

	// delete BRDF viewer
	if(window != NULL){
		delete window;
		window = NULL;
	}

	brdfModified = false;


	this->resize(this->size()+QSize(1,1));

	// recompile AnalyticBRDF and FittedBRDF
/*
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("AnalyticBRDF");

	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("fixed",(Ogre::Real)0.);
	gpuParameters->setNamedConstant("channels",(Ogre::Real)3.);
	gpuParameters->setNamedConstant("scale",Ogre::Vector3(1.,1.,1.)); 
	gpuParameters->setNamedConstant("lobes",(Ogre::Real)1.);

	float *vsParams = new float[5];
	vsParams[0] = 0.;
	vsParams[1] = 0.;
	vsParams[2] = 0.;
	vsParams[3] = 0.;
	vsParams[4] = 1.;

	gpuParameters->setNamedConstant("params",vsParams,5,1);

	*/
/*
	mat = Ogre::MaterialManager::getSingleton().getByName("FittedBRDF");
	
	gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("fixed",(Ogre::Real)0.);
	gpuParameters->setNamedConstant("channels",(Ogre::Real)3.);
	gpuParameters->setNamedConstant("scale",Ogre::Vector3(1.,1.,1.)); 
	gpuParameters->setNamedConstant("lobes",(Ogre::Real)1.);
	gpuParameters->setNamedConstant("params",vsParams,5,1);
*/
//	delete []vsParams;



	/*
	// TODO: for now this window is always open.
	if(objectWindow != NULL){
		delete objectWindow;
		objectWindow = NULL;
	}
	*/

	// delete simulation window
/*
	// not here now
	if(simulationWindow != NULL){
		delete simulationWindow;
		simulationWindow = NULL;
	}
*/
	


}



void 
BRDFLab::newGrooves(double width, double spacing, double botAngle, double angle, int groovesPerSide, int texelsPerGroove){

	if(!createNewModel())
		return;

	mainBRDF = new GroovesBRDF();

	if(width == 0.){
		dynamic_cast<GroovesBRDF*>(mainBRDF)->updateRelief();
	}else{
		dynamic_cast<GroovesBRDF*>(mainBRDF)->updateRelief(width, spacing, botAngle, angle, groovesPerSide, texelsPerGroove);
	}

	pew = new PropertyBRDFEditorWidget(mainBRDF);
	propertyEditorDock->setWidget(pew);
	propertyEditorDock->show();


	surfaceVisualizationView();
	updateSurfaceViewer(0);
	
	enableBRDFOptions();

/*
	mainBRDF = new SpheresBRDF(nSpheres,radius);

	mainWidget->show();

	surfaceVisualizationView();
	surfaceViewer->displaySpheres(dynamic_cast<SpheresBRDF*>(mainBRDF));

	enableBRDFOptions();
*/
}


/*

This is the first version of export tool. 

This method will save in a easily and disk consuming way the full BRDF with user specified parameters.

Hints:
- use the lobe Quality to adjust the quality (more quality more data, it's easy).
- select Export BRDF data
- select the phi subdivision
- select the theta subdivision
(theta subdivisions*phi subdivisions will determine the number of incident directions)
- incoming directions * lobeQuality slider (defining the outgoing resolution) will define the number of directions of the BRDF
- set the fileName
- wait for a few seconds until the BRDF computation is finished.


File structure:
- number of measurements (1xint)
- number of phi divisions (1xint)
- numbre of theta divisions (1xint)

for each measurement:
- incoming direction (3xfloats)
- outgoing direction (3xfloats)
- intensity (1xfloat)

This is all.

*/
void 
BRDFLab::exportBRDFData(){

	//is BRDFViewer open to capture a good resolution?
	if(window==NULL)
		QMessageBox::information(this, tr("BRDFLab"),
			tr("Set BRDF View and adjust the Lobe Quality parameter to define the outgoing resolution before data exporting."));

	
	// ask fileName
	QString exportFileName = QFileDialog::getSaveFileName(this,tr("Export BRDF Data name"),
		tr("../Data/brdfs/exported/"), tr("BRDF Data files (*.brdf)"));

	if(exportFileName == "")
		return;

	// obtain BRDF information

	// ask the user the incident resolution of the simulation
	bool ok;

	// ask for phi subdivisions
	int phiDivision = QInputDialog::getInteger(NULL,
			QObject::tr("BRDFLab"), QObject::tr("Number of phi divisions"),
			1, 1, 360, 1, &ok);
			// default, min, max, step, 

	if (!ok)
		return;

	// ask for theta subdivisions
	int thetaDivision = QInputDialog::getInteger(NULL,
			QObject::tr("BRDFLab"), QObject::tr("Number of theta divisions"),
			16, 1, 360, 1, &ok);
			// default, min, max, step, 

	if (!ok)
		return;



	// generate directions
	std::vector<Ogre::Vector3> u; // incident direction	
	std::vector<Ogre::Vector3> v; // outgoing direction

	double *y;
	int ySize;

	dynamic_cast<SimulatedBRDF*>(mainBRDF)->generateFittingInformation(thetaDivision, phiDivision, &u, &v, y, ySize);


	QFile file(exportFileName);

	if (!file.open(QIODevice::WriteOnly))
         return;

	QDataStream out(&file);

	//store number of directions
	out << ySize; 

	//store the number of phi divisions
	out << phiDivision;

	//store the number of theta divisions
	out << thetaDivision;

	for(int i=0; i<ySize; i++){
		
		//store incoming direction
		out << (float)u[i][0] << (float)u[i][1] << (float)u[i][2];
		//store outgoing direction
		out << (float)v[i][0] << (float)v[i][1] << (float)v[i][2];
		//store intensity value
		out << (float)y[i];
	}

	file.close();

	u.clear();
	v.clear();
	delete []y;


	QMessageBox::information(this, tr("BRDFLab"),
			tr("BRDF data successfully exported."));

}

void 
BRDFLab::about(){
	QMessageBox::information(this, tr("About BRDFLab"),
		tr("Application developed by: \n   Adria Fores \n   Sumanta N. Pattanaik \n   Carles Bosch \n   Xavier Pueyo"));
}

/**
The material to render has been changed. 

0 -> original BRDF, meaning mainBRDF
1 -> fitted BRDF, pass the signal to the fitted property editor.

*/
void 
BRDFLab::setActiveBRDF(int m){
		
	switch(m){
		case 0:
			mainBRDF->setActiveBRDF(true);
			if(pew2 != NULL)
				pew2->setActiveBRDF(false);

			mainBRDF->updateMaterial();
			break;
		case 1:
			mainBRDF->setActiveBRDF(false);
			if(pew2 != NULL)
				pew2->setActiveBRDF(true);

			pew2->updateMaterial();
			break;
		default:
			mainBRDF->setActiveBRDF(true);
			if(pew2 != NULL)
				pew2->setActiveBRDF(true);
			break;
	}
}


/**
This method recieves signals from the objectRenderer window and the brdfDisplay method.

The meaning of the input is the following one:

-1 : display lobe
 0 : point light
 1 : env. map.

*/
void 
BRDFLab::lightTypeChanged(int l){

	if(objectWindow!=NULL){
		if(objectWindow->isOriginalBRDFRendering()){
			if(mainBRDF!=NULL){
				mainBRDF->setActiveBRDF(true);
				mainBRDF->setMaterial(l);
			}
			if(pew2!=NULL)
				pew2->setActiveBRDF(false);
		}else{
			if(mainBRDF!=NULL)
				mainBRDF->setActiveBRDF(false);
			if(pew2!=NULL){
				pew2->setActiveBRDF(true);
				pew2->setMaterial(l);
			}
		}
	}



}

void
BRDFLab::updateSurfaceViewer(int m){

	switch(m)
	{
	case 0: // height field, also including grooves ;)
		surfaceViewer->displayRelief(dynamic_cast<ReliefBRDF*>(mainBRDF)->getTextureName().toStdString());
		break;
	case 1: // spheres
		surfaceViewer->displaySpheres(dynamic_cast<SpheresBRDF*>(mainBRDF));
		break;
	case 2: // triangles
		surfaceViewer->displayTriangles(dynamic_cast<TrianglesBRDF*>(mainBRDF));
		break;
	case 3: // next!

		break;
	case 4:

		break;
	}
}

void
BRDFLab::updateBRDFViewer(){
	if(window != NULL)
		window->forceUpdate();
}


/**

Code from Colvert and Krivanek to convert his .bob file for meshes to OGRE.

*/
void
BRDFLab::convertBobModel(){
	FILE *f = fopen("happy.bob","rb");

	int vertexCount;
	float *positions;
	float *normals;
	float *uvs;
	float *binormals;
	float *tangents;


	if (!f) throw exception("Invalid Filename");

	fread(&vertexCount, sizeof(int), 1, f);

	if (vertexCount <= 0) throw exception("Invalid File Type");

	unsigned int bufferSize = vertexCount*(4 + 4 + 4 + 4 + 4);

	// allocate the memory
	positions = (float*) calloc(bufferSize, sizeof(float));

	// pass it out
	normals		= &positions[vertexCount*4];
	uvs			= &normals[vertexCount*4];
	binormals	= &uvs[vertexCount*4];
	tangents	= &binormals[vertexCount*4];

	// read in the file
	fread(positions, sizeof(float), bufferSize, f);

	fclose(f);

/*
	for(int i=0; i<vertexCount; i++){
		cout << "Vertex " << i << "===============================================" <<endl;
		cout << "position: "  << positions[i*4+0] << " " << positions[i*4+1]<< " " << positions[i*4+2]<< " " << positions[i*4+3] << endl;
		cout << "normals: "  << normals[i*4+0] << " " << normals[i*4+1]<< " " << normals[i*4+2]<< " " << normals[i*4+3] << endl;
		cout << "uvs: "  << uvs[i*4+0] << " " << uvs[i*4+1]<< " " << uvs[i*4+2]<< " " << uvs[i*4+3] << endl;
		cout << "binormals: "  << binormals[i*4+0] << " " << binormals[i*4+1]<< " " << binormals[i*4+2]<< " " << binormals[i*4+3] << endl;
		cout << "tangents: "  << tangents[i*4+0] << " " << tangents[i*4+1]<< " " << tangents[i*4+2]<< " " << tangents[i*4+3] << endl;
	}
*/

	/// create the VBO of the mesh.
	Ogre::String meshName("sphereBOB");

	// delete the previous instance if exists.
	if(Ogre::MeshManager::getSingleton().resourceExists(meshName)){
		Ogre::MeshManager::getSingleton().remove(meshName);
	}

	Ogre::MeshPtr msh = Ogre::MeshManager::getSingleton().createManual(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	/// Create one submesh
	Ogre::SubMesh* sub = msh->createSubMesh();

	/// Define the vertices (3 floats per vertex + 1 float per texCoord).
	const size_t nVertices = vertexCount;
	int infoPerVertex = 3+3+2+3+3;
	const size_t vbufCount = nVertices*infoPerVertex;
	float *vertices = new float[vbufCount];

	for(unsigned int i=0; i<nVertices; i++){

		// vertex
		vertices[i*infoPerVertex+0] = positions[i*4+0];
		vertices[i*infoPerVertex+1] = positions[i*4+1];
		vertices[i*infoPerVertex+2] = positions[i*4+2];

		// Normal
		vertices[i*infoPerVertex+3] = normals[i*4+0];
		vertices[i*infoPerVertex+4] = normals[i*4+1];
		vertices[i*infoPerVertex+5] = normals[i*4+2];

		// uv
		vertices[i*infoPerVertex+6] = uvs[i*4+0];
		vertices[i*infoPerVertex+7] = uvs[i*4+1];

		// tangent
		vertices[i*infoPerVertex+8] = tangents[i*4+0];
		vertices[i*infoPerVertex+9] = tangents[i*4+1];
		vertices[i*infoPerVertex+10] = tangents[i*4+2];

		// binormal
		vertices[i*infoPerVertex+11] = binormals[i*4+0];
		vertices[i*infoPerVertex+12] = binormals[i*4+1];
		vertices[i*infoPerVertex+13] = binormals[i*4+2];

	}

	/// Define triangles
	/// The values in this table refer to vertices in the above table
	const size_t ibufCount = vertexCount;
	unsigned long *faces = new unsigned long[ibufCount];

	for(int i=0; i<vertexCount; i++){
		faces[i] = (unsigned long)i;
	}

	/// Create vertex data structure
	msh->sharedVertexData = new Ogre::VertexData();
	msh->sharedVertexData->vertexCount = nVertices;

	/// Create declaration (memory format) of vertex data
	Ogre::VertexDeclaration* decl = msh->sharedVertexData->vertexDeclaration;
	size_t offset = 0;
	// 1st buffer
	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	decl->addElement(0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);

	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_TANGENT);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_BINORMAL);
	offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

	/// Allocate vertex buffer of the requested number of vertices (vertexCount) 
	/// and bytes per vertex (offset)
	Ogre::HardwareVertexBufferSharedPtr vbuf = 
		Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
		offset, msh->sharedVertexData->vertexCount, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	/// Upload the vertex data to the card
	vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);

	/// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
	Ogre::VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding; 
	bind->setBinding(0, vbuf);


	/// Allocate index buffer of the requested number of vertices (ibufCount) 
	Ogre::HardwareIndexBufferSharedPtr ibuf = Ogre::HardwareBufferManager::getSingleton().
		createIndexBuffer(
		Ogre::HardwareIndexBuffer::IT_32BIT, 
		ibufCount, 
		Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	/// Upload the index data to the card
	ibuf->writeData(0, ibuf->getSizeInBytes(), faces, true);

	/// Set parameters of the submesh
	sub->useSharedVertices = true;
	sub->indexData->indexBuffer = ibuf;
	sub->indexData->indexCount = ibufCount;
	sub->indexData->indexStart = 0;

	/// Set bounding information (for culling)
	msh->_setBounds(Ogre::AxisAlignedBox(-1,-1,-1,1,1,1));//minCoord[0],minCoord[1],minCoord[2],maxCoord[0],maxCoord[1],maxCoord[2]));
	msh->_setBoundingSphereRadius(1);//(maxCoord-minCoord).length()/2);

//	unsigned short src, dest;
//	if (!msh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
//	{
//		msh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
//	}

	/// Notify Mesh object that it has been loaded
	msh->load();

	//  to export the mesh to disk
	Ogre::MeshSerializer store;
	store.exportMesh(msh.get(), "happyBOB.mesh");

	// clear data structures
	delete [] vertices;
	delete [] faces;



	free(positions);
}

/**
This method download a url and stores it to a path.

The downloading is syncronous and blocks all the application interaction while it is downloading. Also, it shows a Progress Bar, giving information to the user 
and allowing him to cancel the download.

It returns true if the download is completed correctly, otherwise false.

*/
bool
BRDFLab::downloadFile(QString url, QString pathToFile){

	QNetworkAccessManager manager;
	QEventLoop downloadLoop;
	bar = new QProgressDialog(mainWidget);
	bar->setLabelText("Downloading measurement data file ...");

	bar->setWindowModality(Qt::ApplicationModal);
	bar->show();

	QNetworkReply *reply = manager.get( QNetworkRequest(QUrl(url)) );
	QObject::connect(reply, SIGNAL( downloadProgress (qint64,qint64)), this, SLOT(updateProgressBar(qint64,qint64)));
	QObject::connect(bar, SIGNAL(canceled()), &downloadLoop, SLOT(quit()));
	QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &downloadLoop, SLOT(quit()));
	QObject::connect(reply, SIGNAL(finished()), &downloadLoop, SLOT(quit()));
	
	downloadLoop.exec();

	bool saveFile = true;

	if(reply->error() != QNetworkReply::NoError){
		QMessageBox::warning(this, tr("BRDFLab"),
			tr("Download failure. Check your network connection."));

		saveFile = false;
	}

	if(bar->wasCanceled()){
		saveFile = false;
	}

	if(saveFile){
	
		QFile file(pathToFile);
		file.open(QIODevice::WriteOnly);
		file.write(reply->readAll());
		file.close();

		delete bar;
		delete reply;

		return true;
	}else{
		delete bar;
		delete reply;

		return false;
	}
}

void
BRDFLab::updateProgressBar(qint64 current,qint64 max){

	bar->setMaximum(max);
	bar->setValue(current);

}

void 
BRDFLab::setBRDFModified(){
	brdfModified = true;
}

void
BRDFLab::optimizeAll(){

	if(actionOptimize_All->text() == "Optimize All"){

		// start
		actionOptimize_All->setText("Stop Optimization");

		pew2->optimizeAll();

		disconnect(actionOptimize_All, SIGNAL(triggered()), 0,0);
		actionOptimize_All->setText("Optimize All");
		connect(actionOptimize_All, SIGNAL(triggered()), this, SLOT(optimizeAll()));

	}else{
		// stop
		pew2->optimizeAll();
	}
}

