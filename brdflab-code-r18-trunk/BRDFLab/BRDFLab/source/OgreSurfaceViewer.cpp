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

#include "OgreSurfaceViewer.h"

#include "imdebug.h"

using namespace std;

OgreSurfaceViewer::OgreSurfaceViewer(QWidget *parent, Ogre::String name):
OgreWidget(parent,name)
{

	center = Ogre::Vector3(0.5,0.5,0.);
	zoomFactor = 0.2;
}

/**

Here the object and light scene nodes are created. Also, the Sphere and Plane objects are created.

*/
void 
OgreSurfaceViewer::createScene(){

	mCamera->setNearClipDistance(0.01);

	// create object scene node
	mainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	//createHemiSphere(5);
	createSphere("SphereP",10,10,10);


	// create light scene node
	lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	Ogre::Light* l = mSceneMgr->createLight("MainLight");
	lightNode->attachObject(l);


	Ogre::Entity *lightBall = mSceneMgr->createEntity("lightBallP","SphereP");
	lightBall->setMaterialName("white");
	lightNode->attachObject(lightBall);
	lightNode->setScale(Ogre::Vector3(0.01,0.01,0.01));

	lightNode->translate(Ogre::Vector3(0,0,10));


/*

	Ogre::Entity *lightBall = mSceneMgr->createEntity("lightBallP","SphereP");
	lightBall->setMaterialName("white");
	lightNode->attachObject(lightBall);

	lightNode->translate(Ogre::Vector3(0,100,0));

	Ogre::Plane *p = new Ogre::Plane(Ogre::Vector3(0.,1.,0.), 0.);
	Ogre::MeshPtr m = Ogre::MeshManager::getSingleton().createPlane("PlaneRelief",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,*p,200,200,1,1,true,1,1.0f,1.0f,Ogre::Vector3::UNIT_Z,Ogre::HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY,Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, true,true );
	m->buildTangentVectors(Ogre::VES_TANGENT);

	Ogre::Entity *mainEntity = mSceneMgr->createEntity("PlaneReliefRendering","PlaneRelief");
	mainEntity->setMaterialName("reliefMappingViewer");
	mainNode->attachObject(mainEntity);

*/




//	paintAxis();
//	startTimer(0);

}

void 
OgreSurfaceViewer::setInitialCameraPosition(){

	mouseRotation[0] = 119;
	mouseRotation[1] = 222;
	cameraZoom = 10;

}


/*

void 
OgreSurfaceViewer::setUpAxis(){


	upAxis = Ogre::Vector3::UNIT_Z;
}

*/
void
OgreSurfaceViewer::display(GPUSimulationBRDF *sim){

	

}

void 
OgreSurfaceViewer::displayFloor(Ogre::String materialName){

	// plane 
	if(!mSceneMgr->hasManualObject("floor")){
		
		Ogre::ManualObject *plane = mSceneMgr->createManualObject("floor"); 

		plane -> begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);	

			//plane->position(0.,0.,0.);
			//plane->position(1.,0.,0.);
			//plane->position(0.,1.,0.);

			//plane->position(0.,1.,0.);
			//plane->position(1.,0.,0.);
			//plane->position(1.,1.,0.);

			plane->position(0.,1.,0.);
			plane->position(0.,0.,0.);
			plane->position(1.,0.,0.);

			plane->position(0.,1.,0.);
			plane->position(1.,0.,0.);
			plane->position(1.,1.,0.);

		plane->end();

		mainNode->attachObject(plane);
	}
}

