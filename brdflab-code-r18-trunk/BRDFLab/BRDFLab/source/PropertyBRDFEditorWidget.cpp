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

#include "PropertyBRDFEditorWidget.h"

using namespace std;

PropertyBRDFEditorWidget::PropertyBRDFEditorWidget(BRDF *mainBRDF):
	brdf(mainBRDF)
{

	variantManager = new QtVariantPropertyManager();
	variantFactory = new QtVariantEditorFactory();
	variantEditor = new QtTreePropertyBrowser();
	variantEditor->setFactoryForManager(variantManager, variantFactory);
	variantEditor->setPropertiesWithoutValueMarked(true);
	variantEditor->setRootIsDecorated(false);

	if(dynamic_cast<AnalyticBRDF*>(brdf) != NULL){
		dynamic_cast<AnalyticBRDF*>(brdf)->setGUIManagers(variantManager,variantEditor);
	}else if(dynamic_cast<SimulatedBRDF*>(brdf) != NULL){
		dynamic_cast<SimulatedBRDF*>(brdf)->setGUIManagers(variantManager,variantEditor);
		//dynamic_cast<SimulatedBRDF*>(brdf)->setQPropertyEditorWidget(brdfOptions);
	}else{
		QMessageBox::warning(this,tr("BRDFLab"),tr("PropertyBRDFEditorWidget with a BRDF that is not Analytic or Scatmech."));
		return;
	}

	QVBoxLayout *layout = new QVBoxLayout();

	if(dynamic_cast<AnalyticBRDF*>(brdf) != NULL){
		
		button = new QPushButton("Add Lobe", this);
		//connect the addLobe button
		connect(button, SIGNAL(clicked()), dynamic_cast<AnalyticBRDF*>(brdf), SLOT(addLobe()));  

		layout->addWidget(button);
	}

	layout->addWidget(variantEditor);

	setLayout(layout);


}