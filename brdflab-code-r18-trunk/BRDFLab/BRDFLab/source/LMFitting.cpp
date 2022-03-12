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

#include "LMFitting.h"

using namespace std;

double opts[LM_OPTS_SZ], info[LM_INFO_SZ];




LMFitting::LMFitting(BRDF *source, AnalyticBRDF *destination, bool nonLinearOptimization):
	Fitting(source,destination, "Levenberg-Marquardt",nonLinearOptimization)
	
{
}


bool 
LMFitting::optimize(){

	forceStop = false;

	int nParameters = preOptimization();

	// check the directions
	// check the initial parameters
	// check the low and high bound
	// set optimizer options
	// optimize
	// show results
	// store to file or add to the src BRDF the dst BRDF with the fitted information

	// set optimizer variables: WHAT ARE THESE

	opts[0] = 1; //scale factor initial mu
	opts[1] = 1; //stoping threshold J^T
	opts[2] = 1; //stoping threshold Dp
	opts[3] = 1; //stoping threshold e_2
	opts[4] = 0; //step used in jacobian approx

	if(verbose){
		cout << "opts[0] - scale factor initial mu: " << opts[0] << endl;
		cout << "opts[1] - stoping threshold J^T: " << opts[1] << endl;
		cout << "opts[2] - stoping threshold Dp: " << opts[2] << endl;
		cout << "opts[3] - stoping threshold e_2: " << opts[3] << endl;
		cout << "opts[4] - step used in jacobian approx: " << opts[4] << endl;
	}


	//read from the source the right color!!!!!!

	// initialize guess parameters, and low and up bounds
	dst->initForOptimize();


	if (errorMetric == 1)
	{
		for(unsigned int i=0; i<u->size(); i+=3){

			float t_in = (*u)[i][2];

			y[i+0] *= t_in;
			y[i+1] *= t_in;
			y[i+2] *= t_in;
		}

	}


	QTime *timer = new QTime();

	timer->start();

	if(nonLinear){
		//call the optimizer
		dlevmar_bc_dif(&LMFitting::funcOptimize,p,y,nParameters,ySize,lowBound,upBound,maxIterations,NULL,info,NULL,NULL,NULL);
		
		delete []p;
		delete []lowBound;
		delete []upBound;
	}else{
		//call the optimizer
		dlevmar_bc_dif(&LMFitting::funcOptimize,dst->getParameters(),y,dst->getNParameters(),ySize,dst->getLowerBound(),dst->getUpperBound(),maxIterations,NULL,info,NULL,NULL,NULL);
	}



	if(verbose){
		cout << "Status:" << endl;
		cout << "info[0]: " << info[0] << endl;
		cout << "info[1]: " << info[1] << endl;
		cout << "info[2]: " << info[2] << endl;
		cout << "info[3]: " << info[3] << endl;
		cout << "info[4]: " << info[4] << endl;
		cout << "info[5] - iterations: " << info[5] << endl;
		cout << "info[6] - status: " << info[6] << endl;
		cout << "info[7] - function evaluations: " << info[7] << endl;
		cout << "info[8] - Jacobian evaluations: " << info[8] << endl;
	}

	status = info[6];

	cout << endl << "Optimization Result: " << endl << endl;
	dst->showParameters();
	if(info[6] != 7)
		dst->setParametersToGUI();
	cout << "Final error: " << info[1] << endl;
	cout << "Fitting time: " << timer->elapsed() << endl;
//	if(!forceStop)
//		error = info[1];

	delete timer;


	return forceStop;

}

void 
LMFitting::funcOptimize(double *p, double *x, int m, int n, void *data){

	if(!forceStop){
//		if(verbose)
//			dst->showParameters();

		computeBRDFVector(p, x);

		error = computeError(x);

		if(verbose)
			cout << "--------------------------------------------- error: " << error << endl;

		if (errorMetric == 1)
		{
			for(unsigned int i=0; i<u->size(); i+=3){

				float t_in = (*u)[i][2];

				x[i+0] *= t_in;
				x[i+1] *= t_in;
				x[i+2] *= t_in;
			}

		}


		dst->setParametersToGUI(); //see the optimization in live
		QCoreApplication::processEvents();

	}else{ // force the optimizer to stop!
	
		for(int i=0; i<n; i++) //used to stop the optimizer
			x[i] = y[i];

	}
}