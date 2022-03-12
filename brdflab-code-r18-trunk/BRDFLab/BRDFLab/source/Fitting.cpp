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

#include "Fitting.h"

using namespace std;

BRDF* Fitting::src = NULL; 
AnalyticBRDF* Fitting::dst = NULL;
double* Fitting:: lobeValueMatrix = NULL;
double* Fitting:: AtransposeA = NULL;
double* Fitting:: invAtransposeA = NULL;
double* Fitting:: AtransposeB = NULL;
double Fitting::error = 0.;

bool Fitting::verbose = false;
bool Fitting::forceStop = false;
std::vector<Ogre::Vector3>* Fitting::u = NULL;
std::vector<Ogre::Vector3>* Fitting::v = NULL;
double* Fitting::y = NULL;
double* Fitting::p = NULL;
double* Fitting::upBound = NULL;
double* Fitting::lowBound = NULL;
int Fitting::ySize = 0;
int Fitting::errorMetric = 0;
bool Fitting::nonLinear = true;
QDoubleSpinBox* Fitting::errorGUI = NULL;

Fitting::Fitting(BRDF *source, AnalyticBRDF *destination, QString name, bool nonLinearOptimization):
	optimizerName(name),
	maxIterations(1000)
{

	
	src = source;
	dst = destination;
	
	nonLinear = nonLinearOptimization;

	error = 0.;

	if (src->getNChannels() != dst->getNChannels()){
		cerr << "Channel Mistmatch between Measured and Analytic BRDF." << endl;
		exit(1);
	}


}




void 
printMat(double *M, int size){
	for (int i=0; i< size; i++){
		for (int j=0; j<size; j++)
			cout << M[i*size+j] << ",";
			cout << endl;
	}
}


// matrix inversion. Adapted from http://chi3x10.wordpress.com/2008/05/28/calculate-matrix-inversion-in-c/
// calculate the cofactor of element (row,col)
void
GetMinor(double *M, double *minor, int row, int col, int size)
{
	// indicate which col and row is being copied to dest
	int colCount=0,rowCount=0;

	for(int i = 0; i < size; i++ )
	{
		if( i != row )
		{
			colCount = 0;
			for(int j = 0; j < size; j++ )
			{
				// when j is not the element
				if( j != col )
				{
					minor[rowCount*(size-1)+colCount] = M[i*size+j];
					colCount++;
				}
			}
			rowCount++;
		}
	}
}

// Calculate the determinant recursively.
double 
CalcDeterminant( double *M, int size)
{
	if( size == 1 )
		return M[0];
	// the determinant value
	double det = 0;
	// allocate memory for cofactor matrix
	double *minor = new double[(size-1)*(size-1)];
	bool odd = false;
	for(int i = 0; i < size; i++ )
	{
		// get minor of element (0,i)
		GetMinor( M, minor, 0, i , size);
		// the recusion is here!
		double term = M[i] * CalcDeterminant( minor,size-1 );
		if (odd) det -=  term;
		else det += term;
		odd = !odd;
	}
	// release memory
	delete [] minor;
	return det;
}

#define SMALL 1e-05
void 
Fitting::matInverse(double *M, int size, double *iM)
{
	// get the determinant of M
	double determinant = CalcDeterminant(M,size);
	if (abs(determinant) < SMALL){ 
		cerr << "Determinance is zero or close to zero" << endl;
		determinant = SMALL;
	}
	double factor = 1.0/determinant;

	// allocate memory
	double *minor = new double[(size-1)*(size-1)];

	for(int j=0;j<size;j++)
	{
		for(int i=0;i<size;i++)
		{
			// get the co-factor of M(j,i)
			GetMinor(M,minor,j,i,size);
			int index = i*size+j;
			iM[index] = factor*CalcDeterminant(minor,size-1);
			if( (i+j)%2 == 1)
				iM[index] = -iM[index];
		}
	}
	// release memory
	delete [] minor;
}


void
Fitting::writeXML(QDomDocument *doc, QDomElement *parent){

	QDomElement fitting = doc->createElement("Fitting");

	fitting.setAttribute("optimizer",optimizerName);
	fitting.setAttribute("error",error);

	
	dst->writeXML(doc,&fitting);
	
	parent->appendChild(fitting);


}

