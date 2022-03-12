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

#include "OgreSimulationWindow.h"

#include "imdebug.h"

using namespace std;

OgreSimulationWindow::OgreSimulationWindow(GPUSimulationBRDF *sim, Ogre::String firstIteration, Ogre::String iterateMaterial, int iterations, int resolutionSimulation, QWidget *parent, Ogre::String name):
OgreWidget(parent,name),
capturingWidth(100),
capturingHeight(100),
nIterations(iterations),
width(resolutionSimulation),
height(resolutionSimulation),
firstIterationName(firstIteration),
iterateMaterialName(iterateMaterial),
simulation(sim)

{
}

/**

Here the object and light scene nodes are created. Also, the Sphere and Plane objects are created.

*/
void 
OgreSimulationWindow::createScene(){


	// create object scene node
	mainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();



	rectangle = new Ogre::Rectangle2D(true);
	rectangle->setCorners(-1.0,1.0,1.0,-1.0);
	rectangle->setBoundingBox(Ogre::AxisAlignedBox(-100000.0 * Ogre::Vector3::UNIT_SCALE, 100000.0 * Ogre::Vector3::UNIT_SCALE));
	rectangle->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);

	mainNode->attachObject(rectangle);
	mainNode->setVisible(true);




}


void
OgreSimulationWindow::createTextures(int w, int h){
	
	// if is not required, do not reconstruct the textures
	if(width == w && height == h)
		return;

	width = w;
	height = h;

	if(width != height){
		cerr << "Warning! extrange things could happen for use a non square texture size for the simulation." << endl;
	}

	// texture creation

	// cleaning MRT's

	if(Ogre::Root::getSingleton().getRenderSystem()->getRenderTarget("MRT0") != NULL){
		Ogre::Root::getSingleton().getRenderSystem()->detachRenderTarget("MRT0");
		Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget("MRT0");
	}

	if(Ogre::Root::getSingleton().getRenderSystem()->getRenderTarget("MRT1") == NULL){
		Ogre::Root::getSingleton().getRenderSystem()->detachRenderTarget("MRT1");
		Ogre::Root::getSingleton().getRenderSystem()->destroyRenderTarget("MRT1");
	}


	if(Ogre::TextureManager::getSingleton().resourceExists("directions0")){
		Ogre::TextureManager::getSingleton().remove("directions0");
	}

	Ogre::TexturePtr mTexture0 = Ogre::TextureManager::getSingleton().createManual("directions0", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
		width, height, 0, Ogre::PF_FLOAT32_RGBA, Ogre::TU_RENDERTARGET );

	if(Ogre::TextureManager::getSingleton().resourceExists("directions1")){
		Ogre::TextureManager::getSingleton().remove("directions1");
	}

	Ogre::TexturePtr mTexture1 = Ogre::TextureManager::getSingleton().createManual("directions1", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
		width, height, 0, Ogre::PF_FLOAT32_RGBA, Ogre::TU_RENDERTARGET );


	if(Ogre::TextureManager::getSingleton().resourceExists("hits0")){
		Ogre::TextureManager::getSingleton().remove("hits0");
	}

	Ogre::TexturePtr mTexture2 = Ogre::TextureManager::getSingleton().createManual("hits0", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
		width, height, 0, Ogre::PF_FLOAT32_RGBA, Ogre::TU_RENDERTARGET );

	if(Ogre::TextureManager::getSingleton().resourceExists("hits1")){
		Ogre::TextureManager::getSingleton().remove("hits1");
	}

	Ogre::TexturePtr mTexture3 = Ogre::TextureManager::getSingleton().createManual("hits1", 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
		width, height, 0, Ogre::PF_FLOAT32_RGBA, Ogre::TU_RENDERTARGET );


	Ogre::RenderTexture* rt0 = mTexture0->getBuffer()->getRenderTarget();
	Ogre::RenderTexture* rt1 = mTexture1->getBuffer()->getRenderTarget();
	Ogre::RenderTexture* rt2 = mTexture2->getBuffer()->getRenderTarget();
	Ogre::RenderTexture* rt3 = mTexture3->getBuffer()->getRenderTarget();


	rt0->setAutoUpdated(false); //directions0
	rt1->setAutoUpdated(false); //directions1
	rt2->setAutoUpdated(false); //hits0
	rt3->setAutoUpdated(false); //hits1


	//create MRT -> first iteration

	rttTex = Ogre::Root::getSingleton().getRenderSystem()->createMultiRenderTarget("MRT0");

	rttTex->bindSurface(0, rt0);
	rttTex->bindSurface(1, rt2);
	rttTex->setAutoUpdated( false );

	// create MRT -> second iteration
	rttTex1 = Ogre::Root::getSingleton().getRenderSystem()->createMultiRenderTarget("MRT1");

	rttTex1->bindSurface(0, rt1);
	rttTex1->bindSurface(1, rt3);
	rttTex1->setAutoUpdated( false );
	


	mViewport = rttTex->addViewport( mCamera );
	mViewport->setBackgroundColour( Ogre::ColourValue( 0,0,0 ) );
	mViewport->setClearEveryFrame(true);
	mViewport->setOverlaysEnabled(false);

	mViewport = rttTex1->addViewport( mCamera );	
	mViewport->setBackgroundColour( Ogre::ColourValue( 0,0,0 ) );
	mViewport->setClearEveryFrame(true);
	mViewport->setOverlaysEnabled(false);



}




