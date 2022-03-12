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

#ifndef SubsurfaceParticleBRDF_H
#define SubsurfaceParticleBRDF_H

// parent
#include "ScatmechBRDF.h"



//additional scatmech dependencies for the model
#include "scatmech/miescat.h"
#include "scatmech/subsphere.h"



class SubsurfaceParticleBRDF : public ScatmechBRDF
{

public:
	SubsurfaceParticleBRDF();


	void loadXML(QDomElement params);
	void writeXML(QDomDocument *doc, QDomElement *parent);

	public slots:
		void guiUpdated();



private:
	SCATMECH::Free_Space_Scatterer *ptr_sphericalScatterer;
};


#endif