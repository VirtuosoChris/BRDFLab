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

#include "OgreObjectRenderWindow.h"

#include "imdebug.h"

using namespace std;

OgreObjectRenderWindow::OgreObjectRenderWindow(QWidget *parent, Ogre::String name):
OgreWidget(parent,name),
pointLightMaterialName("PointLightAnalyticBRDF"),
envMapLightMaterialName("EnvMapAnalyticBRDF")	

{
}

void 
OgreObjectRenderWindow::setLightType(int x){
	
	lightType = x;

	switch(lightType){

		case 0: // Point Light
		//	mSceneMgr->setSkyBox(false,"GardenSkyBox");
			sphereSkyNode->setVisible(false);
			mSceneMgr->getEntity("ObjectRenderingEntity")->setMaterialName(pointLightMaterialName);
		//	mainEntity->setMaterialName("PointLight");
			lightNode->setVisible(true);
			break;

		case 1: // Filtered Environment Light
		//	mSceneMgr->setSkyBox(true, "GardenSkyBox" );
			sphereSkyNode->setVisible(true);
			mSceneMgr->getEntity("ObjectRenderingEntity")->setMaterialName(envMapLightMaterialName);
			lightNode->setVisible(false);
			break;

		case 2: // Environment Light
			// activate cube map?
	//		mSceneMgr->setSkyBox(true, "GardenSkyBox" );
			sphereSkyNode->setVisible(false);
			mSceneMgr->getEntity("ObjectRenderingEntity")->setMaterialName("PointLight3DTextureBRDF");
			lightNode->setVisible(true);
			break;

		case 3: 
			sphereSkyNode->setVisible(true);
			lightNode->setVisible(false);
			mSceneMgr->getEntity("ObjectRenderingEntity")->setMaterialName("FilteredEnvMap");

			break;
	}
}

void 
OgreObjectRenderWindow::setMaterials(Ogre::String point, Ogre::String envMap){
	pointLightMaterialName = point;
	envMapLightMaterialName = envMap;
}


void 
OgreObjectRenderWindow::setModel(int x){

	model = x;
	

	mainNode->detachAllObjects();
	mSceneMgr->destroyEntity("ObjectRenderingEntity");
	
	Ogre::Entity *mainEntity;

	switch(model){

		case 0: // Plane
			mainEntity = mSceneMgr->createEntity("ObjectRenderingEntity","sphere.mesh");
//			mainEntity->setMaterialName("HDRSkyBox");
			mainNode->setScale(Ogre::Vector3(100.,100.,100.));
			break;

		case 1: // Sphere
			// activate cube map?
			mainEntity = mSceneMgr->createEntity("ObjectRenderingEntity","dragon.mesh");
//			mainNode->rotate(Ogre::Vector3(1.,0.,0.),Ogre::Radian(1.));
			//mainEntity->setMaterialName("HDRSkyBox");
			mainNode->setScale(Ogre::Vector3(1000.,1000.,1000.));
			break;

		case 2: // cube 6 faces
			mainEntity = mSceneMgr->createEntity("ObjectRenderingEntity","happy.mesh");
			mainNode->setScale(Ogre::Vector3(1000.,1000.,1000.));
			break;

		case 3: // cube 512
			mainEntity = mSceneMgr->createEntity("ObjectRenderingEntity","xyzrgb_dragon.mesh");
			mainNode->setScale(Ogre::Vector3(1.6,1.6,1.6));
			break;
			
		case 4: // Car
			mainEntity = mSceneMgr->createEntity("ObjectRenderingEntity","armadillo.mesh");
			mainNode->setScale(Ogre::Vector3(1.4,1.4,1.4));
			break;
	

		case 5: // What else? :)
			mainEntity = mSceneMgr->createEntity("ObjectRenderingEntity","bunny.mesh");
			mainNode->setScale(Ogre::Vector3(1000.,1000.,1000.));
			break;
		case 6: // What else? :)
			mainEntity = mSceneMgr->createEntity("ObjectRenderingEntity","neptune3_blender_cleaned.mesh");
			mainNode->setScale(Ogre::Vector3(-10.,-10.,-10.));
			break;
			
	}

	mainNode->attachObject(mainEntity);

	setLightType(lightType);
}