void 
OgreSimulationWindow::initializeGL()
{

	// TODO: remove this little cheat!
	int w = width;
	width = -1;


//	rttTex = Ogre::Root::getSingleton().getRenderSystem()->createMultiRenderTarget("MRT0");
//	rttTex1 = Ogre::Root::getSingleton().getRenderSystem()->createMultiRenderTarget("MRT1");


/*
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("FirstHeightFieldSimulation");

	mat->getTechnique(0)->getPass(0)->setTextureFiltering(Ogre::TFO_NONE);

	mat = Ogre::MaterialManager::getSingleton().getByName("IterationHeightFieldSimulation");

	mat->getTechnique(0)->getPass(0)->setTextureFiltering(Ogre::TFO_NONE);
*/

  //== Ogre Initialization ==//
  mSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC,(windowName + Ogre::String("SceneManager")) );
  
  mCamera = mSceneMgr->createCamera( "QOgreWidget_Cam" );

  mCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);


  mCamera->setAspectRatio(1);


  createTextures(w, w);
}

void
OgreSimulationWindow::debugTexture(Ogre::String name){

	Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().getByName(name);
	
	Ogre::HardwarePixelBufferSharedPtr hpb = tex->getBuffer(0,0);

	// Allocate space to store the texture in memory
	float *tex01 = new float[tex->getHeight()*tex->getWidth()*4];
	
	// Creating a pixelbox with correct the format for the texture...
	Ogre::PixelBox pBox(tex->getWidth(),tex->getHeight(),tex->getDepth(),Ogre::PF_FLOAT32_RGBA,tex01);
	
	// Copying the texture from the GPU to the CPU
	hpb->blitToMemory(pBox);

	imdebug("rgba b=32f w=%d h=%d %p", tex->getWidth(), tex->getHeight(), tex01);

	delete []tex01;
}

float*
OgreSimulationWindow::getTexture(Ogre::String name){

	Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().getByName(name);
	
	Ogre::HardwarePixelBufferSharedPtr hpb = tex->getBuffer(0,0);

	// Allocate space to store the texture in memory
	float *tex01 = new float[tex->getHeight()*tex->getWidth()*4];
	
	// Creating a pixelbox with correct the format for the texture...
	Ogre::PixelBox pBox(tex->getWidth(),tex->getHeight(),tex->getDepth(),Ogre::PF_FLOAT32_RGBA,tex01);
	
	// Copying the texture from the GPU to the CPU
	hpb->blitToMemory(pBox);

	return tex01;
}




