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


#include "ogrewidget.h"

/**

Adaptation of the code from kito berg-taylor.

*/
#define _A 0.525731112119133606f
#define _B 0.850650808352039932f

using namespace std;

//#include "imdebug.h"


void OgreWidget::init( std::string plugins_file,
		 std::string ogre_cfg_file,
		 std::string ogre_log )
{
  // create the main ogre object

	mOgreRoot = new Ogre::Root( plugins_file, ogre_cfg_file, ogre_log );

	// setup a renderer
	Ogre::RenderSystemList renderers = mOgreRoot->getAvailableRenderers();
	assert( !renderers.empty() ); // we need at least one renderer to do anything useful

	Ogre::RenderSystem *renderSystem;
	renderSystem = chooseRenderer( &renderers );

	assert( renderSystem ); // user might pass back a null renderer, which would be bad!

	cout << "Rendersystem used: " << renderSystem->getName() << endl;

	mOgreRoot->setRenderSystem( renderSystem );
	QString dimensions = QString( "%1x%2" )
								.arg(this->width())
								.arg(this->height());

	renderSystem->setConfigOption( "Video Mode", dimensions.toStdString() );

	// initialize without creating window
	mOgreRoot->getRenderSystem()->setConfigOption( "Full Screen", "No" );
	mOgreRoot->saveConfig();
	mOgreRoot->initialise(false); // don't create a window



	
}


void OgreWidget::initializeGL()
{
  //== Creating and Acquiring Ogre Window ==//

// in linux..
  // Get the parameters of the window QT created 
 /* QX11Info info = x11Info();
  Ogre::String winHandle;
  winHandle  = Ogre::StringConverter::toString((unsigned long)(info.display()));
  winHandle += ":";
  winHandle += Ogre::StringConverter::toString((unsigned int)(info.screen()));
  winHandle += ":";
  winHandle += Ogre::StringConverter::toString((unsigned long)(this->parentWidget()->winId()));
*/

//windows
  Ogre::NameValuePairList params;

//	windowHWND = winId();

  params["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)winId() );

  mOgreWindow = mOgreRoot->createRenderWindow( (windowName + Ogre::String("RenderWindow")),
					       this->width(),
					       this->height(),
					       false,
					       &params );
  mOgreWindow->setActive(true);

  setAttribute( Qt::WA_PaintOnScreen, true );
  setAttribute( Qt::WA_NoBackground );

  //== Ogre Initialization ==//
  mSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC,(windowName + Ogre::String("SceneManager")) );
  mSceneMgr->setAmbientLight(Ogre::ColourValue(1.,1.,1.));
  
  mCamera = mSceneMgr->createCamera( "QOgreWidget_Cam" );



  
/*  
  mCamera->setPosition( Ogre::Vector3(0.,200,200) );
  mCamera->lookAt( Ogre::Vector3(0,0,0) );
  if(windowName == "first") // todo ... clean that when removing things from OgreWidget class
	mCamera->setOrientation (Ogre::Quaternion(-0.000806117,0.000333062,0.382683,0.923879));
*/

  mCamera->setNearClipDistance( 1 );
  mCamera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));
  mCamera->setFixedYawAxis(false);
//  mCamera->setPolygonMode(Ogre::PM_WIREFRAME);

  setInitialCameraPosition();
  setUpAxis();
  updateCamera();

  mViewport = mOgreWindow->addViewport( mCamera );	
  mViewport->setBackgroundColour( Ogre::ColourValue( 0,0,0 ) );
  mViewport->setClearEveryFrame(true);
  mViewport->setOverlaysEnabled(false);

  mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
}

void 
OgreWidget::setInitialCameraPosition(){


	mouseRotation[0] = 119;
	mouseRotation[1] = 222;
	cameraZoom = 300;

}


void OgreWidget::paintGL()
{
  assert( mOgreWindow );
  mOgreRoot->renderOneFrame();

}

/**
 * @brief resize the GL window
 * @author Kito Berg-Taylor
 */
void OgreWidget::resizeEvent(QResizeEvent *)
{
  //assert( mOgreWindow );
  if(mOgreWindow){
	mOgreWindow->windowMovedOrResized();
	mCamera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));
	mViewport->update();
  }
}



Ogre::RenderSystem* OgreWidget::chooseRenderer( Ogre::RenderSystemList *renderers )
{
  // It would probably be wise to do something more friendly 
  // that just use the first available renderer
  return *renderers->begin();
}

void OgreWidget::update(){
	if(mOgreWindow){
		mOgreRoot->renderOneFrame();

	}
}

void OgreWidget::paintEvent(){
	if(!mOgreWindow){

		if(mOgreRoot == NULL){ // to solve setup more than 1 window
			init( "plugins.cfg", "ogre.cfg", "ogre.log" );
		 
			initializeGL();
			setupResources();
		}else{
			initializeGL();
		}

		createScene();
		timerId = startTimer(50);
	}else{
		update();
	}


	
}	