void 
OgreObjectRenderWindow::resizeEvent(QResizeEvent *)
{
  //assert( mOgreWindow );
  if(mOgreWindow){
	mOgreWindow->windowMovedOrResized();
	mCamera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));

	float x = width()*0.2;
	float y = height()*0.2;
	panel->setPosition(width()-x-10, height()-y-10);
	panel->setDimensions(x, y);
	mViewport->update();
  }
}


/**

Here the object and light scene nodes are created. Also, the Sphere and Plane objects are created.

*/
void 
OgreObjectRenderWindow::createScene(){


	cout << "begin model preproces..." << endl;

	generateTangents("sphere.mesh");
	generateTangents("dragon.mesh");
	generateTangents("happy.mesh");
	generateTangents("xyzrgb_dragon.mesh");
	generateTangents("armadillo.mesh");
	generateTangents("bunny.mesh");
//	generateTangents("athena.mesh");
//	generateTangents("happy.mesh");

	cout << "end model preproces..." << endl;



	// create object scene node
	mainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();


	//createHemiSphere(5);
	createSphere("Sphere",10,100,100);

//	QString hdrBackground("../Data/textures/dualparaboloids/uffizi_probe.hdr");

//	loadHDRBackground(hdrBackground);
	//createSphere("SphereSky",1000,30,30);



	Ogre::Entity *sphereSkyEntity = mSceneMgr->createEntity("sphereSkyEntity","skySphere.mesh");
//	sphereSkyEntity->setMaterialName("green");
	sphereSkyEntity->setMaterialName("EnvSphere");

	sphereSkyNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	sphereSkyNode->attachObject(sphereSkyEntity);
	sphereSkyNode->setScale(Ogre::Vector3(10000.,10000.,10000.));


	// create light scene node
	lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	Ogre::Light* l = mSceneMgr->createLight("MainLight");
	lightNode->attachObject(l);
	lightNode->setScale(Ogre::Vector3(0.5,0.5,0.5));


	Ogre::Entity *lightBall = mSceneMgr->createEntity("lightBall","Sphere");
	lightBall->setMaterialName("white");
	lightNode->attachObject(lightBall);
	lightNode->setScale(Ogre::Vector3(0.2,0.2,0.2));
	lightNode->translate(Ogre::Vector3(0,300,0));

	Ogre::Plane *p = new Ogre::Plane(Ogre::Vector3(0.,1.,0.), 0.);
	Ogre::MeshPtr m = Ogre::MeshManager::getSingleton().createPlane("Plane",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*p,200,200,1,1,true,1,1.0f,1.0f,Ogre::Vector3::UNIT_Z,Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, true,true );
	m->buildTangentVectors(Ogre::VES_TANGENT);


//	paintAxis();
//	startTimer(0);


	//generateUniformSampledDirections(200);
//	genLafortuneSamples(80, -1., 1., 15.);

//	mViewport->setBackgroundColour(Ogre::ColourValue::White);

	mViewport->setOverlaysEnabled(false);

	Ogre::OverlayManager& overlayManager = Ogre::OverlayManager::getSingleton();

	// Create a panel
	panel = static_cast<Ogre::OverlayContainer*>(
		overlayManager.createOverlayElement("Panel", "PanelName"));
	panel->setMetricsMode(Ogre::GMM_PIXELS);
	panel->setPosition(10, 10);
	panel->setDimensions(100, 100);
	panel->setMaterialName("Overlay1"); // Optional background material

	// Create an overlay, and add the panel
	Ogre::Overlay* overlay = overlayManager.create("OverlayName");
	overlay->add2D(panel);

	// Show the overlay
	overlay->show();

	setProbe(0);

	//TODO: remove that

		// get the material
//	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("EnvMap");

	// get the fragment shader parameters
//	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

//	gpuParameters->setNamedConstant("nDirections",(Ogre::Real)n);
//	gpuParameters->setNamedConstant("level",(Ogre::Real)1.);

	
//	mainNode->rotate(Ogre::Vector3(1.,0.,0.),Ogre::Radian(Ogre::Math::PI));

	//TODO: Test, remove it please
//	sphericalHarmonicsCoeficients();
}