float*
OgreSimulationWindow::compute(Ogre::Vector2 incidentVector){

	// first iteration -> fixed, given an incoming direction, the outgoing is computed

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(firstIterationName);//"FirstHeightFieldSimulation"

	mat->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();
	
	simulation->setMaterialInfo(firstIterationName);

	/// obtain gpu vertex shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	Ogre::Vector3 eyeDirection(BRDF::spherical2cart(incidentVector));

	cout << "eye Direction on simulation: " << -eyeDirection << endl;

	gpuParam->setNamedConstant("eyeDirection",eyeDirection);

	//update the MRT
	rectangle->setMaterial(firstIterationName);//"FirstHeightFieldSimulation"
	rttTex -> update();

//	debugTexture("directions0");
//	debugTexture("hits0");

//	return r;

	for(int i=0; i < nIterations; i++){

		Ogre::String dname;
		Ogre::String hname;

		if(i%2 == 0){
			dname = Ogre::String("directions0");
			hname = Ogre::String("hits0");
		}else{
			dname = Ogre::String("directions1");
			hname = Ogre::String("hits1");
		}
		
		/*
		cout << "iteracio: " << i << endl;
		cout << "entrada: " << dname << endl;
		cout << "entrada: " << hname << endl;
		cout << "---------------------------------------------------------------" << endl;
		*/

		mat = Ogre::MaterialManager::getSingleton().getByName(iterateMaterialName);//"IterationHeightFieldSimulation");

		mat->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

		// set the material texture information
		int texNumber = simulation->setMaterialInfo(iterateMaterialName);

		Ogre::TextureUnitState *tus = mat->getTechnique(0)->getPass(0)->createTextureUnitState(dname);
		tus->setTextureFiltering(Ogre::TFO_NONE);
		tus = mat->getTechnique(0)->getPass(0)->createTextureUnitState(hname);
		tus->setTextureFiltering(Ogre::TFO_NONE);

		
		gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

		gpuParam->setNamedConstant("directions",texNumber++);
		gpuParam->setNamedConstant("hits",texNumber++);

		rectangle->setMaterial(iterateMaterialName);//"IterationHeightFieldSimulation");

		if(i%2 == 0.){
			rttTex1->update();
			//debugTexture("directions1");
	//		debugTexture("hits1");
		}else{
			rttTex->update();
			//debugTexture("directions0");
	//		debugTexture("hits0");
		}
		

		
	//	debugTexture("hits1");
		}

	if(nIterations-1%2 == 0.){
		//debugTexture("directions1");
		return getTexture("directions1");
	}else{
		//debugTexture("directions0");
		return getTexture("directions0");
	}	
}