void 
Fitting::generateSyntheticDirections(std::vector<Ogre::Vector3> *u, std::vector<Ogre::Vector3> *v){
	
	int j=7; // for now, isotropic
	int n=16;
	for (int i = 0; i < n; i++) 
	{
		double theta_in = i * 0.5 * Ogre::Math::PI / n;
	   // for (int j = 0; j < 4*n; j++) 
		{
			double phi_in = j * 2.0 * Ogre::Math::PI / (4*n);
			for (int k = 0; k < n; k++) 
			{
				double theta_out = k * 0.5 * Ogre::Math::PI / n;
				for (int l = 0; l < 4*n; l++) 
				{
						double phi_out = l * 2.0 * Ogre::Math::PI / (4*n);

						Ogre::Vector3 dir(BRDF::spherical2cart(Ogre::Vector2(phi_out,theta_out)));
						
						u->push_back(dir);
						v->push_back(dir); // only one needed...
				}		
			}
		}
	}
}

int 
Fitting::preOptimization()
{
	

	if(dynamic_cast<AnalyticBRDF*>(src) != NULL ){
		
		// generate directions
		u = new std::vector<Ogre::Vector3>; // to do delete those vectors, y, ...at the end of the fitting.
		v = new std::vector<Ogre::Vector3>;
		
		generateSyntheticDirections(u,v);

		// set guess values
//		dynamic_cast<AnalyticBRDF*>(src)->setGuessValues();
		
		// create the Y vector
		int nChannels = src->getNChannels();
	
		ySize = nChannels*u->size();
		y = new double[ySize];

		//compute the measured vector with the actual p values
		for(unsigned int i=0; i<u->size(); i++){
			dynamic_cast<AnalyticBRDF*>(src)->evalBRDF(dynamic_cast<AnalyticBRDF*>(src)->getParameters(), (*u)[i],(*v)[i],y+i*nChannels);
		}
			
	}else if(dynamic_cast<MeasuredBRDF*>(src) != NULL) { // we are dealing with measured data
/*
		// get directions 
		u = (dynamic_cast<MeasuredBRDF*>(src))->getIncidentDirections();
		v = (dynamic_cast<MeasuredBRDF*>(src))->getOutgoingDirections();

		// get y vector
		y = (dynamic_cast<MeasuredBRDF*>(src))->getMeasuredData();

		// get the y vector size
		ySize = (dynamic_cast<MeasuredBRDF*>(src))->getYSize();
*/
		// generate directions
		u = new std::vector<Ogre::Vector3>; // to do delete those vectors, y, ...at the end of the fitting.
		v = new std::vector<Ogre::Vector3>;


		dynamic_cast<MeasuredBRDF*>(src)->getFittingInformation(u, v, y, ySize);
	}else if (dynamic_cast<RawDataBRDF*>(src) != NULL){

		u = dynamic_cast<RawDataBRDF*>(src)->getIncidentDirections();
		v = dynamic_cast<RawDataBRDF*>(src)->getOutgoingDirections();
		std::vector<Ogre::Vector3> *yVector = dynamic_cast<RawDataBRDF*>(src)->getIntensities();

		ySize = yVector->size()*3;
		y = new double[ySize];

		int count = 0;
		for (int i=0; i<ySize; i=i+3){
			y[i+0] = (*yVector)[count][0]; //red
			y[i+1] = (*yVector)[count][1]; //green
			y[i+2] = (*yVector)[count][2]; //blue
			count++;
		}

	}else { // simulated data

		// ask the user the incident resolution of the simulation
		bool ok;

		int phiDivision = QInputDialog::getInteger(NULL,
				QObject::tr("BRDFLab"), QObject::tr("Number of phi divisions"),
				1, 1, 360, 1, &ok);
				// default, min, max, step, 

		if (!ok)
			return -1;

		int thetaDivision = QInputDialog::getInteger(NULL,
				QObject::tr("BRDFLab"), QObject::tr("Number of theta divisions"),
				16, 1, 360, 1, &ok);
				// default, min, max, step, 

		if (!ok)
			return -1;

		// generate directions
		u = new std::vector<Ogre::Vector3>; // to do delete those vectors, y, ...at the end of the fitting.
		v = new std::vector<Ogre::Vector3>;


		dynamic_cast<SimulatedBRDF*>(src)->generateFittingInformation(thetaDivision, phiDivision, u, v, y, ySize);

	/*
		// create the Y vector
		int nChannels = src->getNChannels();
	
		ySize = nChannels*u->size();
		y = new double[ySize];

		//compute the measured vector with the actual p values
		for(unsigned int i=0; i<u->size(); i++){
			y[i] = dynamic_cast<ScatmechBRDF*>(src)->evalBRDF((*u)[i], (*v)[i]);
			
		}*/
	}

	int nScalingFactors = 0;
	int nParameters = 0;

	if (dst->linearCombination()){

		if(nonLinear){
			
			nParameters = dst->getNLobes()*dst->getNChannels()+dst->getNParameters();

			p = new double[nParameters];
			upBound = new double[nParameters];
			lowBound = new double[nParameters];

			double *scaling = dst->getScalingFactors();

			nScalingFactors = dst->getNLobes()*dst->getNChannels();

			cout << "optimize function: " << endl;
			cout << "scaling: ";
			for(int i=0; i<nScalingFactors; i++){
				p[i] = scaling[i];
				upBound[i] = 100.;
				lowBound[i] = 0.;

				cout << " " << p[i]; 
			}
			cout << endl;

			double *parameters = dst->getParameters();
			double *lb = dst->getLowerBound();
			double *ub = dst->getUpperBound();

			cout << "parameters: ";
			for(int i=0; i<dst->getNParameters(); i++){
				p[nScalingFactors+i] = parameters[i];
				lowBound[nScalingFactors+i] = lb[i];
				upBound[nScalingFactors+i] = ub[i];
				cout << " " << p[nScalingFactors+i];
			}		
			cout << endl;
			

		}else{
			lobeValueMatrix = new double[u->size()*dst->getNLobes()];
			AtransposeA = new double[dst->getNLobes()*dst->getNLobes()];
			invAtransposeA = new double[dst->getNLobes()*dst->getNLobes()];
			AtransposeB = new double[dst->getNLobes()*dst->getNChannels()];
			if (lobeValueMatrix == NULL||AtransposeA==NULL||invAtransposeA==NULL||AtransposeB==NULL){
				cerr << "Can not allocate Lobe Value Matrix." << endl;
				exit(1);
			}
		}
	}
	else{
		lobeValueMatrix = AtransposeA = AtransposeB = NULL;
	}

	return nParameters;
}


