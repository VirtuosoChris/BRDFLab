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


#ifndef GroovesBRDF_H
#define GroovesBRDF_H

#include "ReliefBRDF.h" 
#include "OgreGroovesWindow.h"


class GroovesBRDF: public ReliefBRDF
{
	Q_OBJECT
public:

	GroovesBRDF():
		ReliefBRDF("GroovesRelief"),
		groovesTexture("GroovesRelief"),
		ogw(NULL)
	{		
		brdfType = QString("Grooves");
		
		// create gui Parameters
		//guiParams.push_back(new GroovesParameters());
	}

	~GroovesBRDF(){
		if(Ogre::TextureManager::getSingleton().resourceExists(groovesTexture)){
			Ogre::TextureManager::getSingleton().remove(groovesTexture);
		}		

		if(ogw != NULL){
			delete ogw;
		}

	}

	void writeXML(QDomDocument *doc, QDomElement *parent);
	void recreateParameterGUI(){}

	public slots:
		void guiUpdated() {updateRelief(); } 
		void updateRelief(double width=0., double spacing=0., double botAngle=0., double angle=0., int groovesPerSide=0, int texelsPerGroove=0);

protected:
	
	Ogre::String groovesTexture;
	OgreGroovesWindow *ogw;


};



#endif