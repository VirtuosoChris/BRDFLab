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

#ifndef NLoptFitting_H
#define NLoptFitting_H

#include "Fitting.h"


// include NLopt optimization
#include <math.h>
#include <nlopt.h>

#include <QDoubleSpinBox>
#include <QInputDialog>
#include <QCoreApplication>

class NLoptFitting : public Fitting
{

public:


	NLoptFitting(BRDF *source, AnalyticBRDF *destination, bool nonLinearOptimization = false, QString oName = "COBYLA");

	bool optimize();
	static double funcOptimize(unsigned n, const double *p, double *grad, void *my_func_data);
	
	

protected:

	
	nlopt_algorithm algorithm;

};


#endif