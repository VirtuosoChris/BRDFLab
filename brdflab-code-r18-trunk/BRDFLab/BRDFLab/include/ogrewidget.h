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

#ifndef __OGREWIDGET_H__
#define __OGREWIDGET_H__

#include <Ogre.h>
//#include <QtOpenGL\QGLWidget>
#include <QWidget>
#include <QMouseEvent>
#include <QTime>
#include <QLabel>

#include <iostream>

#include "BRDF.h"

//#include <QX11Info>

class OgreWidget : public QWidget
{
  Q_OBJECT;
  
 public:
	 OgreWidget( QWidget *parent=0 , Ogre::String name="first"):
    QWidget( parent ),
    mOgreWindow(NULL),
	windowName(name),
	tempValue(1.),
	zoomFactor(1.)
    {
		this -> setFocusPolicy(Qt::StrongFocus) ;
		//IMPORTANT!!!
		// the first window opened will not have the Ogre Root object and this class will create it!
		// on the other windows opened, the Ogre Root object wont be NULL.
		//
		if(Ogre::Root::getSingletonPtr()==NULL){
	 		mOgreRoot = NULL;
		}else{
			mOgreRoot = Ogre::Root::getSingletonPtr();
		}


	}
  
  ~OgreWidget()
    {
//		std::cout << "em matennnn!!!  " << std::endl;

		killTimer(timerId);

		if(mOgreWindow != NULL){
			mOgreRoot->detachRenderTarget(mOgreWindow->getName());

			mOgreWindow->setActive(false);

			mOgreRoot->getRenderSystem()->destroyRenderTarget(mOgreWindow->getName());
			std::cout << mOgreWindow->getName() << std::endl;

//			DestroyWindow( windowHWND );
		}
		
		if(mSceneMgr != NULL){
			mSceneMgr->destroyAllCameras();
			mSceneMgr->destroyAllEntities();
			mSceneMgr->destroyAllLights();
			mSceneMgr->destroyAllManualObjects();

			mOgreRoot->destroySceneManager(mSceneMgr);

		}

		

		//  mOgreRoot->shutdown();
   //   delete mOgreRoot;

	//root->detachRenderTarget(nameRenderWindow);

		//destroy();
    }

  void setIncidentVector(float phi,float theta);
  void paintMeasuredPoints(std::vector<Ogre::Vector3> *points);
  void cleanMeasuredPoints();
  void setSecondaryMaterial(QString name);
  void setPrimaryMaterial(QString name);
  void createSphere(const std::string& strName, const float r, const int nRings = 16, const int nSegments = 16);
  void createHemiSphere(int level);
  bool exists(Ogre::Vector3 v, std::vector<Ogre::Vector3> *vertex_, int &pos);
  void subdivideTriangle(Ogre::Vector3 tri,std::vector<Ogre::Vector3> *vertex_, std::vector<Ogre::Vector3> *index_, int level);
  void drawImportanceSamplingDirections(std::vector<Ogre::Vector3> *directions);
  virtual void updateCamera();
  virtual void setUpAxis();
  virtual void setInitialCameraPosition();
  void displayPrimaryMaterial(bool a);
  void displaySecondaryMaterial(bool a);
  
//  void setActive(bool a);
  
 
public slots:
	void update();
	void paintEvent();
	void timerEvent(QTimerEvent *) ;
	void resizeEvent(QResizeEvent *);

	// input
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *e);
	void wheelEvent(QWheelEvent *e);
	void keyPressEvent ( QKeyEvent * event );  
	void setObjects(int x);
	

protected:
	
   virtual void initializeGL();
   void paintGL();
 

  void init( std::string, std::string, std::string );

protected:
	void setupResources();
	virtual void createScene();

	void paintAxis();

   Ogre::RenderSystem* chooseRenderer( Ogre::RenderSystemList* );

  Ogre::Root *mOgreRoot;
  Ogre::RenderWindow *mOgreWindow;
  Ogre::Camera *mCamera;
  Ogre::Viewport *mViewport;
  Ogre::SceneManager *mSceneMgr;
  Ogre::SceneNode *mainNode;

	//gui
  	QPoint mousePressPos;
	bool mousePressed;
	Qt::MouseButton buttonPressed;
	Ogre::Vector2 mousePos;
	Ogre::Vector2 mouseRotation;
	float cameraZoom;
	Ogre::Vector3 upAxis;

	Ogre::String windowName;
	Ogre::ManualObject *incidentVectorManualObject;
	Ogre::ManualObject *measuredPoints;

	Ogre::SceneNode *lightNode;
	float tempValue;
	float zoomFactor;

	int timerId;
//	HWND windowHWND;
};

#endif