void
OgreSurfaceViewer::displayBox(Ogre::String materialName){

	if(!mSceneMgr->hasManualObject("SimulationBox")){

		Ogre::ManualObject *box = mSceneMgr->createManualObject("wireBox"); 

		box -> begin("white", Ogre::RenderOperation::OT_LINE_LIST);	
		
			//upper
			box -> position(Ogre::Vector3(0.,0.,1.));
			box -> position(Ogre::Vector3(0.,1.,1.));

			box -> position(Ogre::Vector3(0.,0.,1.));
			box -> position(Ogre::Vector3(1.,0.,1.));

			box -> position(Ogre::Vector3(0.,1.,1.));
			box -> position(Ogre::Vector3(1.,1.,1.));

			box -> position(Ogre::Vector3(1.,0.,1.));
			box -> position(Ogre::Vector3(1.,1.,1.));

			// lower
			box -> position(Ogre::Vector3(0.,0.,0.));
			box -> position(Ogre::Vector3(0.,1.,0.));

			box -> position(Ogre::Vector3(0.,0.,0.));
			box -> position(Ogre::Vector3(1.,0.,0.));

			box -> position(Ogre::Vector3(0.,1.,0.));
			box -> position(Ogre::Vector3(1.,1.,0.));

			box -> position(Ogre::Vector3(1.,0.,0.));
			box -> position(Ogre::Vector3(1.,1.,0.));

			// vertical

			box -> position(Ogre::Vector3(0.,0.,1.));
			box -> position(Ogre::Vector3(0.,0.,0.));

			box -> position(Ogre::Vector3(1.,0.,1.));
			box -> position(Ogre::Vector3(1.,0.,0.));

			box -> position(Ogre::Vector3(0.,1.,1.));
			box -> position(Ogre::Vector3(0.,1.,0.));

			box -> position(Ogre::Vector3(1.,1.,1.));
			box -> position(Ogre::Vector3(1.,1.,0.));

		box->end();

		Ogre::SceneNode *wireBox = mainNode->createChildSceneNode();
		wireBox->attachObject(box);
		wireBox->setScale(1.001,1.001,1.001);

		// box

		box = mSceneMgr->createManualObject("SimulationBox"); 

		box -> begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);	
		
			//upper
			box -> position(Ogre::Vector3(0.,0.,0.));
			box -> position(Ogre::Vector3(0.,1.,1.));
			box -> position(Ogre::Vector3(0.,1.,0.));

			box -> position(Ogre::Vector3(0.,0.,0.));
			box -> position(Ogre::Vector3(0.,0.,1.));
			box -> position(Ogre::Vector3(0.,1.,1.));

			box -> position(Ogre::Vector3(0.,0.,0.));
			box -> position(Ogre::Vector3(1.,0.,0.));
			box -> position(Ogre::Vector3(1.,0.,1.));

			box -> position(Ogre::Vector3(0.,0.,0.));
			box -> position(Ogre::Vector3(1.,0.,1.));
			box -> position(Ogre::Vector3(0.,0.,1.));

			box -> position(Ogre::Vector3(0.,0.,1.));
			box -> position(Ogre::Vector3(1.,0.,1.));
			box -> position(Ogre::Vector3(1.,1.,1.));

			box -> position(Ogre::Vector3(0.,0.,1.));
			box -> position(Ogre::Vector3(1.,1.,1.));
			box -> position(Ogre::Vector3(0.,1.,1.));

			box -> position(Ogre::Vector3(1.,0.,0.));
			box -> position(Ogre::Vector3(1.,1.,0.));
			box -> position(Ogre::Vector3(1.,1.,1.));

			box -> position(Ogre::Vector3(1.,0.,0.));
			box -> position(Ogre::Vector3(1.,1.,1.));
			box -> position(Ogre::Vector3(1.,0.,1.));

			box -> position(Ogre::Vector3(1.,1.,0.));
			box -> position(Ogre::Vector3(0.,1.,0.));
			box -> position(Ogre::Vector3(0.,1.,1.));

			box -> position(Ogre::Vector3(1.,1.,0.));
			box -> position(Ogre::Vector3(0.,1.,1.));
			box -> position(Ogre::Vector3(1.,1.,1.));

			box -> position(Ogre::Vector3(0.,0.,0.));
			box -> position(Ogre::Vector3(0.,1.,0.));
			box -> position(Ogre::Vector3(1.,0.,0.));

			box -> position(Ogre::Vector3(1.,0.,0.));
			box -> position(Ogre::Vector3(0.,1.,0.));
			box -> position(Ogre::Vector3(1.,1.,0.));
			
		box->end();

		mainNode->attachObject(box);

	}
}