void
OgreObjectRenderWindow::generateTangents(Ogre::String meshName){

	Ogre::MeshManager::getSingleton().load(meshName,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::MeshPtr pMesh = Ogre::MeshManager::getSingleton().getByName(meshName);

	unsigned short src, dest;
	if (!pMesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
	{	
		pMesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
		cout << "Tangents generated for model " << meshName << endl;
	}

}


void
OgreObjectRenderWindow::generateUniformSampledDirections(int n){


	// uniform distribution over an hemisphere
	// phi = 2*PI*random() 
	// theta = arccos(random())


	float *uniformDirections = new float[n*2];

	for(int i=0; i<n; i++){
		uniformDirections[i*2+0] = 2*Ogre::Math::PI*(float)Ogre::Math::UnitRandom(); // phi
		uniformDirections[i*2+1] = (float)Ogre::Math::ACos(Ogre::Math::UnitRandom()).valueRadians(); // theta
	}


	
	// get the material
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("EnvMapAnalyticBRDF");

	// get the fragment shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

//	gpuParameters->setNamedConstant("nDirections",(Ogre::Real)n);
	gpuParameters->setNamedConstant("directions",uniformDirections, n, 2);

	mat->load();


	delete []uniformDirections;
}

void
OgreObjectRenderWindow::setNSamples(int n){
	
	// get the material
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("EnvMapAnalyticBRDF");

	// get the fragment shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("nDirections",(Ogre::Real)n);

	mat->load();


}



/**

Generate the new samples and send it to the appropiate shader.

*//*
void 
OgreObjectRenderWindow::setNSamples(int n){
	setNSamples(n);	
}*/



void
OgreObjectRenderWindow::setKeyGamma(Ogre::Vector3 value){

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("EnvSphere");
	
	// get the fragment shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();


//powf(10.f,exposureKey->GetValue()), 1.f/gamma->GetValue()

	gpuParameters->setNamedConstant("key",Ogre::Math::Pow(Ogre::Real(10.), Ogre::Real(value[0])));
	gpuParameters->setNamedConstant("gamma",Ogre::Real(1./value[1]));

	//set the same to the env map material
	mat = Ogre::MaterialManager::getSingleton().getByName("EnvMapAnalyticBRDF");
	
	// get the fragment shader parameters
	gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("key",Ogre::Math::Pow(Ogre::Real(10.), Ogre::Real(value[0])));
	gpuParameters->setNamedConstant("gamma",Ogre::Real(1./value[1]));

	//set it to the mirror env map too..
	mat = Ogre::MaterialManager::getSingleton().getByName("EnvMap");
	
	// get the fragment shader parameters
	gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("key",Ogre::Math::Pow(Ogre::Real(10.), Ogre::Real(value[0])));
	gpuParameters->setNamedConstant("gamma",Ogre::Real(1./value[1]));


	mat = Ogre::MaterialManager::getSingleton().getByName("PointLightAnalyticBRDF");
	
	// get the fragment shader parameters
	gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("key",Ogre::Math::Pow(Ogre::Real(10.), Ogre::Real(value[0])));
	gpuParameters->setNamedConstant("gamma",Ogre::Real(1./value[1]));

	// 3d texture point light
	mat = Ogre::MaterialManager::getSingleton().getByName("PointLight3DTextureBRDF");
	
	// get the fragment shader parameters
	gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("key",Ogre::Math::Pow(Ogre::Real(10.), Ogre::Real(value[0])));
	gpuParameters->setNamedConstant("gamma",Ogre::Real(1./value[1]));



}


void
OgreObjectRenderWindow::sphericalHarmonicsCoeficients(QString hdrBackground){

	float *r = new float[16];
	float *g = new float[16];
	float *b = new float[16];

//	QString hdrBackground("../Data/textures/dualparaboloids/ennis_probe.hdr");

	std::string name = hdrBackground.toStdString();

	const char *fileName = name.c_str();


	const unsigned int reflectMapSize = 512;
	float *fBuffer = new float[reflectMapSize*reflectMapSize*3*3];
	float *bBuffer = &fBuffer[reflectMapSize*reflectMapSize*3];


	HDRImage *image = new HDRImage(fileName);
	HDRProbe *probe = new HDRProbe(image);

	probe->ConstructSHMatrices(r,g,b);

	Ogre::Matrix4 R(r[0],r[4],r[8], r[12],
					r[1],r[5],r[9], r[13],
					r[2],r[6],r[10], r[14],
					r[3],r[7],r[11], r[15]);

	Ogre::Matrix4 G(g[0],g[4],g[8], g[12],
					g[1],g[5],g[9], g[13],
					g[2],g[6],g[10], g[14],
					g[3],g[7],g[11], g[15]);

	Ogre::Matrix4 B(b[0],b[4],b[8], b[12],
					b[1],b[5],b[9], b[13],
					b[2],b[6],b[10], b[14],
					b[3],b[7],b[11], b[15]);

	// get the material
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("EnvMapAnalyticBRDF");

	// get the fragment shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuParameters->setNamedConstant("rM",R);
	gpuParameters->setNamedConstant("gM",G);
	gpuParameters->setNamedConstant("bM",B);

	delete probe;
	delete image;
	delete []fBuffer;
	delete []r;
	delete []g;
	delete []b;

}


/**

This code load from hard drive the HDR environment map (single image). And constructs the dual paraboloid map (front and back).

Also, deletes the previous textures with the same name.

Loading code adapted from Mark Colbert & Jaroslav Krivanek

*/
void
OgreObjectRenderWindow::loadHDRBackground(QString filename){

	//name of the hdr textures used in the dual paraboloid map
	Ogre::String front("DualParaboloidFront");
	Ogre::String back("DualParaboloidBack");

	// clean textures

	//front
	if(Ogre::TextureManager::getSingleton().resourceExists(front)){
		Ogre::TextureManager::getSingleton().remove(front);
	}

	//back
	if(Ogre::TextureManager::getSingleton().resourceExists(back)){
		Ogre::TextureManager::getSingleton().remove(back);
	}

	std::string name = filename.toStdString();

	const char *fileName = name.c_str();

//	const char *fileName = "../Data/Textures/kitchen_probe.hdr";

	const unsigned int reflectMapSize = 512;
	float *fBuffer = new float[reflectMapSize*reflectMapSize*3*3];
	float *bBuffer = &fBuffer[reflectMapSize*reflectMapSize*3];


	HDRImage *image = new HDRImage(fileName);
	HDRProbe *probe = new HDRProbe(image);

	probe->ConstrutDualParabolicMap(fBuffer, bBuffer, reflectMapSize, reflectMapSize);


	//test begin
/*
	float *prova = new float[reflectMapSize*reflectMapSize*3+reflectMapSize*reflectMapSize*3/4];

	for(int i=0; i<reflectMapSize*reflectMapSize*3+reflectMapSize*reflectMapSize*3/4; i++){

		if(i<reflectMapSize*reflectMapSize*3)
			if(i % 3 == 0)
				prova[i] = 1.;
			else
				prova[i] = 0.;
		else
			if(i % 3 == 1)
				prova[i] = 1.;
			else
				prova[i] = 0.;

	}

	imdebug("rgb b=32f w=%d h=%d %p", reflectMapSize, reflectMapSize, prova);
	imdebug("rgb b=32f w=%d h=%d %p", reflectMapSize/2, reflectMapSize/2, prova+(reflectMapSize*reflectMapSize*3));
*/
	//test end


	Ogre::DataStreamPtr fTex = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(fBuffer,reflectMapSize*reflectMapSize*3*sizeof(float)));
	Ogre::Image *fTexImage = new Ogre::Image();
	fTexImage->loadRawData(fTex,reflectMapSize,reflectMapSize,1,Ogre::PF_FLOAT32_RGB ,1,0);

	fTexImage->save("-front.hdr");
	cout << "num mipmaps: " << fTexImage->getNumMipmaps() << endl;
	Ogre::TextureManager::getSingleton().loadImage(front,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*fTexImage,Ogre::TEX_TYPE_2D,1,1.0f,false,Ogre::PF_FLOAT32_RGB,true);

	Ogre::DataStreamPtr bTex = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(bBuffer,reflectMapSize*reflectMapSize*3*sizeof(float)));
	Ogre::Image *bTexImage = new Ogre::Image();
	bTexImage->loadRawData(bTex,reflectMapSize,reflectMapSize,1,Ogre::PF_FLOAT32_RGB ,1,0);
	bTexImage->save("-back.hdr");
/*
	Ogre::DataStreamPtr bTex = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(prova,(reflectMapSize*reflectMapSize*3/4+reflectMapSize*reflectMapSize*3)*sizeof(float)));
	Ogre::Image *bTexImage = new Ogre::Image();
	bTexImage->loadRawData(bTex,reflectMapSize,reflectMapSize,1,Ogre::PF_FLOAT32_RGB ,1,1);
	cout << "num mipmaps: " << bTexImage->getNumMipmaps() << endl;
	//Ogre::TextureManager::getSingleton().loadImage(back,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*bTexImage,Ogre::TEX_TYPE_2D,Ogre::TextureMipmap::MIP_UNLIMITED,1.0f,false,Ogre::PF_FLOAT32_RGB);
	Ogre::TextureManager::getSingleton().loadImage(back,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*bTexImage,Ogre::TEX_TYPE_2D,1,1.0f,false,Ogre::PF_FLOAT32_RGB,true);
*/
//	Ogre::TextureManager::getSingleton().loadImage("front_nomip",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*fTexImage,Ogre::TEX_TYPE_2D,0,1.0f,false,Ogre::PF_FLOAT32_RGB,true);
//	Ogre::TextureManager::getSingleton().loadImage("back_nomip",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*bTexImage,Ogre::TEX_TYPE_2D,0,1.0f,false,Ogre::PF_FLOAT32_RGB,true);

	
//	imdebug("rgb b=32f w=%d h=%d %p", reflectMapSize, reflectMapSize, fBuffer);
//	imdebug("rgb b=32f w=%d h=%d %p", reflectMapSize, reflectMapSize, bBuffer);

	delete probe;
	delete image;
	delete []fBuffer;

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
OgreObjectRenderWindow::setInitialCameraPosition(){
	// initial camera position
	mouseRotation[0] = 119;
	mouseRotation[1] = 334;
	cameraZoom = 800;
}

void 
OgreObjectRenderWindow::setUpAxis(){


	upAxis = Ogre::Vector3::UNIT_Y;
}



void
OgreObjectRenderWindow::genLafortuneSamples(int smpls, float cxy, float cz, float n) {

	float* randnum = new float[smpls];
	float* randcos = new float[smpls];
	float* randsin = new float[smpls];
	float* randlog = new float[smpls];


	float minrnd=0.000001;
	float maxrnd=1-minrnd;


	// void genHaltonSequence(int n) {
	for (int i=0; i < smpls; i++) {
		randnum[i] = (float) RadicalInverse(i,2);
		if(randnum[i]<minrnd) randnum[i]=minrnd;
		if(randnum[i]>maxrnd) randnum[i]=maxrnd;
		randlog[i] = logf(randnum[i]);

		double e2 = 2.*Ogre::Math::PI*RadicalInverse(i,3);
		randcos[i] = (float) cos(e2);
		randsin[i] = (float) sin(e2);		
	}



	float *lSmpls = new float[smpls*4];
	float *lScales = new float[smpls];

	double M_LN2 = 0.69314718055994530942;

	// format of the data
	// - u_x, u_y, u_z, biasCmp
	// - normalize*BRDF/PDF (partial)

	// Log[2, I*I/N/(2Pi)]*0.5 = Log[2, 512*512/40/(2*Pi)]*0.5 = 5.01329+1
	float lodPreComp = logf(512.f*512.f/((float) smpls)/(2.f*((float) Ogre::Math::PI)))/((float) M_LN2)*0.5f+1.f;

	for (int i=0; i < smpls; i++) {
		float costheta = powf(randnum[i], 1.f/(n+1.f));
		float sintheta = sqrtf(1.f - costheta*costheta);
		
		lSmpls[i*4+0] = randcos[i]*sintheta;
		lSmpls[i*4+1] = randsin[i]*sintheta;
		lSmpls[i*4+2] = costheta;
		
		float pdf = (n+1)*powf(costheta, n)/(((float) Ogre::Math::PI)*2.f);
		
		
		lSmpls[i*4+3] = max(0.f, lodPreComp - logf(pdf)/((float) M_LN2)*0.5f);
		lScales[i] = (n+2)*powf(costheta, n)/pdf;
	}


	
	// get the material
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("EnvMapAnalyticBRDF");

	// get the fragment shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("samples",(Ogre::Real)smpls);
	gpuParameters->setNamedConstant("smpls",lSmpls, smpls, 4);
	gpuParameters->setNamedConstant("fs",lScales, smpls, 1);

	mat->load();


	delete []randnum;
	delete []randcos;
	delete []randsin;
	delete []randlog;
	delete []lSmpls;
	delete []lScales;
}


/* 
This function is used to change the environment map for Env map rendering.

*/
void
OgreObjectRenderWindow::setProbe(int i){

	Ogre::String frontTexture;
	Ogre::String backTexture;

	QString probeInformationPathTexture;

	// select the texture
	switch(i)
	{
	case 0: // ennis
		frontTexture = Ogre::String("ennis-front.dds");
		backTexture = Ogre::String("ennis-back.dds");
		probeInformationPathTexture = QString("../Data/textures/dualparaboloids/ennis_probe.hdr");
		break;

	case 1: // Cathedral
		frontTexture = Ogre::String("grace-front.dds");
		backTexture = Ogre::String("grace-back.dds");
		probeInformationPathTexture = QString("../Data/textures/dualparaboloids/grace_probe.hdr");
		break;

	case 2: // kitchen
		frontTexture = Ogre::String("kitchen-front.dds");
		backTexture = Ogre::String("kitchen-back.dds");
		probeInformationPathTexture = QString("../Data/textures/dualparaboloids/kitchen_probe.hdr");		
		break;
	case 3: // campus
		frontTexture = Ogre::String("campus-front.dds");
		backTexture = Ogre::String("campus-back.dds");
		probeInformationPathTexture = QString("../Data/textures/dualparaboloids/campus_probe.hdr");		
		break;
	case 4: // beach
		frontTexture = Ogre::String("beach-front.dds");
		backTexture = Ogre::String("beach-back.dds");
		probeInformationPathTexture = QString("../Data/textures/dualparaboloids/beach_probe.hdr");		
		break;
	case 5: // building
		frontTexture = Ogre::String("building-front.dds");
		backTexture = Ogre::String("building-back.dds");
		probeInformationPathTexture = QString("../Data/textures/dualparaboloids/building_probe.hdr");		
		break;
	case 6: // galileo
		frontTexture = Ogre::String("galileo-front.dds");
		backTexture = Ogre::String("galileo-back.dds");
		probeInformationPathTexture = QString("../Data/textures/dualparaboloids/galileo_probe.hdr");		
		break;
	case 7: // uffizi
		frontTexture = Ogre::String("uffizi-front.dds");
		backTexture = Ogre::String("uffizi-back.dds");
		probeInformationPathTexture = QString("../Data/textures/dualparaboloids/uffizi_probe.hdr");		
		break;
	case 8: // stpeters
		frontTexture = Ogre::String("stpeters-front.dds");
		backTexture = Ogre::String("stpeters-back.dds");
		probeInformationPathTexture = QString("../Data/textures/dualparaboloids/stpeters_probe.hdr");		
		break;


	}


	// change the textures on the sphere

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("EnvSphere");

	mat ->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	mat->getTechnique(0)->getPass(0)->createTextureUnitState(frontTexture);
	mat->getTechnique(0)->getPass(0)->createTextureUnitState(backTexture);

	Ogre::GpuProgramParametersSharedPtr gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("front",0);
	gpuParameters->setNamedConstant("back",1);



//	mat->load();

	// change the textures on the env map rendering for analytical models

	mat = Ogre::MaterialManager::getSingleton().getByName("EnvMapAnalyticBRDF");

	mat ->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	//set the texture to the material
	mat->getTechnique(0)->getPass(0)->createTextureUnitState(frontTexture);
	mat->getTechnique(0)->getPass(0)->createTextureUnitState(backTexture);

	gpuParameters = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParameters->setNamedConstant("front",0);
	gpuParameters->setNamedConstant("back",1);

	// reset the spherical harmonics parameters

	sphericalHarmonicsCoeficients(probeInformationPathTexture);

}