void
Fitting::stop(){
	forceStop = true;
}

void 
Fitting::setErrorGUI(QDoubleSpinBox *errorSpinBox){

	errorGUI = errorSpinBox;
}


int
Fitting::getStatus(){
	return status;
}

void Fitting::computeBRDFVector( double *p, double *x )
{
	int count = 0;

	int nChannels = src->getNChannels();
	int nLobes = dst->getNLobes();
	int nScalingFactors = nChannels*nLobes;

	if (dst->linearCombination()){


		if(nonLinear){
			//set all the parameters to the BRDF.



			// set the scaling factors.
			double *scaling = dst->getScalingFactors();

			//				cout << "scaling: ";

			for(int i=0; i<nScalingFactors; i++){
				scaling[i] = p[i];
				//					cout << " " << p[i];
			}

			double *parameters = dst->getParameters();
			int nParams = dst->getNParameters();

			for(int i=0; i<nParams; i++)
				parameters[i] = p[i+nScalingFactors];

		}else{

			int nLobes = dst->getNLobes();
			// BEGIN: Linear Least Squares estimation of ScaleFactors
			// Coefficients = inv(A'A)*(A'B) where A' is transpose(A)
			for(unsigned int i=0; i<u->size(); i++)  // todo look what happens here!!!
				dst->evalBRDFLobes(p, (*u)[i],(*v)[i],lobeValueMatrix+i*nLobes);

			for(unsigned int i=0; i<u->size()*dst->getNLobes(); i++){

				if(lobeValueMatrix[i] != lobeValueMatrix[i])
					cout << lobeValueMatrix[i] << " piii " << endl;

			}


			//Compute A'A matrix
			for (int i=0; i<nLobes;i++)
				for (int j=0; j<nLobes;j++){
					AtransposeA[i*nLobes+j] = 0.0;
					for (unsigned int l=0; l<u->size(); l++) 
						AtransposeA[i*nLobes+j]+=lobeValueMatrix[l*nLobes+i]*lobeValueMatrix[l*nLobes+j];
				}
				//Compute inv(A'A)
				matInverse(AtransposeA,nLobes,invAtransposeA);
				//Compute A'B vector for each channel.
				for (int i=0; i<nLobes;i++){
					for (int k=0;k<nChannels;k++)
						AtransposeB[i*nChannels+k] = 0.0;
					for (unsigned int l=0; l<u->size(); l++)
						for (int k=0;k<nChannels;k++)
							AtransposeB[i*nChannels+k]+=lobeValueMatrix[l*nLobes+i]*y[l*nChannels+k];

				}
				//Compute Scaling Coefficients inv(A'A)*(A'B)
				for (int i=0; i<nLobes;i++){
					for (int k=0;k<nChannels;k++)
						dst->scaleFactors[i*nChannels+k] = 0.0;
					for (int j=0; j<nLobes;j++) 
						for (int k=0;k<nChannels;k++)
							dst->scaleFactors[i*nChannels+k] += invAtransposeA[i*nLobes+j]*AtransposeB[j*nChannels+k];
				}
				// END: Linear Least Squares

				//				printMat(invAtransposeA, nLobes);

				for(int i=0; i< nLobes*nChannels; i++){
					if(dst->scaleFactors[i] != dst->scaleFactors[i]){
						printMat(AtransposeA, nLobes);
					}


				}



		}
	}

	//dst->showParameters();

	

	//		cout << parameters[0] << " " << parameters[1] << " " << parameters[2] << endl;

	//	cout << "lafortune coefs: " << p[nScalingFactors+0] << " " << p[nScalingFactors+1] << " " << p[nScalingFactors+2] << endl;

	double error = 0.;

	//compute the measured vector with the actual p values
	for(unsigned int i=0; i<u->size(); i++){

		if(nonLinear){

			dst->evalBRDF(p+nScalingFactors, (*u)[i],(*v)[i],x+i*nChannels);
		}else{
			dst->evalBRDF(p, (*u)[i],(*v)[i],x+i*nChannels);
		}

//		for (int k=0; k < nChannels; k++) error += (x[i*nChannels+k] - y[i*nChannels+k])*(x[i*nChannels+k] - y[i*nChannels+k]);
	}



}

