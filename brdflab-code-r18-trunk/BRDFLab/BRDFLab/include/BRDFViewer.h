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

#ifndef BRDFViewer_H
#define BRDFViewer_H

#include "ogrewidget.h"
#include "BRDF.h"
#include "SimulatedBRDF.h"
#include "MeasuredBRDF.h"
#include "RawDataBRDF.h"

#include "BRDFLab.h"


//#include "ScatmechBRDF.h"
//#include "ReliefBRDF.h"


#include <QWidget>
#include <QSlider>
#include <QVBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <iostream>


class BRDFLab;

class BRDFViewer : public QWidget
{

	Q_OBJECT

public:
	BRDFViewer(QWidget *parent=0, BRDFLab *main=0 );

	~BRDFViewer();

	void initSliders();

//	void setActive(bool a) { window->setActive(a); }

	//public slots:
		//void resizeEvent( QResizeEvent * event );

	public slots:
		void forceUpdate() { slidersChanged(); }
		void updateImportanceSampleDirections();\
		void setMeshDisplay();
		void setPointsDisplay();

	private slots:
		void slidersChanged();
		void sliderTexSizeChanged();
		void setDisplayChannel(int x);
		void cubeRootStateChanged(int x);
		void transparentObjectsStateChanged(int x);
		void displayLobe(int x);
		void setTransformation(int x);
		void setDisplayMode(int x);


private:

//	int previousThetaValue;
//	int previousPhiValue;

	BRDFLab *brdfLab;
	OgreWidget *window;

	QComboBox* channelComboBox;
	Ogre::Vector3 incidentVector;

	QSlider *sliderTheta;
	QSlider *sliderPhi;
	QSlider *sliderTexSize;
	QCheckBox *cubeRoot;
	QCheckBox *transparentCheckBox;
	QCheckBox *displayImportanceSamplesCheckBox;
	QGroupBox *incidentDirectionGroupBox;

	// display only for measured BRDFs
	QComboBox* transformComboBox;
	QComboBox* lobeComboBox;
	QGroupBox *displayGroupBox;
	QGroupBox *displayPointsGroupBox;
	QRadioButton *pointsRadioButton;
	QRadioButton *meshRadioButton;
	bool displayPoints;
};

#endif