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


#ifndef FittedBRDF_H
#define FittedBRDF_H

#include "AnalyticBRDF.h" 

class FittedBRDF: public AnalyticBRDF
{
	Q_OBJECT

public:

	FittedBRDF(int channels,LobeCombination lc,QString name);

//	~FittedBRDF();

	void loadFittingParametersXML(QDomElement param);
	void writeXML(QDomDocument *doc, QDomElement *parent);

	void setError(double e){ error = e;}
	double getError() {return error; }

	void setOptimizerName(QString opti) {optimizerName = opti; }
	QString getOptimizerName() {return optimizerName; }

	bool isReadOnly(){return readOnly;}

private:
	bool readOnly;
	double error;
	QString optimizerName;
};
#endif

