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

#ifndef ObjectRenderer_H
#define ObjectRenderer_H

#include "OgreObjectRenderWindow.h"
#include "BRDF.h"

#include <iostream>

#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QRadioButton>
#include <QGroupBox>



class ObjectRenderer : public QWidget
{

	Q_OBJECT

public:
	ObjectRenderer(QWidget *parent=0);

	void activeFittedRadioButton(bool active);
//	int getLight();
	int getLightType();
	bool isOriginalBRDFRendering() {return originalRadioButton->isChecked(); }
	void setOriginalMaterialType(int t);
	void clickOriginalRadioButton();
		
	public slots:
		void adaptGUI(int);
		void setKeyGamma();
		void setOriginalMaterial();
		void setFittedMaterial();
		void setLightType(int l);
		void setLightMaterials(int i);

//		void ambientLightChanged(double x);

	signals:
		void setActiveBRDF(int m);
		void lightTypeChanged(int);






private:

	OgreObjectRenderWindow *ogreWindow;

	QGroupBox *parametersGroupBox;
	QComboBox *lightsComboBox;
	QComboBox *modelsComboBox;
	QComboBox *probeComboBox;
//	QLabel *textSamples;
//	QSpinBox *nSamplesSpinBox;
	QLabel *lightLabel, *modelLabel, *probeLabel;

	
	QGroupBox *toneMapGroupBox;
	QDoubleSpinBox *keyDoubleSpinBox;
	QDoubleSpinBox *gammaDoubleSpinBox;

	QDoubleSpinBox *ambientSpinBox;

	QGroupBox *materialGroupBox;
	QRadioButton *originalRadioButton;
	QRadioButton *fittedRadioButton;

	int originalMaterialType;
	int currentLightType;
};

#endif