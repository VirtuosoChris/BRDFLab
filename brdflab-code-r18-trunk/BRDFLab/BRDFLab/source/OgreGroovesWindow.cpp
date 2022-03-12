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

#include "OgreGroovesWindow.h"

#include "imdebug.h"

using namespace std;

OgreGroovesWindow::OgreGroovesWindow(Ogre::String textureName, QWidget *parent, Ogre::String name):
OgreWidget(parent,name),
texName(textureName),
width(1000),
height(1000),
rt0(NULL)

/*
nIterations(iterations),
*/
{
}

/**

Here the object and light scene nodes are created. Also, the Sphere and Plane objects are created.

*/
void 
OgreGroovesWindow::createScene(){


	// create object scene node
	mainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();






	

	//Ogre::Entity* mainEntity = mSceneMgr->createEntity("ObjectRenderingEntity","sphere.mesh");
	//mainEntity->setMaterialName("testingMaterial");
	//mainNode->attachObject(mainEntity);



/*
	Ogre::Rectangle2D *rectangle = new Ogre::Rectangle2D(true);
	rectangle->setCorners(-1.0,1.0,1.0,-1.0);
	rectangle->setBoundingBox(Ogre::AxisAlignedBox(-100000.0 * Ogre::Vector3::UNIT_SCALE, 100000.0 * Ogre::Vector3::UNIT_SCALE));
	rectangle->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);

	rectangle->setMaterial("red");

	mainNode->attachObject(rectangle);
*/
	mainNode->setVisible(true);



}


void 
OgreGroovesWindow::initializeGL()
{


		
  //== Ogre Initialization ==//
  mSceneMgr = mOgreRoot->createSceneManager(Ogre::ST_GENERIC,(windowName + Ogre::String("SceneManager")) );
  
  mCamera = mSceneMgr->createCamera( "QOgreWidget_Cam" );

  mCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
  
  mCamera->setOrthoWindow(1.,1.);

  mCamera->setNearClipDistance(0.001);
  mCamera->setFarClipDistance(10.);
  mCamera->setPosition(Ogre::Vector3(0.5,0.5,1.));
  mCamera->setFOVy(Ogre::Radian(Ogre::Degree(45.0f)));


//  mCamera->setOrientation(result);
//  mCamera->lookAt(Ogre::Vector3(0.5,0.5,1.));
  
/*
  mViewport = rt0->addViewport( mCamera );
  mViewport->setBackgroundColour( Ogre::ColourValue( 0,0,0 ) );
  mViewport->setClearEveryFrame(true);
  mViewport->setOverlaysEnabled(false);
*/
//  mCamera->setAspectRatio(1);

//  cout << "direction: " << mCamera->getDirection() << endl;
//  cout << "positoin: " << mCamera->getPosition() << endl;

}


