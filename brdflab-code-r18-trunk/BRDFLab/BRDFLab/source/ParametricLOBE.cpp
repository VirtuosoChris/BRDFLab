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

#include "ParametricLOBE.h"

using namespace std;

#define value_type double

//---------------------------------------------------------------------------
// Factory function for creating new parser variables
// This could as well be a function performing database queries.
double* AddVariable(const mu::char_type *a_szName, void *a_pUserData)
{
	// I don't want dynamic allocation here, so i used this static buffer
	// If you want dynamic allocation you must allocate all variables dynamically
	// in order to delete them later on. Or you find other ways to keep track of 
	// variables that have been created implicitely.
	static double afValBuf[100];  
	static int iVal = 0;          
/*
	cout << _T("Generating new variable \"") 
		<< a_szName << _T("\" (slots left: ")
		<< 99-iVal << _T(")")
		<< _T(" User data pointer is:") 
		<< std::hex << a_pUserData <<endl;
*/
	afValBuf[iVal] = 0;
	if (iVal>=99)
		throw mu::ParserError( _T("Variable buffer overflow.") );

	return &afValBuf[iVal++];
}

ParametricLOBE::ParametricLOBE( QString fileName):
paramVal(NULL),
ux(0.),
uy(0.),
uz(1.),
vx(0.),
vy(0.2),
vz(0.5),
theta_i(0.),
theta_o(0.),
phi_i(0.),
phi_o(0.)
{
	// open XML file
	QDomDocument doc("AnalyticBRDFModel");
	QFile file(fileName);

	if (!file.open(QIODevice::ReadOnly))
		return;

	if (!doc.setContent(&file)) {
		file.close();
		return;
	}

	file.close();

	// print out the element names of all elements that are direct children
	// of the outermost element.
	QDomElement e = doc.documentElement();

	//QDomNode n = desc.firstChild(); //analytic

		if(!e.isNull()) {
			if(e.tagName() == "AnalyticBRDFModel"){

				lobeName = QString(e.attribute("name").toStdString().c_str());
				specularLobeDirection = QString(e.attribute("specularLobeDirection").toStdString().c_str());

				// add parameterization
				
				QString type(e.attribute("lobeType").toStdString().c_str());

				if(type == "Isotropic")
					lobeType = Isotropic;
				else
					lobeType = Anisotropic;

				// get child
				QDomNode analyticModel = e.firstChild();

				if(!analyticModel.isNull()){

					while(!analyticModel.isNull()){
						
						QDomElement insideElement = analyticModel.toElement();

						if(insideElement.tagName() == "Parameters"){

							QDomNode param = insideElement.firstChild();

							if(!param.isNull()){
								while(!param.isNull()){
									
									LobeParameter lp(param.toElement());

									parameters.push_back(lp);
																		
									param = param.nextSibling();
								}

							}

						}else if (insideElement.tagName() == "Equation"){
							equation = insideElement.text();
						}else if (insideElement.tagName() == "Equation_GLSL"){
							equationGLSL = insideElement.text();
						}else if (insideElement.tagName() == "PDF_GLSL"){
							pdfGLSL = insideElement.text();
						}else if (insideElement.tagName() == "ImportanceSampling_GLSL"){
							importanceSamplingGLSL = insideElement.text();
						}else if (insideElement.tagName() == "ComputeMirrorDirection_GLSL"){
							mirrorDirectionGLSL = insideElement.text();
						}

						analyticModel = analyticModel.nextSibling();
					}
				
				}
			}
		}
	

	nParameters = parameters.size();
	
	// clean equation string.
	equation.replace("\t"," ");
	equation.replace("\n"," ");
	equation.replace("\r"," ");

	equationGLSL.replace("\r"," ");
	pdfGLSL.replace("\r"," ");
	importanceSamplingGLSL.replace("\r"," ");
	mirrorDirectionGLSL.replace("\r"," ");

	equationGLSL.replace("U+003C","<");
	pdfGLSL.replace("U+003C","<");
	importanceSamplingGLSL.replace("U+003C","<");
	mirrorDirectionGLSL.replace("U+003C","<");

	// Define the variable factory
	parser.SetVarFactory(AddVariable, &parser);


	parser.DefineVar(_T("ux"), &ux);
	parser.DefineVar(_T("uy"), &uy);
	parser.DefineVar(_T("uz"), &uz);
	parser.DefineVar(_T("vx"), &vx);
	parser.DefineVar(_T("vy"), &vy);
	parser.DefineVar(_T("vz"), &vz);
	parser.DefineVar(_T("theta_i"), &theta_i);
	parser.DefineVar(_T("phi_i"), &phi_i);
	parser.DefineVar(_T("theta_o"), &theta_o);
	parser.DefineVar(_T("phi_o"), &phi_o);

	if(parameters.size() > 0){
		paramVal = new double[nParameters];

		//define the parameters in the parser
		for (int i=0; i<nParameters; i++)
		{
			paramVal[i] = parameters[i].getDefaultVal();
			
			parser.DefineVar(_T(parameters[i].getName().toStdString().c_str()), &paramVal[i]);
		}
	}


	// initialize parser
	parser.SetExpr(equation.toStdString().c_str());

	cout << parser.Eval() << endl;

}

ParametricLOBE::~ParametricLOBE()
{
	if(paramVal != NULL){
		delete []paramVal;
		paramVal = NULL;
	}
}