double Fitting::computeError( double *x )
{

	int nChannels = dst->getNChannels();

	error = 0.;

	switch (errorMetric)
	{
	case 0:
		// L2 error
		for(unsigned int i=0; i<u->size(); i++)
			for (int k=0; k < nChannels; k++) 
				error += (x[i*nChannels+k] - y[i*nChannels+k])*(x[i*nChannels+k] - y[i*nChannels+k]);

		break;

	case 1:
		// L2 error weighted with cos theta_i
		for(unsigned int i=0; i<u->size(); i++)
			for (int k=0; k < nChannels; k++) 
				error += (x[i*nChannels+k]*(*u)[i].z - y[i*nChannels+k]*(*u)[i].z)*(x[i*nChannels+k]*(*u)[i].z - y[i*nChannels+k]*(*u)[i].z);
		
		break; 

	case 2:
		// Ngan et al. error Metric
		/*
		Solid angle computation equation is: \integration sin\theta d\theta d\phi
			You can use an approximation of this equation as: sin\theta (\delta
			\theta) (\delta \phi)
			Delta \theta and Delta\phi depend on the resolution. (pi/2/rows and
			2Pi/cols). Compute \theta at the center of the bin. As expected, the
			accuracy of the approximation improves with the resolution. Total
			solid angle should be 2PI steradian. So it is better to sum up the
			total solid angle and normalize to 2PI.
		*/
	// float solidAngle = sin(theta)*(Ogre::Math::PI*2/capturingWidth)*(Ogre::Math::PI/2/capturingWidth);
		//	float phiDivisions = 64.;
		//	float thetaDivisions = 16.;


		float phiDivisions = 64.;
		float thetaDivisions = 16.;

		float capturingTheta = Ogre::Math::PI/2/thetaDivisions;
		float capturingPhi = Ogre::Math::PI*2/phiDivisions;

		float totalSolidAngle = 0.;


		for(unsigned int i=0; i<u->size(); i++){
			Ogre::Vector2 sph = BRDF::cart2spherical((*u)[i]);
			float solidAngle = sin(sph[1])*capturingPhi*capturingTheta;
			totalSolidAngle += solidAngle;
			for (int k=0; k < nChannels; k++) 
				error += (solidAngle*(x[i*nChannels+k]*(*u)[i].z - y[i*nChannels+k]*(*u)[i].z)*(x[i*nChannels+k]*(*u)[i].z - y[i*nChannels+k]*(*u)[i].z));
		}

		error /= totalSolidAngle;
		error = sqrt(error);

		break;
	}

	if(errorGUI!=NULL)
		errorGUI->setValue(error);


	return error;
}

void 
Fitting::setErrorMetric(int eM){

	errorMetric = eM;

}