void OgreWidget::timerEvent(QTimerEvent *) {
	update();
}

void OgreWidget::setupResources(void) {
	// Load resource paths from config file
	Ogre::ConfigFile config;
	config.load("resources.cfg");
	
	// Go through all sections & settings in the file
	Ogre::ConfigFile::SectionIterator it = config.getSectionIterator();
	
	Ogre::String secName, typeName, archName;
	while (it.hasMoreElements()) {
		secName = it.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = it.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		
		for (i = settings->begin(); i != settings->end(); ++i) {
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
    }
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
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
OgreWidget::createScene(){

	incidentVectorManualObject = NULL;
	measuredPoints = NULL;

	// testing...
	mainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

		// testing...
	lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 	Ogre::Light* l = mSceneMgr->createLight("MainLight");
	
//	l->setPosition(10.,10.,0.);
	lightNode->attachObject(l);
	lightNode->translate(Ogre::Vector3(10,10,100));
	

	setObjects(0);

	// the following lines are used to take pictures for the MsC document
/*
	mViewport->setBackgroundColour(Ogre::ColourValue::White);
	
	//createHemiSphere(5);
	createSphere("Sphere22",100,20,20);

	Ogre::Entity *example = mSceneMgr->createEntity("example","Sphere22");
	mainNode->attachObject(example);
	example->setMaterialName("black");
*/
	
//	mainEntity->setVisible(false);

/*
	Ogre::SceneNode *planeNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Ogre::Plane *p = new Ogre::Plane(Ogre::Vector3(0.,0.,1.), 0.);
	Ogre::MeshPtr m = Ogre::MeshManager::getSingleton().createPlane("Plane",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*p,200,200,1,1,true,1,1.0f,1.0f,Ogre::Vector3::UNIT_Y,Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, true,true );
	Ogre::Entity* planeEntity = mSceneMgr->createEntity("planeEntity", "Plane");
	planeNode->attachObject(planeEntity);
	planeNode->rotate(Ogre::Vector3(1.,0.,0.),Ogre::Radian(Ogre::Math::PI/2.));
	planeNode->translate(Ogre::Vector3(0.,0.,-0.1));
	planeNode->scale(Ogre::Vector3(10.,10.,10.));
	planeEntity->setMaterialName("Checkboard");
//	planeEntity->setMaterialName("test");
*/


/*
	Ogre::Entity *model = mSceneMgr->createEntity("model","car.mesh");
	mainNode->attachObject(model);
	model->setMaterialName("PointLight");


	createSphere("Sphere",10);
	Ogre::Entity *model2 = mSceneMgr->createEntity("ObjectRenderingEntity","Sphere");
	mainNode->attachObject(model2);
	model2->setMaterialName("PointLight");




//	model->setVisible(false);

	Ogre::Plane *p = new Ogre::Plane(*(new Ogre::Vector3(0.,0.,1.)), 0.);
	Ogre::MeshPtr m = Ogre::MeshManager::getSingleton().createPlane("Plane",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*p,200,200,1,1,true,1,1.0f,1.0f,Ogre::Vector3::UNIT_Y,Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, true,true );
	m->buildTangentVectors(Ogre::VES_TANGENT);
//	m->buildTangentVectors(Ogre::VES_BINORMAL);
	Ogre::Entity* thisEntity = mSceneMgr->createEntity("entityPla", "Plane");

	mainNode->attachObject(thisEntity);
	thisEntity->setMaterialName("PointLight");
//	mainEntity->setVisible(false);

*/

	//lightNode->setPosition(Ogre::Vector3(10.,10.,0.));

	//paint axis
	paintAxis();
/*

	Ogre::SceneNode *testing = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	// paint directions
	Ogre::ManualObject *mo = mSceneMgr->createManualObject("testing");\
	
	float minTheta = 3.;
	float minPDF = 3.;
	float maxTheta = 0.;
	float maxPDF = 0.;


	mo->begin("red",Ogre::RenderOperation::OT_LINE_LIST);


	int count = 2;
	int count2 = 2;
	int n = 50;

	for(int i=0; i<100; i++){
		mo->position(Ogre::Vector3::ZERO);

		// phi, theta	
		float phi = 2.*Ogre::Math::PI*RadicalInverse(count++,2);
		float theta = acos(Ogre::Math::Pow(RadicalInverse(count++,3),1./(n+1.)));

	
		Ogre::Vector2 dirPolar(phi,theta);
		Ogre::Vector3 dir(BRDF::spherical2cart(dirPolar));

		float pdf = (n+1)/2.*Ogre::Math::PI*Ogre::Math::Pow(dir[2],n)*sin(theta);

		cout << "theta: " << theta << endl;
		cout << "pdf: " << pdf  << endl;
		cout << "____________________________________________________" << endl;

	
		if(theta < minTheta)
			minTheta = theta;

		if(theta > maxTheta)
			maxTheta = theta;

		if(pdf < minPDF)
			minPDF = pdf;

		if(pdf > maxPDF)
			maxPDF = pdf;



//		cout << "direction: " <<  dir << endl;

		mo->position(dir*100.);
	}

	mo->end();

	testing->attachObject(mo);

	cout << "maxPDF = " << maxPDF << endl;
	cout << "minTheta = " << minTheta << endl;

	cout << "minPDF = " << minPDF << endl;
	cout << "maxTheta = " << maxTheta << endl;
		cout << "____________________________________________________" << endl;
		cout << "____________________________________________________" << endl;

		*/
}



/**
This function is used to set the basic mesh on the display.

*/
void
OgreWidget::setObjects(int x){

	bool mainEntityVisible = true;
	bool fittedEntityVisible = true;

	if(mSceneMgr->hasEntity("mainEntity")){
		mainEntityVisible = mSceneMgr->getEntity("mainEntity")->isVisible();
		mainNode->detachObject("mainEntity");
		mSceneMgr->destroyEntity("mainEntity");
	}

	if(mSceneMgr->hasEntity("fittedEntity")){
		mainEntityVisible = mSceneMgr->getEntity("fittedEntity")->isVisible();
		mainNode->detachObject("fittedEntity");
		mSceneMgr->destroyEntity("fittedEntity");
	}

	Ogre::String meshUsed;

	switch(x){

		case 0:
		case 1:
			meshUsed = Ogre::String("hemisphere.mesh");

			break;
		case 2:
//			meshUsed = Ogre::String("hemisphere.mesh");
			meshUsed = Ogre::String("subdividedQuad.mesh");
			
			break;
		default:
			meshUsed = Ogre::String("hemisphere.mesh");
			break;
	}


	Ogre::Entity *mainEntity = mSceneMgr->createEntity("mainEntity",meshUsed);

	mainNode->attachObject(mainEntity);
	mainEntity->setVisible(mainEntityVisible);

	Ogre::Entity *fittedEntity = mSceneMgr->createEntity("fittedEntity",meshUsed);
	mainNode->attachObject(fittedEntity);
	fittedEntity->setVisible(fittedEntityVisible);

}




/**
If the user press a mouse button, this function will be executed.

*/
void 
OgreWidget::mousePressEvent(QMouseEvent *e) {

//	timerId = startTimer(0);

	mousePressed = true;
	mousePressPos = e->pos();
	buttonPressed = e->button();

	mousePos = (-1,-1);

	killTimer(timerId);
	timerId = startTimer(0);
}

///
/// The user released a mouse button, stop tracking
/// \param e The event data
///
void 
OgreWidget::mouseReleaseEvent(QMouseEvent *) {

//	killTimer(timerId);

	mousePressed = false;

	killTimer(timerId);
	timerId = startTimer(50);
}

///
/// The user moved the mouse, if tracking process it
/// \param e The event data
///

/**
This method is executed when the user move the mouse.

Is used in some modes, like camera moving (0) and edit control points of a line/spline (2)

*/
void 
OgreWidget::mouseMoveEvent(QMouseEvent *e) {

	Ogre::Radian mRotX, mRotY;

	if ((mousePressed)){
		QPoint curPos = e->pos();
			
		Ogre::Vector2 mouseCurPos((Ogre::Real)curPos.x(),(Ogre::Real)curPos.y());

		if(mousePos[0] != -1){
	
						
			Ogre::Vector2 mouseRelPos(mouseCurPos-mousePos);

		//	cout << "mouse rel pos: " << mouseRelPos << endl;

			

		//	cout << "mouse rotation: " << mouseRotation << endl;

			Ogre::Vector3 mtv;
	
			float pan_accelerator = 1.;

			switch(buttonPressed){
				case Qt::RightButton:
	
					cameraZoom += mouseRelPos[1]*3*zoomFactor;
					updateCamera();

					break;

				case Qt::LeftButton:
					mouseRotation += mouseRelPos*0.5;
					updateCamera();

					break;
				case Qt::MidButton:
	// 				cout << "boto central" << endl;
	
					mouseRotation += mouseRelPos*0.5;
					mRotX = Ogre::Degree(-mouseRelPos[0]* 0.1);
					mRotY = Ogre::Degree(-mouseRelPos[1]* 0.1);
					mCamera->yaw(mRotX);
					mCamera->pitch(mRotY);

					break;
	
	
			}

			
		}

		mousePos = mouseCurPos;
	}

}


void 
OgreWidget::wheelEvent(QWheelEvent *e){

	
	float wheel_desaccelerator = 0.050;
	cameraZoom += -e->delta() * wheel_desaccelerator*zoomFactor;

	updateCamera();
	update();
}


void 
OgreWidget::setUpAxis(){
	upAxis = Ogre::Vector3::UNIT_Z;
}

void 
OgreWidget::updateCamera(){

	// Quats to work with
	Ogre::Quaternion cameraRotationAboutWorldY, cameraRotationAboutWorldX, result; 

	// Set up the quaternions using the mouse input
	cameraRotationAboutWorldY.FromAngleAxis(Ogre::Degree(mouseRotation[0]),upAxis);
	cameraRotationAboutWorldX.FromAngleAxis(Ogre::Degree(-mouseRotation[1]),Ogre::Vector3::UNIT_X);

	// Get the result
	result = cameraRotationAboutWorldY * cameraRotationAboutWorldX;

	// Now play with the camera
	mCamera->setPosition(Ogre::Vector3::ZERO);
	mCamera->setOrientation(result);
	mCamera->moveRelative(Ogre::Vector3(0,cameraZoom,0));
	mCamera->lookAt(Ogre::Vector3::ZERO);
}

void 
OgreWidget::keyPressEvent ( QKeyEvent * event ){

	//cout << "key ! " << endl;
/*
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("PointLight");

	// get the vertex shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuFragmentParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();


	float *lala = new float[5];

	lala[0] = 1.;
	lala[1] = 1.;
	lala[2] = 0.;
	lala[3] = 1.;
	lala[4] = 1.;

	// set the parameters
	gpuFragmentParam->setNamedConstant("prova",lala,2,1);
*/

	switch(event->key()){
			case Qt::Key_M:{
				lightNode->translate(Ogre::Vector3(0,10,0));
				//cout << lightNode->getPosition() << endl;
			}
			break;

		case Qt::Key_N:{
				lightNode->translate(Ogre::Vector3(0,-10,0));
				//cout << lightNode->getPosition() << endl;
			}
			break;
		case Qt::Key_Right:{
				lightNode->translate(Ogre::Vector3(10,0,0));
				//cout << lightNode->getPosition() << endl;
			}
			break;

		case Qt::Key_Left:{
				lightNode->translate(Ogre::Vector3(-10,0,0));
				//cout << lightNode->getPosition() << endl;
			}
			break;

		case Qt::Key_W:{
				mCamera->setPolygonMode(Ogre::PM_WIREFRAME);
			}
			break;

		case Qt::Key_Q:{
				mCamera->setPolygonMode(Ogre::PM_SOLID);
			}
			break;

		case Qt::Key_Down:{
				lightNode->translate(Ogre::Vector3(0,0,10));
				//cout << lightNode->getPosition() << endl;
			}
			break;
		case Qt::Key_Up:{
				lightNode->translate(Ogre::Vector3(0,0,-10));
				//cout << lightNode->getPosition() << endl;
			}
			break;

		case Qt::Key_F:{
				const Ogre::RenderTarget::FrameStats& stats = mOgreWindow->getStatistics();
				Ogre::LogManager::getSingleton().logMessage("Cur FPS: " + Ogre::StringConverter::toString(stats.lastFPS));
				Ogre::LogManager::getSingleton().logMessage("Avg FPS: " + Ogre::StringConverter::toString(stats.avgFPS));
				Ogre::LogManager::getSingleton().logMessage("Tri Count: " + Ogre::StringConverter::toString(stats.triangleCount));
				Ogre::LogManager::getSingleton().logMessage("Batch Count: " + Ogre::StringConverter::toString(stats.batchCount));
			}
			break;

		case Qt::Key_J:{
				Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("test");

				// get the fragment shader parameters
				Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	
				gpuParameters->setNamedConstant("level",(Ogre::Real)++tempValue);
				cout << "new mipmap level: " << tempValue << endl;
			}
			break;

		case Qt::Key_K:{
				Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("test");

				// get the fragment shader parameters
				Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	
				gpuParameters->setNamedConstant("level",(Ogre::Real)--tempValue);
				cout << "new mipmap level: " << tempValue << endl;
			}
			break;
		case Qt::Key_P:{
				mOgreWindow->writeContentsToFile("screenshot.png");
			}
			break;

		case Qt::Key_B:{
				cout << "position: " << mCamera->getPosition() << endl;
				cout << "orientation: " << mCamera->getOrientation() << endl;
			}
			break;

		case Qt::Key_V:{
				mCamera->setPosition(9.64601, 20.696, 57.6423);
				mCamera->setOrientation(Ogre::Quaternion(-0.982171, 0.168768, -0.0816121, -0.0140235));
			}
			break;

		case Qt::Key_T:{
				Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("PointLightAnalyticBRDF");

				Ogre::GpuProgramPtr fs = mat->getTechnique(0)->getPass(0)->getFragmentProgram();

				fs->setParameter("preprocessor_defines","TEST=0");

				fs->reload();
				mat->reload();
			}
			break;
   }
}


void
OgreWidget::paintAxis(){
	
	Ogre::SceneNode *axisNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();


	Ogre::ManualObject *XAxis = mSceneMgr->createManualObject("Xaxis"); 
	Ogre::ManualObject *YAxis = mSceneMgr->createManualObject("Yaxis"); 
	Ogre::ManualObject *ZAxis = mSceneMgr->createManualObject("Zaxis"); 

//	Ogre::Vector3 jk(-1.,1.,2.);

	XAxis -> begin("red", Ogre::RenderOperation::OT_LINE_LIST);	
		XAxis -> position(Ogre::Vector3(0.,0.,0.));
		XAxis -> position(Ogre::Vector3(500.,0.,0.));
//		XAxis -> position(jk);
	XAxis->end();

	YAxis -> begin("green", Ogre::RenderOperation::OT_LINE_LIST);	
		YAxis -> position(Ogre::Vector3(0.,0.,0.));
		YAxis -> position(Ogre::Vector3(0.,500.,0.));
//		YAxis -> position(Ogre::Vector3(0.,jk[2],-jk[1]));
	YAxis->end();

	ZAxis -> begin("blue", Ogre::RenderOperation::OT_LINE_LIST);	
		ZAxis -> position(Ogre::Vector3(0.,0.,0.));
		ZAxis -> position(Ogre::Vector3(0.,0.,500.));
//		ZAxis -> position((jk.crossProduct(Ogre::Vector3(0.,jk[2],-jk[1]))));
	ZAxis->end();

	axisNode->attachObject(XAxis);
	axisNode->attachObject(YAxis);
	axisNode->attachObject(ZAxis);

}

void
OgreWidget::setIncidentVector(float phi,float theta){

//	cout << "theta value: " << theta << endl;


	if(incidentVectorManualObject == NULL){
		// if is the first time.. create it
		incidentVectorManualObject = mSceneMgr->createManualObject("incidentVector"); 
		mainNode->attachObject(incidentVectorManualObject);
	}else{
		incidentVectorManualObject->clear();
	}

	incidentVectorManualObject -> begin("orange", Ogre::RenderOperation::OT_LINE_LIST);	

		// incident
		incidentVectorManualObject -> position(Ogre::Vector3(0.,0.,0.));
		incidentVectorManualObject -> position(1000*BRDF::spherical2cart(Ogre::Vector2(phi,theta)));

	incidentVectorManualObject->end();


}

void 
OgreWidget::cleanMeasuredPoints(){
	measuredPoints->clear();
}

void
OgreWidget::paintMeasuredPoints(std::vector<Ogre::Vector3> *points){

	mSceneMgr->getEntity("mainEntity")->setVisible(false);

	if(!measuredPoints){
		measuredPoints = mSceneMgr->createManualObject("MeasuredPoints"); 
		mainNode->attachObject(measuredPoints);
	}else
		measuredPoints->clear();

	measuredPoints -> begin("white", Ogre::RenderOperation::OT_POINT_LIST);	
	

	for(unsigned int i=0; i< points->size(); i++){
		measuredPoints->position(100*(*points)[i]);
	}

	measuredPoints->end();

}

void
OgreWidget::setSecondaryMaterial(QString name){

//	mSceneMgr->getEntity("fittedEntity")->setVisible(true);

	mSceneMgr->getEntity("fittedEntity")->setMaterialName(name.toStdString());

}

void
OgreWidget::setPrimaryMaterial(QString name){

//	mSceneMgr->getEntity("mainEntity")->setVisible(true);

	mSceneMgr->getEntity("mainEntity")->setMaterialName(name.toStdString());

}



/**
void 
OgreWidget::createSphere(const std::string& strName, const float r, const int nRings, const int nSegments)
{
	Ogre::ManualObject * manual = mSceneMgr->createManualObject(strName);
	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		
	float fDeltaRingAngle = (Ogre::Math::PI / nRings);
	float fDeltaSegAngle = (2 * Ogre::Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			manual->position( x0, y0, z0);
			manual->normal(Ogre::Vector3(x0, y0, z0).normalisedCopy());
			manual->textureCoord((float) seg / (float) nSegments, (float) ring / (float) nRings);

			if (ring != nRings) {
				// each vertex (except the last) has six indicies pointing to it
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex);               
				manual->index(wVerticeIndex + nSegments);
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex + 1);
				manual->index(wVerticeIndex);
				wVerticeIndex ++;
				}
		}; // end for seg
	} // end for ring
	manual->end();
	Ogre::MeshPtr mesh = manual->convertToMesh(strName);
	mesh->_setBounds( Ogre::AxisAlignedBox( Ogre::Vector3(-r, -r, -r), Ogre::Vector3(r, r, r) ), false );

	mesh->_setBoundingSphereRadius(r);
       unsigned short src, dest;
   	if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
   	{
      	       mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
   	}
}
*/


void 
OgreWidget::createSphere(const std::string& strName, const float r, const int nRings, const int nSegments)
{
	if(Ogre::MeshManager::getSingleton().resourceExists(strName))
		Ogre::MeshManager::getSingleton().remove(strName);

	if(mSceneMgr->hasManualObject(strName))
		mSceneMgr->destroyManualObject(strName);
	

	Ogre::ManualObject * manual = mSceneMgr->createManualObject(strName);
	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		
	float fDeltaRingAngle = (Ogre::Math::PI / nRings);
	float fDeltaSegAngle = (2 * Ogre::Math::PI / nSegments);
	unsigned short wVerticeIndex = 0 ;

	// Generate the group of rings for the sphere
	for( int ring = 0; ring <= nRings; ring++ ) {
		float r0 = r * sinf (ring * fDeltaRingAngle);
		float y0 = r * cosf (ring * fDeltaRingAngle);

		// Generate the group of segments for the current ring
		for(int seg = 0; seg <= nSegments; seg++) {
			float x0 = r0 * sinf(seg * fDeltaSegAngle);
			float z0 = r0 * cosf(seg * fDeltaSegAngle);

			// Add one vertex to the strip which makes up the sphere
			manual->position( x0, y0, z0);
			manual->normal(Ogre::Vector3(x0, y0, z0).normalisedCopy());
			manual->textureCoord((float) seg / (float) nSegments, (float) ring / (float) nRings);

			if (ring != nRings) {
				// each vertex (except the last) has six indicies pointing to it
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex);               
				manual->index(wVerticeIndex + nSegments);
				manual->index(wVerticeIndex + nSegments + 1);
				manual->index(wVerticeIndex + 1);
				manual->index(wVerticeIndex);
				wVerticeIndex ++;
				}
		}; // end for seg
	} // end for ring
	manual->end();
	Ogre::MeshPtr mesh = manual->convertToMesh(strName);
	mesh->_setBounds( Ogre::AxisAlignedBox( Ogre::Vector3(-r, -r, -r), Ogre::Vector3(r, r, r) ), false );

	mesh->_setBoundingSphereRadius(r);
       unsigned short src, dest;
   	if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
   	{
      	       mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
   	}
}




void
OgreWidget::createHemiSphere(int level){

	// build an icosaedre
	std::vector<Ogre::Vector3> vertex_;

	vertex_.push_back(Ogre::Vector3(-_A,0.0,_B));
	vertex_.push_back(Ogre::Vector3(_A,0.0,_B));
	vertex_.push_back(Ogre::Vector3(-_A,0.0,-_B));
	vertex_.push_back(Ogre::Vector3(_A,0.0,-_B));
	vertex_.push_back(Ogre::Vector3(0.0,_B,_A));
	vertex_.push_back(Ogre::Vector3(0.0,_B,-_A));
	vertex_.push_back(Ogre::Vector3(0.0,-_B,_A));
	vertex_.push_back(Ogre::Vector3(0.0,-_B,-_A));
	vertex_.push_back(Ogre::Vector3(_B,_A,0.0));
	vertex_.push_back(Ogre::Vector3(-_B,_A,0.0));
	vertex_.push_back(Ogre::Vector3(_B,-_A,0.0));
	vertex_.push_back(Ogre::Vector3(-_B,-_A,0.0));


	cout << "Nvertex: " << vertex_.size() << endl;

	std::vector<Ogre::Vector3> indexInicials_;

	indexInicials_.push_back(Ogre::Vector3(0,4,1));
	indexInicials_.push_back(Ogre::Vector3(0,9,4));
	indexInicials_.push_back(Ogre::Vector3(9,5,4));
	indexInicials_.push_back(Ogre::Vector3(4,5,8));
	indexInicials_.push_back(Ogre::Vector3(4,8,1));
	indexInicials_.push_back(Ogre::Vector3(8,10,1));
	indexInicials_.push_back(Ogre::Vector3(8,3,10));
	indexInicials_.push_back(Ogre::Vector3(5,3,8));
	indexInicials_.push_back(Ogre::Vector3(5,2,3));
	indexInicials_.push_back(Ogre::Vector3(2,7,3));
	indexInicials_.push_back(Ogre::Vector3(7,10,3));
	indexInicials_.push_back(Ogre::Vector3(7,6,10));
	indexInicials_.push_back(Ogre::Vector3(7,11,6));
	indexInicials_.push_back(Ogre::Vector3(11,0,6));
	indexInicials_.push_back(Ogre::Vector3(0,1,6));
	indexInicials_.push_back(Ogre::Vector3(6,1,10));
	indexInicials_.push_back(Ogre::Vector3(9,0,11));
	indexInicials_.push_back(Ogre::Vector3(9,11,2));
	indexInicials_.push_back(Ogre::Vector3(9,2,5));
	indexInicials_.push_back(Ogre::Vector3(7,2,11));

	cout << "Ntriangles: " << indexInicials_.size() << endl;

	std::vector<Ogre::Vector3> index_;

	for(unsigned int i=0; i<indexInicials_.size(); i++){
		subdivideTriangle(indexInicials_[i],&vertex_, &index_, level);
	}

	cout << "Nvertex finals: " << vertex_.size() << endl;
	cout << "Ntriangles finals : " << index_.size() << endl;


	// normalize the vertex
	for(unsigned int i=0; i<vertex_.size(); i++){

		//if the Z coord is negative, set to the plane (Z = 0)
	//	if(vertex_[i][2] < 0){
	//		vertex_[i][2] = 0.;
	//	}

		vertex_[i].normalise();
	}


	// hemisphere
	std::vector<Ogre::Vector3> indexHemi_;
	std::vector<Ogre::Vector3> vertexHemi_;


	// for each triangle, look if all the points are on the upper hemisphere
	for(unsigned int i=0; i<index_.size(); i++){

		// if the triangle is not flat on the XZ plane, add to the final list.
	//	if(!((vertex_[index_[i][0]][2] == 0.)&&(vertex_[index_[i][1]][2] == 0.)&&(vertex_[index_[i][2]][2] == 0.))){
			
			// search vertex on storage if it exists, get the position. 
			// If not, create the new vertex and save it to the new storage.
			int pos0,pos1,pos2;
			if(!exists(vertex_[index_[i][0]], &vertexHemi_, pos0)){
				vertexHemi_.push_back(vertex_[index_[i][0]]); // size -1
				pos0 = vertexHemi_.size()-1;
			}

			if(!exists(vertex_[index_[i][1]], &vertexHemi_, pos1)){
				vertexHemi_.push_back(vertex_[index_[i][1]]); // size -1
				pos1 = vertexHemi_.size()-1;
			}
			
			if(!exists(vertex_[index_[i][2]], &vertexHemi_, pos2)){
				vertexHemi_.push_back(vertex_[index_[i][2]]); // size -1 
				pos2 = vertexHemi_.size()-1;
			}

			Ogre::Vector3 tri(pos0,pos1,pos2); 

			indexHemi_.push_back(tri);
	//	}
	}

	cout << "Nvertex hemi: " << vertexHemi_.size() << endl;
	cout << "Ntriangles hemi : " << indexHemi_.size() << endl;

	// clear previous data structures
	indexInicials_.clear();
	vertex_.clear();
	index_.clear();

	/// create the VBO of the mesh.
	Ogre::String meshName("hemiSphereMesh");

	// delete the previous instance if exists.
	if(Ogre::MeshManager::getSingleton().resourceExists(meshName)){
		Ogre::MeshManager::getSingleton().remove(meshName);
	}
	
	Ogre::MeshPtr msh = Ogre::MeshManager::getSingleton().createManual(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	/// Create one submesh
	Ogre::SubMesh* sub = msh->createSubMesh();

	/// Define the vertices (3 floats per vertex + 1 float per texCoord).
	const size_t nVertices = vertexHemi_.size();
	const size_t vbufCount = 8*nVertices;
	float *vertices = new float[vbufCount];

	for(unsigned int i=0; i<vertexHemi_.size(); i++){

		float lenght =  vertexHemi_[i].length();

		vertices[i*8+0] = vertexHemi_[i][0]; // X coord
		vertices[i*8+1] = vertexHemi_[i][1]; // Y coord
		vertices[i*8+2] = vertexHemi_[i][2]; // Z coord

											 // Normals...
		vertices[i*8+3] = vertexHemi_[i][0]; // X coord 
		vertices[i*8+4] = vertexHemi_[i][1]; // Y coord
		vertices[i*8+5] = vertexHemi_[i][2]; // Z coord

		vertices[i*8+6] = Ogre::Math::ASin(vertexHemi_[i][0]).valueRadians()/Ogre::Math::PI + 0.5;//  asin(Nx)/PI + 0.5;//fabs(vertexHemi_[i][0])/lenght; // Y coord
		vertices[i*8+7] = Ogre::Math::ASin(vertexHemi_[i][1]).valueRadians()/Ogre::Math::PI + 0.5;//asin(Ny)/PI + 0.5 ;//fabs(vertexHemi_[i][1])/lenght; // Z coord
	}

	/// Define triangles
	/// The values in this table refer to vertices in the above table
	const size_t ibufCount = indexHemi_.size()*3;
	unsigned long *faces = new unsigned long[ibufCount];

	for(unsigned int i=0; i<indexHemi_.size(); i++){
		faces[i*3] = (unsigned long)indexHemi_[i][2];
		faces[i*3+1] = (unsigned long)indexHemi_[i][1];
		faces[i*3+2] = (unsigned long)indexHemi_[i][0];
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

    unsigned short src, dest;
   	if (!msh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
   	{
      	       msh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
   	}

	/// Notify Mesh object that it has been loaded
	msh->load();

//  to export the mesh to disk
	Ogre::MeshSerializer store;
    store.exportMesh(msh.get(), "Sphere_level6.mesh");

	// clear data structures
	vertexHemi_.clear();
	indexHemi_.clear();
	
	delete [] vertices;
	delete [] faces;

}

bool
OgreWidget::exists(Ogre::Vector3 v, std::vector<Ogre::Vector3> *vertex_, int &pos){
		
	bool trobat=false;
	unsigned int i=0;

	while((!trobat)&&(i<(*vertex_).size())){
		if(v == (*vertex_)[i])
			trobat=true;
		else
			i++;
	}
	
	if(trobat){
		pos = i;
		return true;
	}else{
		return false;
	}
}

void 
OgreWidget::subdivideTriangle(Ogre::Vector3 tri,std::vector<Ogre::Vector3> *vertex_, std::vector<Ogre::Vector3> *index_, int level){
		
		if(level>0){
			Ogre::Vector3 v01(((*vertex_)[tri[0]] + (*vertex_)[tri[1]])/2);
			Ogre::Vector3 v02(((*vertex_)[tri[0]] + (*vertex_)[tri[2]])/2);
			Ogre::Vector3 v12(((*vertex_)[tri[1]] + (*vertex_)[tri[2]])/2);

			(*vertex_).push_back(v01); // size -3
			(*vertex_).push_back(v02); // size -2
			(*vertex_).push_back(v12); // size -1

			int vertexSize = (*vertex_).size();

			Ogre::Vector3 tri0(tri[0],vertexSize-3, vertexSize-2);
			Ogre::Vector3 tri1(vertexSize-3,tri[1], vertexSize-1);
			Ogre::Vector3 tri2(vertexSize-3,vertexSize-1, vertexSize-2);
			Ogre::Vector3 tri3(tri[2],vertexSize-2, vertexSize-1);

/*

			if( ((*vertex_)[tri[0]][1]>0) && ((*vertex_)[vertexSize-3][1]>0) && ((*vertex_)[vertexSize-2][1]>0)){
				Ogre::Vector3 tri0(tri[0],vertexSize-3, vertexSize-2);
				subdivideTriangle(tri0, vertex_,index_,level-1);
			}
			if( ((*vertex_)[vertexSize-3][1]>0) && ((*vertex_)[tri[1]][1]>0) && ((*vertex_)[vertexSize-1][1]>0)){
				Ogre::Vector3 tri1(vertexSize-3,tri[1], vertexSize-1);
				subdivideTriangle(tri1, vertex_,index_,level-1);
			}
			if( ((*vertex_)[vertexSize-3][1]>0) && ((*vertex_)[vertexSize-1][1]>0) && ((*vertex_)[vertexSize-2][1]>0)){
				Ogre::Vector3 tri2(vertexSize-3,vertexSize-1, vertexSize-2);
				subdivideTriangle(tri2, vertex_,index_,level-1);
			}
			if( ((*vertex_)[tri[2]][1]>0) && ((*vertex_)[vertexSize-2][1]>0) && ((*vertex_)[vertexSize-1][1]>0)){
				Ogre::Vector3 tri3(tri[2],vertexSize-2, vertexSize-1);
				subdivideTriangle(tri3, vertex_,index_,level-1);
			}
*/
			subdivideTriangle(tri0, vertex_,index_,level-1);
			subdivideTriangle(tri1, vertex_,index_,level-1);
			subdivideTriangle(tri2, vertex_,index_,level-1);
			subdivideTriangle(tri3, vertex_,index_,level-1);
		}else{
			(*index_).push_back(tri);
		}

}



void
OgreWidget::drawImportanceSamplingDirections(std::vector<Ogre::Vector3> *directions){

	Ogre::String objectName("importanceSampledDirections");

	Ogre::ManualObject *mo;

	if(!mSceneMgr->hasManualObject(objectName)){
		mo = mSceneMgr->createManualObject(objectName);	
		mainNode->attachObject(mo);
	}else{
		mo = mSceneMgr->getManualObject(objectName);
		mo->clear();
	}

	if(directions != NULL){
		// draw all the directions!
		mo->begin("red",Ogre::RenderOperation::OT_LINE_LIST);

		for(int i=0; i<directions->size(); i++){
			mo->position(Ogre::Vector3::ZERO);

			mo->position((*directions)[i]*1000.);
		}

		mo->end();
	}


}

void 
OgreWidget::displayPrimaryMaterial(bool a){

	mSceneMgr->getEntity("mainEntity")->setVisible(a);

}

void 
OgreWidget::displaySecondaryMaterial(bool a){

	mSceneMgr->getEntity("fittedEntity")->setVisible(a);

}

/**
This function is used in order to stop the rendering of this window.

It is indeed to be used when the window is not shown to the user.

*/
/*
void 
OgreWidget::setActive(bool a) {
	if(a){
		timerId = startTimer(50);
	}else{
		killTimer(timerId);
	}
}
*/