float*
OgreSimulationWindow::processTexture(float* directionsTexture,  int &x, int &y, float &min, float &max ){

	y = x;
	capturingHeight = y;
	capturingWidth = x;//*4;
	x = capturingWidth;

	

	// size of the texture float3*width*heght
	int nDirections = width*height;
	int directionSize = 4;

	int outTextureSize = capturingWidth*capturingHeight;
	float *outTexture = new float[outTextureSize];

	for(int i=0; i<outTextureSize; i++){
		outTexture[i] = 0.;
	}

	int notFinished = 0;
	int goingDown = 0;

	for(int i=0; i<nDirections; i++){
			
		if(directionsTexture[i*directionSize + 3] != 0.){

			Ogre::Vector3 dir(directionsTexture[i*directionSize + 0],directionsTexture[i*directionSize + 1],directionsTexture[i*directionSize + 2]);

			if(dir[2] > 0){ // take care of the directions that come out of the relief plane
				// phi, theta
				Ogre::Vector2 uv = BRDF::cart2spherical(dir);
			

				if(uv[1] > Ogre::Math::PI/2) // if this is true, some direction comes down the surface!!!
					uv[1] = Ogre::Math::PI/2.;
		//			cout << "lalala" << endl;

				uv[0] = Ogre::Math::IFloor(uv[0]/(2.*Ogre::Math::PI)*capturingWidth);//phi
				uv[1] = Ogre::Math::IFloor((uv[1]/(Ogre::Math::PI/2.))*capturingHeight);//theta


				int position = capturingWidth*uv[1] + uv[0];

				outTexture[position]++;
			}else{
				goingDown++;
			}

		}else{
			notFinished++;
		}
	}


	cout << "Rays going down: " << ((float)goingDown)/((float)nDirections)*100 << "%" << endl;
	cout << "Rays not finished: " << ((float)notFinished)/((float)nDirections)*100 << "%" << endl;

	int theta_0 = 0;

	// set the first line to the sum of all the positions on the first line -> theta = 0.
	for(int i=0; i<capturingWidth; i++){
		theta_0 += outTexture[i];
	}

	theta_0 /= capturingWidth;

	for(int i=0; i<capturingWidth; i++){
		outTexture[i] = theta_0;
	}

	float sangle = 0.;

	for(int i=0; i<outTextureSize; i++){
		
		float theta = get2DCoord( i, capturingWidth)[1];

		//solid angle correction term
		float solidAngle = sin(theta)*(Ogre::Math::PI*2/capturingWidth)*(Ogre::Math::PI/2/capturingWidth);

		sangle += solidAngle;

		float cosTheta = cos(theta);


		// normalize the shots on each texture position. Dividing by the number of rays shot, the solid angle of the patch and cos(outgoingtheta)
		outTexture[i] /= (nDirections*solidAngle*cosTheta);
	
		// find out the maximum, for the transfer function on the displaying.
		if(outTexture[i] > max)
			max = outTexture[i];
	}

	return outTexture;

	cout << "total solid angle: " << sangle << endl;
/*
	Ogre::String intensityTextureName("IntensityTexture");

	if(Ogre::TextureManager::getSingleton().resourceExists(intensityTextureName)){
		Ogre::TextureManager::getSingleton().remove(intensityTextureName);
	}


// create Texture.
	Ogre::DataStreamPtr indirectSteam = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(outTexture,x * y * sizeof(float)));
	
	Ogre::Image *intensityImage = new Ogre::Image();
	
	intensityImage->loadRawData(indirectSteam,x,y,1,Ogre::PF_FLOAT32_R,1,0);


	Ogre::TextureManager::getSingleton().loadImage(intensityTextureName,
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		*intensityImage,
		Ogre::TEX_TYPE_2D,
		0,
		1.0f,
		true,
		Ogre::PF_FLOAT32_R);


	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("ScatmechBRDF");

	/// clean all the textures on the material.
	mat ->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	/// set the hash texture to the material
	Ogre::TextureUnitState *texUnitState =  mat->getTechnique(0)->getPass(0)->createTextureUnitState(intensityTextureName);

	/// disable filtering on both textures
	texUnitState->setTextureFiltering(Ogre::TFO_BILINEAR );
//	texUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_WRAP);
//	texUnitState->setTextureFiltering(Ogre::FT_MAG,Ogre::TFO_BILINEAR );

	/// obtain gpu vertex shader parameters
	Ogre::GpuProgramParametersSharedPtr gpuVertexParam = mat->getTechnique(0)->getPass(0)->getVertexProgramParameters();

	gpuVertexParam->setNamedConstant("min",(Ogre::Real)0.);
	gpuVertexParam->setNamedConstant("max",(Ogre::Real)max);
	gpuVertexParam->setNamedConstant("thetaSize",(Ogre::Real)y);



//	imdebug("r w=%d h=%d %p  b=32f", x, y, intensitys);

	delete []outTexture;
	*/
}

Ogre::Vector2
OgreSimulationWindow::get2DCoord( float pos, float texwidth )
{
	Ogre::Vector2 uv;

	// Rectangular coordinate => ( 0..texwidth, 0..texwidth )
	uv[1] = floor( pos / texwidth );
	uv[0] = pos - texwidth * uv[1];		// pos % texwidth


	// Convert to uniform space / normalize => ( 0..1, 0..1 )
	uv = ( uv + 0.5 ) / texwidth;

	return uv;
}