/**

Set the lower and upper boundaries

*/
void 
ParametricLOBE::initForOptimize(double *lb, double *ub, int combinationType){
	if (lobeName != "Lambertian" || combinationType == 1)
	{
		for (int i = 0; i<nParameters; i++){
			lb[i] = parameters[i].getLowBound();
			ub[i] = parameters[i].getHighBound();
		}
	}
}


/**

Show the parameters of the model

*/
void 
ParametricLOBE::showParameters(double *p,  int combinationType){
	if (lobeName != "Lambertian" || combinationType == 1)
	{
		for (int i = 0; i<nParameters; i++)
			cout << parameters[i].getName().toStdString() << " " << p[i] << " ";
		cout << endl;

		cout << "--------------------------------------------------------------------" << endl;
	}
}

void 
ParametricLOBE::writeXML(QDomDocument *doc, QDomElement *parent, double *p, int combinationType){

	QDomElement model = doc->createElement("Model");
	model.appendChild(doc->createTextNode(lobeName));

	QDomElement type = doc->createElement("Type");
	type.appendChild(doc->createTextNode((lobeType==Isotropic)?"Isotropic":"Anisotropic"));

	QDomElement params = doc->createElement("Parameters");

	if (lobeName != "Lambertian" || combinationType == 1){
		for (int i=0; i<nParameters; i++)
			params.setAttribute(parameters[i].getName(),p[i]);
	}else{
		params.setAttribute("value",1.);
	}


	parent->appendChild(model);
	parent->appendChild(type);
	parent->appendChild(params);
}

void 
ParametricLOBE::loadXML(QDomElement param, double *p, int combinationType){
	if (lobeName != "Lambertian" || combinationType == 1)
	{
		//read the data for all the Lobe parameters
		for (int i=0; i<nParameters; i++)
			p[i] = param.attribute(parameters[i].getName()).toFloat();
	}
}

double 
ParametricLOBE::evalLOBE(double *param, Ogre::Vector3 u, Ogre::Vector3 v, int combinationType){

	if (lobeName != "Lambertian" || combinationType == 1)
	{
		ux = u[0];
		uy = u[1];
		uz = u[2];
		vx = v[0];
		vy = v[1];
		vz = v[2];
		
		Ogre::Vector2 theta_in(cart2spherical(u));
		Ogre::Vector2 theta_out(cart2spherical(v));

		phi_i = theta_in[0];
		theta_i = theta_in[1];

		phi_o = theta_out[0];
		theta_o = theta_out[1];

		for(int i=0; i<nParameters; i++)
			paramVal[i] = param[i];

		return parser.Eval();
	}else
		return 1.;
}




double 
ParametricLOBE::fresnel(double F0, double dotVH)
{
	return F0 + (1.-F0) * pow(1.-dotVH, 5.);
}

/**
By default, we put cosine lobe importance sampling by default. With exponent = 5

*/
void 
ParametricLOBE::computeNImportanceSamples(double *p, float *samples, int nSamples){

	// phi, theta

	float n = 5;

	for(int i=0; i<nSamples; i++){
		// phi, theta	
		samples[i*2+0] = 2.*Ogre::Math::PI*samples[i*2+0];

		samples[i*2+1] = acos(Ogre::Math::Pow(samples[i*2+1],1./(n+1.)));
	}


}

void 
ParametricLOBE::computeImportanceSamplingOutgoingVectors(double *p,float *samples,int nSamples, std::vector<Ogre::Vector3> *directions, Ogre::Vector3 v){

	
	for(int i=0; i<nSamples; i++){
		
		Ogre::Vector3 v = spherical2cart(Ogre::Vector2(samples[i*2+0],samples[i*2+1]));
		
		// do something
		directions->push_back(v);
	}


}

void 
ParametricLOBE::computeImportanceSamplingOutgoingVectorsHalfwayVector(double *p,float *samples,int nSamples, std::vector<Ogre::Vector3> *directions, Ogre::Vector3 v){


	for(int i=0; i<nSamples; i++){

		// access the correct sample, represents the halfway vector
		Ogre::Vector3 h = spherical2cart(Ogre::Vector2(samples[i*2+0],samples[i*2+1]));

		if(h.dotProduct(v) < 0.)
			h = -h;



		// compute the incident direction by reflecting about h
		Ogre::Vector3 u = -v +2. * h.dotProduct(v) * h;

//		if(u[2] < 0.)
//			u[2] = -u[2];

		float pdf = computePDF(p,u,v);

		// do something
		directions->push_back(u*pdf);


	}
}

LobeParameter
ParametricLOBE::getLobeParameter( int i )
{
	return parameters[i];
}

void ParametricLOBE::setDefaultParameters( double *param, int combinationType)
{
	if (lobeName != "Lambertian" || combinationType == 1)
	{
		for (int i=0; i<nParameters; i++)
		{
			param[i] = parameters[i].getDefaultVal();
		}
	}
}

int ParametricLOBE::getNParameters( int combinationType )
{
	if (lobeName != "Lambertian" || combinationType == 1)
		return nParameters;
	else
		return 0;
}

QString ParametricLOBE::getEquationGLSL( int combinationType )
{
	if (lobeName != "Lambertian" || combinationType == 1)
		return equationGLSL;
	else
		return QString("float LambertianBRDF(vec3 u, vec3 v, inout int j)\n {\n	return 1.; \n }\n");
}