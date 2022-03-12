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

#include "FittedBRDF.h"

FittedBRDF::FittedBRDF(int channels, LobeCombination lc, QString name):
	AnalyticBRDF(channels,lc,name),
	error(0.),
	optimizerName("none"),
	readOnly(false)
{
	fitted = true;
	setCubeRoot(cubeRoot);
}

void 
FittedBRDF::loadFittingParametersXML(QDomElement param){
	
	optimizerName = param.attribute("optimizerName");
	error = param.attribute("error").toDouble();
	QString r = param.attribute("readOnly");

	if(r == QString("true"))
		readOnly = true;
	else
		readOnly = false;

//	todo work on this! put the editor in read only!!

	if(readOnly){
	/*
		for(unsigned int i=0; i<paramLobes.size(); i++)
			paramLobes[i]->setReadOnly(true);
	
		for(unsigned int i=0; i<paramScaleFactors.size(); i++)
			paramScaleFactors[i]->setReadOnly(true);
	*/
	}

	//load Analytic BRDF information
//	QDomNode n = param.firstChild();
//	QDomElement e = n.toElement();
//	dynamic_cast<AnalyticBRDF*>(this)->loadXML(e);
	
}

void 
FittedBRDF::writeXML(QDomDocument *doc, QDomElement *parent){
	
	// write the fitting properties
	QDomElement fitting = doc->createElement("Fitting");

	fitting.setAttribute("optimizerName",optimizerName);
	fitting.setAttribute("error",error);

	if(readOnly)
		fitting.setAttribute("readOnly","true");
	else
		fitting.setAttribute("readOnly","false");

	// write the Analytic BRDF
	writeMaterialXML(doc,&fitting);
	
	parent->appendChild(fitting);

}