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

#include "NLoptFitting.h"

using namespace std;



NLoptFitting::NLoptFitting(BRDF *source, AnalyticBRDF *destination, bool nonLinearOptimization, QString oName):
	Fitting(source,destination, oName, nonLinearOptimization)
{

	if(optimizerName == "COBYLA")
		algorithm = NLOPT_LN_COBYLA;
	else if(optimizerName == "BOBYQA")
		algorithm = NLOPT_LN_BOBYQA;
	else if(optimizerName == "NEWUOA")
		algorithm = NLOPT_LN_NEWUOA_BOUND;
	else if(optimizerName == "PRAXIS")
		algorithm = NLOPT_LN_PRAXIS;
	else if(optimizerName == "Nelder-Mead Simplex")
		algorithm = NLOPT_LN_NELDERMEAD;
	else if(optimizerName == "Sbplx")
		algorithm = NLOPT_LN_SBPLX;
	else // if nothing is reasonable, use COBYLA
		algorithm = NLOPT_LN_COBYLA;

}


bool 
NLoptFitting::optimize(){

	forceStop = false;

	int nParameters = preOptimization();

	if(!nonLinear)
		nParameters = dst->getNParameters();

	// check the directions
	// check the initial parameters
	// check the low and high bound
	// set optimizer options
	// optimize
	// show results
	// store to file or add to the src BRDF the dst BRDF with the fitted information

	// set optimizer variables: WHAT ARE THESE


	//init optimizer






	//read from the source the right color!!!!!!

	// initialize guess parameters, and low and up bounds
	dst->initForOptimize();

	nlopt_opt opt;



	opt = nlopt_create(algorithm , nParameters); /* algorithm and dimensionality */ //NLOPT_LD_MMA ,NLOPT_LN_COBYLA
	nlopt_set_lower_bounds(opt, lowBound);
	nlopt_set_upper_bounds(opt, upBound);
	nlopt_set_min_objective(opt, funcOptimize, NULL);

	nlopt_set_xtol_rel(opt, 1e-4);



	QTime *timer = new QTime();

	timer->start();

	if(nonLinear){
		//call the optimizer
		if (nlopt_optimize(opt, p, &error) < 0) {
			cout << "nlopt failed!\n";
		}
		else {
			cout << "optimization done!" << endl;
		}

		delete []p;
		delete []lowBound;
		delete []upBound;
	}else{
		//call the optimizer
		if (nlopt_optimize(opt, p, &error) < 0) {
			cout << "nlopt failed!\n";
			status = 7;
		}
		else {
			cout << "optimization done!" << endl;			
			status = 0;
		}
	}

	nlopt_destroy(opt);

	

	cout << endl << "Optimization Result: " << endl << endl;
	dst->showParameters();
	if(status != 7)
		dst->setParametersToGUI();
	cout << "Final error: " << error << endl;
	cout << "Fitting time: " << timer->elapsed() << endl;

	delete timer;

	return forceStop;


}

double 
NLoptFitting::funcOptimize(unsigned n, const double *p, double *grad, void *my_func_data){

	if(!forceStop){
//		if(verbose)
//			dst->showParameters();
		double *x = new double[u->size()*dst->getNChannels()];
		double *params = new double[n];

		for(int i=0; i<n; i++)
			params[i] = p[i];


		computeBRDFVector(params, x);

		error = computeError(x);

		if(verbose)
			cout << "--------------------------------------------- error: " << error << endl;

		dst->setParametersToGUI(); //see the optimization in live
		QCoreApplication::processEvents();
		
		delete []x;
		delete []params;

		return error;

	}else{ // force the optimizer to stop!

		return 0.;

	}
}