void 
OgreSurfaceViewer::displayRelief(Ogre::String textureName){

	displayFloor("reliefMappingViewer");

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("reliefMappingViewer");

	mat->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	Ogre::TextureUnitState *heightTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState(textureName);
	heightTexture->setTextureFiltering(Ogre::TFO_BILINEAR);

	Ogre::GpuProgramParametersSharedPtr gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	
	gpuParam->setNamedConstant("tilt",1);
	gpuParam->setNamedConstant("depth",(Ogre::Real)1.);

	center = Ogre::Vector3(0.5,0.5,0.);
	cameraZoom = 10;

	this->updateCamera();
}

void
OgreSurfaceViewer::displaySpheres(SpheresBRDF *sph){

	displayBox("spheresViewer");
	displayFloor("white");

	
//	killTimer(timerId);
	
/*
	Ogre::String materialName("spheresViewer");

	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(materialName);

	Ogre::TextureUnitState *heightTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("spheresData");
	heightTexture->setTextureFiltering(Ogre::TFO_NONE);

	Ogre::GpuProgramParametersSharedPtr gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParam->setNamedConstant("texWidth",(Ogre::Real)sph->getTexWidth());
	gpuParam->setNamedConstant("nSpheres",sph->getNSpheres());

	gpuParam->setNamedConstant("data",0);
*/
	center = Ogre::Vector3(0.5,0.5,0.5);
	
	this->updateCamera();


//	timerId = startTimer(50);

}


