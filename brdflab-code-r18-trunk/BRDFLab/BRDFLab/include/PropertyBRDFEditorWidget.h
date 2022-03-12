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

#ifndef PropertyBRDFEditorWidget_H_
#define PropertyBRDFEditorWidget_H_


#include <iostream>

#include <QWidget>
#include <QInputDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QtCore/QLocale>
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"

#include "BRDF.h"
#include "AnalyticBRDF.h"
#include "SimulatedBRDF.h"


class PropertyBRDFEditorWidget : public QWidget
{
	Q_OBJECT

public:

	PropertyBRDFEditorWidget(BRDF *mainBRDF);


private:

//	QComboBox *comboBox;
	QtVariantPropertyManager *variantManager;
	QtTreePropertyBrowser *variantEditor;
	QtVariantEditorFactory *variantFactory;

	QPushButton *button;
	BRDF* brdf;

};
#endif