void
OgreGroovesWindow::createGroovesTexture(double grooveWidth , double grooveSpacing , double grooveBotAngle , double groovesAngle ,int numGroovesPerSide, int numTexelsPerGroove){
/*
		// Inputs
	double grooveWidth = 0.004;		// groove width in inches
	double grooveSpacing = 0.02;	// space between the grooves in inches (between their centers/pitch)
	double grooveBotAngle = 60;		// groove bottom angle in degrees
	double groovesAngle = 90;		// angle between the grooves in degrees

	// Quality-related params
	unsigned numGroovesPerSide = 3;		// number of grooves for each side
	unsigned numTexelsPerGroove = 30;	// number of texels covered by the groove (by its width)
*/
	// Conversion factors
//	double inch2mm = 25.4;			// from inches to millimetres
	double deg2rad = Ogre::Math::PI / 180.;	// from degrees to radians

	// Compute other required parameters
	double grooveDepth = grooveWidth * 0.5 / tan(grooveBotAngle * deg2rad * 0.5); // tan(a) = w/h --> h = w / tan(a)
	double sampleSize = numGroovesPerSide * grooveSpacing;		// sample size in inches
	double inch2tex = 1 / sampleSize;	// from inches to uniform texture space
	double texelsPerInch = numTexelsPerGroove / grooveWidth;
	unsigned mapRes = (unsigned)(sampleSize * texelsPerInch);	// sample size in texels (relief map resolution)

	width = mapRes;
	height = mapRes;
	

//	cout << "mapRes=" << mapRes << endl;

	// Profile (simple groove)
	const unsigned pfPoints = 3;
	Ogre::Vector4 pf[pfPoints];

	pf[0] = Ogre::Vector4( -grooveWidth * 0.5 * inch2tex, 0., 0., -grooveDepth * inch2tex ) ;
	pf[1] = Ogre::Vector4( grooveWidth * 0.5 * inch2tex, 0., 0.,0.);
//	pf[2] = Ogre::Vector3( grooveWidth * 0.5 * inch2tex, 0., 0. );


	// set background to surface normal


	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("GenerateGrooves");

//	mat->setDepthFunction(Ogre::CMPF_GREATER);
	// mirar el clear a 0.


	Ogre::GpuProgramParametersSharedPtr gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParam->setNamedConstant("npoints", (Ogre::Real)pfPoints);
	gpuParam->setNamedConstant("prof1", pf[0]);
	gpuParam->setNamedConstant("prof2", pf[1]);
//	gpuParam->setNamedConstant("prof3", pf[2]);


	float drawWidth = grooveWidth * 0.5 * inch2tex;		// half groove width in texture space
	float dist = 1.0f / numGroovesPerSide;			// distance between grooves in texture space

	Ogre::Vector2 binormal(1., 0.);


	// clean previous manual objects
	mSceneMgr->destroyAllManualObjects();
	
	for ( unsigned s = 0; s < 2; ++s )
	{
		Ogre::SceneNode *node = mainNode->createChildSceneNode();

		Ogre::ManualObject *tri = mSceneMgr->createManualObject("tri"+Ogre::StringConverter::toString(s)); 

		// Rotate grooves (for the second set)
		if ( s == 1 )
		{
		//	node->translate(0.5,0.5,0.);
			node->rotate(Ogre::Vector3(0.,0.,1.),Ogre::Radian(Ogre::Degree(groovesAngle)));
		//	node->translate(-0.5,-0.5,0.);
			binormal[0] = cos(Ogre::Math::DegreesToRadians((Ogre::Real)groovesAngle));
			binormal[1] = sin(Ogre::Math::DegreesToRadians((Ogre::Real)groovesAngle));
		}

		
		tri -> begin("GenerateGrooves", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		for ( float x = - dist * 0.5; x < 1.f + dist; x += dist )
		{
			
			// triangle 1
			
			// C
			tri->textureCoord(-drawWidth, binormal[0], binormal[1]);
			tri->position(x - drawWidth, 5., 0.);
			
			// B
			tri->textureCoord(drawWidth, binormal[0], binormal[1]);
			tri->position(x + drawWidth, -5, 0.);

			// D
			tri->textureCoord(drawWidth, binormal[0], binormal[1]);
			tri->position(x + drawWidth, 5., 0.);

			// triangle 2
			
			// C
			tri->textureCoord(-drawWidth, binormal[0], binormal[1]);
			tri->position(x - drawWidth, 5., 0.);
			
			// A
			tri->textureCoord(-drawWidth, binormal[0], binormal[1]);
			tri->position(x - drawWidth, -5., 0.);

			// B
			tri->textureCoord(drawWidth, binormal[0], binormal[1]);
			tri->position(x + drawWidth, -5., 0.);
		}
		
		tri->end();
		
		node->attachObject(tri);

	}
	

	// texture creation
	if(Ogre::TextureManager::getSingleton().resourceExists(texName)){
		Ogre::TextureManager::getSingleton().remove(texName);
	}

	Ogre::TexturePtr mTexture0 = Ogre::TextureManager::getSingleton().createManual(texName, 
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
		width, height, 0, Ogre::PF_FLOAT32_RGBA, Ogre::TU_RENDERTARGET );

	rt0 = mTexture0->getBuffer()->getRenderTarget();

	rt0->setAutoUpdated(false); //directions0


	// setup viewport
	mViewport = rt0->addViewport( mCamera );
	mViewport->setBackgroundColour(Ogre::ColourValue(0.5,0.5,1.,1.));

//	mViewport->setBackgroundColour( Ogre::ColourValue( 0,0,0 ) );
	mViewport->setClearEveryFrame(true);
	mViewport->setOverlaysEnabled(false);


	// update texture
	rt0->update();
	//debugTexture(texName);


}


void
OgreGroovesWindow::debugTexture(Ogre::String name){

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