void
createMesh(int nTriangles, int nVertex, int *index, Ogre::Vector3 *vertex, Ogre::Vector3 *normal){
	
	Ogre::String meshName("SurfaceMicrostructure");

	// delete the previous instance if exists.
	if(Ogre::MeshManager::getSingleton().resourceExists(meshName)){
		Ogre::MeshManager::getSingleton().remove(meshName);
	}
	
	Ogre::MeshPtr msh = Ogre::MeshManager::getSingleton().createManual(meshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	/// Create one submesh
	Ogre::SubMesh* sub = msh->createSubMesh();

	/// Define the vertices (3 floats per vertex + 1 float per texCoord).
	const size_t nVertices = nVertex;
	const size_t vbufCount = 6*nVertices;
	float *vertices = new float[vbufCount];

	for(unsigned int i=0; i<nVertices; i++){

		vertices[i*6+0] = vertex[i][0]; // X coord
		vertices[i*6+1] = vertex[i][1]; // Y coord
		vertices[i*6+2] = vertex[i][2]; // Z coord

											 // Normals...
		vertices[i*6+3] = normal[i][0]; // X coord 
		vertices[i*6+4] = normal[i][1]; // Y coord
		vertices[i*6+5] = normal[i][2]; // Z coord
	}

	/// Define triangles
	/// The values in this table refer to vertices in the above table
	const size_t ibufCount = nTriangles*3;
	unsigned long *faces = new unsigned long[ibufCount];


	for(unsigned int i=0; i<nTriangles; i++){
		faces[i*3] = (unsigned long)index[i*3];
		faces[i*3+1] = (unsigned long)index[i*3+1];
		faces[i*3+2] = (unsigned long)index[i*3+2];
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

	/// Notify Mesh object that it has been loaded
	msh->load();

	Ogre::MeshSerializer store;
    store.exportMesh(msh.get(), "subdividedQuad.mesh");


}




void 
OgreSurfaceViewer::displayTriangles(TrianglesBRDF *tri){



	int *index = tri->getIndex();
	Ogre::Vector3 *vertex = tri->getVertex();
	int nVertex = tri->getNVertex();
	int nTriangles = tri->getNTriangles();
	Ogre::Vector3 *normal =  tri->getVertexNormals();
	
	createMesh(nTriangles, nVertex, index, vertex, normal);

	
	if(mSceneMgr->hasEntity("triangleDisplay"))
		mSceneMgr->destroyEntity("triangleDisplay");

	Ogre::Entity *triMesh = mSceneMgr->createEntity("triangleDisplay","SurfaceMicrostructure");
	triMesh->setMaterialName("GouraudShading");

	mainNode->attachObject(triMesh);


	if(mSceneMgr->hasManualObject("box"))
		mSceneMgr->destroyManualObject("box");

	Ogre::ManualObject *box = mSceneMgr->createManualObject("box"); 

	box -> begin("white", Ogre::RenderOperation::OT_LINE_LIST);	

	box->position(0,0,0);
	box->position(1,0,0);

	box->position(0,0,0);
	box->position(0,1,0);

	box->position(1,0,0);
	box->position(1,1,0);

	box->position(1,1,0);
	box->position(0,1,0);
	box->end();

	mainNode->attachObject(box);









	/*
	displayBox("trianglesViewer");

	int width = tri->getWidth();
	float maxHeight = tri->getMaxHeight();
	int indexTexWidth = tri->getIndexTexWidth();
	int nTriangles = tri->getNTriangles();

	// set material info
	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("trianglesViewer");

	mat->getTechnique(0)->getPass(0)->removeAllTextureUnitStates();

	Ogre::TextureUnitState *vertexTexturePtr = mat->getTechnique(0)->getPass(0)->createTextureUnitState("vertexTexture");
	vertexTexturePtr->setTextureFiltering(Ogre::TFO_NONE);

	Ogre::TextureUnitState *indexTexturePtr = mat->getTechnique(0)->getPass(0)->createTextureUnitState("indexTexture");
	indexTexturePtr->setTextureFiltering(Ogre::TFO_NONE);

	Ogre::GpuProgramParametersSharedPtr gpuParam = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

	gpuParam->setNamedConstant("vertexWidth",(Ogre::Real)width);
//	cout << "vertexWidth: " << width << endl;
	gpuParam->setNamedConstant("indexWidth",(Ogre::Real)indexTexWidth);
//	cout << "indexWidth: " << indexTexWidth << endl;
	gpuParam->setNamedConstant("nTriangles",(Ogre::Real)nTriangles);
//	cout << "nTriangles: " << nTriangles << endl;
	gpuParam->setNamedConstant("maxHeight",(Ogre::Real)maxHeight);
	

	gpuParam->setNamedConstant("width",width-1);
	gpuParam->setNamedConstant("height",width-1);
	
	gpuParam->setNamedConstant("vertexTexture",0);
	gpuParam->setNamedConstant("indexTexture",1);


	mat->reload();

	// scale the main node to 1.,1.,maxHeight
	mainNode->setScale(0.99,0.99,maxHeight);
*/

	// set the camera lookat ~center to 0.5, 0.5, maxHeight/2.
	center = Ogre::Vector3(0.5,0.5,tri->getMaxHeight()/2.);
	
	this->updateCamera();
	zoomFactor = 0.02;
	update();

}




void 
OgreSurfaceViewer::updateCamera(){

	// Quats to work with
	Ogre::Quaternion cameraRotationAboutWorldY, cameraRotationAboutWorldX, result; 

	// Set up the quaternions using the mouse input
	cameraRotationAboutWorldY.FromAngleAxis(Ogre::Degree(mouseRotation[0]),upAxis);
	cameraRotationAboutWorldX.FromAngleAxis(Ogre::Degree(-mouseRotation[1]),Ogre::Vector3::UNIT_X);

	// Get the result
	result = cameraRotationAboutWorldY * cameraRotationAboutWorldX;

	// Now play with the camera
	mCamera->setPosition(center);
	mCamera->setOrientation(result);
	mCamera->moveRelative(Ogre::Vector3(0,cameraZoom,0));
	mCamera->lookAt(center);
}

void
OgreSurfaceViewer::cleanDisplay(){
	
	mSceneMgr->destroyAllManualObjects();
	mainNode->removeAllChildren();
	mainNode->setScale(1.,1.,1.);

	center = Ogre::Vector3(0.,0.,0.);

	if(mSceneMgr->hasEntity("triangleDisplay"))
		mSceneMgr->destroyEntity("triangleDisplay");


}