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

#ifndef PropertyEditorDock_H_
#define PropertyEditorDock_H_

#include <QWidget>

#include <iostream>

#include <QInputDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QtCore/QLocale>
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"

#include "AnalyticBRDF.h"
#include "LMFitting.h"
#include "NLoptFitting.h"
#include "FittedBRDF.h"


class PropertyEditorWidget : public QWidget
{
	Q_OBJECT

public:

	PropertyEditorWidget(BRDF *src);

	// TODO: cleaning this!
	~PropertyEditorWidget(){
		
		// delete fitted BRDFs
		for(unsigned int i=0; i<brdfs.size(); i++)
			delete brdfs[i];

		brdfs.clear();

		delete addBRDFButton;
		delete delBRDFButton;
		delete addLobeButton;
		delete clearBRDFButton;

		delete errorSpinBox;
		delete optimizeButton;
		delete comboBox;


	
//		delete lmfit;
	}
	void writeXML(QDomDocument *doc, QDomElement *parent);

	bool isOptimizing() {return optimizing; }

	Fitting* createOptimizer(QString optimizer, bool nonLinearOptimization = false);

	/*
	void setMainBRDF( AnalyticBRDF *main){
		comboBox->addItem(main->getMaterialName());
		dst = main;
	}*/
	
	public slots:
		void setError(double e);
		void addBRDF();
		void addBRDF(QDomElement *e);
		void delBRDF();
		void addLobe();
		void clearBRDF();
		void optimize();
		void optimizeAll();
		void brdfChanged(int index);
		void setMaterial(int m);
		void setActiveBRDF(bool a);
		void updateMaterial();
		void cleanBRDFs();

		//to delete
		//void clear();
	
signals:
		void brdfModified();


private:

	void setDatabaseGUI();
	void setFittingGUI();
	void enableGui();
	void disableGui();


	QtVariantPropertyManager *variantManager;
	QtTreePropertyBrowser *variantEditor;
	QtVariantEditorFactory *variantFactory;

	QComboBox *comboBox;	
	QPushButton *addBRDFButton;
	QPushButton *delBRDFButton;
	QPushButton *addLobeButton;
	QPushButton *clearBRDFButton;
	QDoubleSpinBox *errorSpinBox;
	QPushButton *optimizeButton;

	bool optimizing;
	int brdfsCreated;

	Fitting *fit;

	BRDF* src;
	//AnalyticBRDF* dst;

	std::vector<FittedBRDF*> brdfs;
	int actualBRDF;
	int currentMaterial;

	bool brdfStatus;


};
#endif