Ogre::Vector2
OgreSimulationWindow::get2DCoordCorner( float pos, float texwidth )
{
	Ogre::Vector2 uv;

	// Rectangular coordinate => ( 0..texwidth, 0..texwidth )
	uv[1] = floor( pos / texwidth );
	uv[0] = pos - texwidth * uv[1];		// pos % texwidth


	// Convert to uniform space / normalize => ( 0..1, 0..1 )
	uv = uv / texwidth;

	return uv;
}

void
OgreSimulationWindow::mipmapping(){


	//agafar textura inicial.
	QString texName = QFileDialog::getOpenFileName(this,
		tr("Open texture"),"../Data/textures/relief", tr("Texture files (*.dds)"));

	//("four_NormHeight.dds");

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



	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().getByName(texName.toStdString());

	cout << "tex resolution: " << texture->getWidth() << " "  << texture->getHeight() << endl;

	Ogre::HardwarePixelBufferSharedPtr hpb = texture->getBuffer(0,0);
		
	// Allocate space to store the texture in memory
	float *tex0 = new float[texture->getHeight()*texture->getWidth()*4];
	
	// Creating a pixelbox with correct the format for the texture...
	Ogre::PixelBox pBox(texture->getWidth(),texture->getHeight(),texture->getDepth(),Ogre::PF_FLOAT32_RGBA,tex0);
	
	// Copying the texture from the GPU to the CPU
	hpb->blitToMemory(pBox);

//	imdebug("rgba b=32f w=%d h=%d %p", texture->getWidth(), texture->getHeight(), tex0);

	

//	createLevel(tex0,texture->getWidth());

	delete []tex0;

/*
	float *level1 = new float[texture->getWidth()* texture->getHeight()*4/4];

	for(int i=0; i<texture->getWidth()/2; i++){
		for(int j=0; j<texture->getHeight()/2; j++){	
			level1[i*texture->getWidth()/2+ = 
		}
	}
*/



	



	// a partir del seu tamany, fer el mipmapping.







	int size = 100*100*4/4+100*100*4+100*100*4/16;



	float *prova = new float[size];

	for(int i=0; i<size/4; i++){

		if( i < 100*100){
			prova[i*4+0] = 1.;
			prova[i*4+1] = 0.;
			prova[i*4+2] = 0.;
			prova[i*4+3] = 1.;
		}else if (i <100*100+100*100/4){
			prova[i*4+0] = 0.;
			prova[i*4+1] = 1.;
			prova[i*4+2] = 0.;
			prova[i*4+3] = 1.;
		}else{
			prova[i*4+0] = 0.;
			prova[i*4+1] = 0.;
			prova[i*4+2] = 1.;
			prova[i*4+3] = 1.;


		}
	}

	Ogre::DataStreamPtr bTex = Ogre::DataStreamPtr( new Ogre::MemoryDataStream(prova,size*sizeof(float)));
	Ogre::Image *bTexImage = new Ogre::Image();
	bTexImage->loadRawData(bTex,100,100,1,Ogre::PF_FLOAT32_RGBA ,1,2);
	cout << "num mipmaps: " << bTexImage->getNumMipmaps() << endl;
	Ogre::TextureManager::getSingleton().loadImage("provaprova",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*bTexImage,Ogre::TEX_TYPE_2D,2,1.0f,false,Ogre::PF_FLOAT32_RGBA);


	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("test");

	/// clean all the textures on the material.
	mat ->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	/// set the hash texture to the material
	Ogre::TextureUnitState *texUnitState =  mat->getTechnique(0)->getPass(0)->createTextureUnitState("provaprova");

	/// disable filtering on both textures
	texUnitState->setTextureFiltering(Ogre::TFO_BILINEAR );

	delete []prova;


}


