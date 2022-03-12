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

#include "GroovesBRDF.h"

using namespace std;
#include <QMetaObject>
#include <QMetaProperty>

void
GroovesBRDF::updateRelief(double width, double spacing, double botAngle, double angle, int groovesPerSide, int texelsPerGroove){
/*
	if(ogw == NULL){
		ogw = new OgreGroovesWindow(groovesTexture);
		ogw->paintEvent();
	}

	if(width == 0.){
		GroovesParameters *parameters = dynamic_cast<GroovesParameters*>(guiParams[0]);

		ogw->createGroovesTexture(parameters->width(), parameters->spacing(),parameters->botAngle(),parameters->angle(),parameters->groovesPerSide(),parameters->texelsPerGroove());
	}else{
		ogw->createGroovesTexture(width, spacing, botAngle, angle, groovesPerSide, texelsPerGroove);
	}

	Ogre::MaterialManager::getSingleton().getByName("reliefMaterial")->reload();//reliefMaterial //reliefMappingViewer
	*/
}

void 
GroovesBRDF::writeXML(QDomDocument *doc, QDomElement *parent){
	/*
	QDomElement material = doc->createElement("Material");

	QDomElement type = doc->createElement("Type");
	type.appendChild(doc->createTextNode(brdfType));

	GroovesParameters *parameters = dynamic_cast<GroovesParameters*>(guiParams[0]);

	QDomElement params = doc->createElement("Parameters");
	params.setAttribute("Width",parameters->width());
	params.setAttribute("Spacing",parameters->spacing());
	params.setAttribute("BotAngle",parameters->botAngle());
	params.setAttribute("Angle",parameters->angle());
	params.setAttribute("GroovesPerSide",parameters->groovesPerSide());
	params.setAttribute("TexelsPerGroove",parameters->texelsPerGroove());

	material.appendChild(type);
	material.appendChild(params);

	parent->appendChild(material);
	*/
}


//* gui updated? emit brdfModified();