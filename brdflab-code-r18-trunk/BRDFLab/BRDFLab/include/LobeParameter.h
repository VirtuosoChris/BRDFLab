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

#ifndef LobeParameter_H
#define LobeParameter_H

//include OGRE for using Vector3 types and that stuff.
#include <Ogre.h>

#include <QString>
#include <QDomDocument>
#include <iostream>


class LobeParameter
{
public:

	LobeParameter(QDomElement desc);

	QString getName() {return name;}
	double getLowBound() {return lowBound;}
	double getHighBound() {return highBound;}
	double getDefaultVal() {return defaultVal;}

protected:

	QString name;
	double lowBound;
	double highBound;
	double defaultVal;
};


#endif