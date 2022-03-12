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

#include "SubsurfaceParticleBRDF.h"

using namespace std;

using namespace SCATMECH;

SubsurfaceParticleBRDF::SubsurfaceParticleBRDF():
	ScatmechBRDF(1,"SubsurfaceParticle")
{

	brdfType = QString("SubsurfaceParticle");

	//create the model
/*	
	guiParams.push_back(new BasicParameters());
	guiParams.push_back(new MieScattererParameters());
	guiParams.push_back(new SubsurfaceParticleParameters());

	guiUpdated();
*/
}


void
SubsurfaceParticleBRDF::guiUpdated(){
/*
	BasicParameters *m_BasicOptions = dynamic_cast<BasicParameters*>(guiParams[0]);
	MieScattererParameters *m_MieScattererOptions = dynamic_cast<MieScattererParameters*>(guiParams[1]);
	SubsurfaceParticleParameters *m_SpecificSubsurfaceParticle = dynamic_cast<SubsurfaceParticleParameters*>(guiParams[2]);

	optical_constant med(m_MieScattererOptions->medium().X,m_MieScattererOptions->medium().Y);
	optical_constant sp(m_MieScattererOptions->sphere().X,m_MieScattererOptions->sphere().Y);
	
	dielectric_function medium(med);

	dielectric_function sphere(sp);

//	if(ptr_sphericalScatterer != NULL)
//		delete ptr_sphericalScatterer;

	ptr_sphericalScatterer = new MieScatterer();
	
	// Free space scatterer parameters
	ptr_sphericalScatterer->set_lambda(m_MieScattererOptions->lambda());
	
	ptr_sphericalScatterer->set_medium(medium);

	//// SphericalScatterer params
	((SphericalScatterer *)ptr_sphericalScatterer)->set_sphere(sphere);
	((SphericalScatterer *)ptr_sphericalScatterer)->set_radius(m_MieScattererOptions->radius());

	if(brdfModel != NULL)
		delete brdfModel;

	brdfModel = new Subsurface_Particle_BRDF_Model();


	Subsurface_Particle_BRDF_Model *model = dynamic_cast<Subsurface_Particle_BRDF_Model*>(brdfModel);

	// Following 5 parameters are set at Subsurface_Particle_BRDF level

	model->set_parameter("depth",m_SpecificSubsurfaceParticle->depth());

//	ptr_sphericalScatterer->print_parameters(std::cout);
	
	Free_Space_Scatterer_Ptr sphericalScatterer_ptr = Free_Space_Scatterer_Ptr(ptr_sphericalScatterer);
//	sphericalScatterer_ptr->print_parameters(std::cout);
	model->set_scatterer(sphericalScatterer_ptr);
	sphericalScatterer_ptr->print_parameters(std::cout);
	model->set_parameter("stack","");

//			brdfModel->set_parameter("alpha",m_SpecificSubsurfaceParticle->alpha());
	model->set_alpha(m_SpecificSubsurfaceParticle->alpha());

//			brdfModel->set_parameter("beta",m_SpecificSubsurfaceParticle->beta());
	model->set_beta(m_SpecificSubsurfaceParticle->beta());

	// Following is set at Local_BRDF level
	model->set_parameter("density",m_SpecificSubsurfaceParticle->density());

	// Following 3 are set at BRDF level
	COMPLEX subs1(m_BasicOptions->substrate().X, m_BasicOptions->substrate().Y);
	optical_constant subs2(subs1); //peta igual..
	dielectric_function subs3(subs2);
	model->set_substrate(subs3);
//			brdfModel->set_parameter("substrate",subs); ///////////////////////canviar!!!!
//	brdfModel->set_parameter("substrate","(1.0,0)");

	model->set_type(m_BasicOptions->type());
	model->set_lambda(m_BasicOptions->lambda());

	model->print_parameters(std::cout);

	displayBRDF(textureResolution[0]);


	emit brdfModified();
*/
}



void 
SubsurfaceParticleBRDF::writeXML(QDomDocument *doc, QDomElement *parent){
/*
	BasicParameters *m_BasicOptions = dynamic_cast<BasicParameters*>(guiParams[0]);
	MieScattererParameters *m_MieScattererOptions = dynamic_cast<MieScattererParameters*>(guiParams[1]);
	SubsurfaceParticleParameters *m_SpecificSubsurfaceParticle = dynamic_cast<SubsurfaceParticleParameters*>(guiParams[2]);


	QDomElement material = doc->createElement("Material");

	QDomElement type = doc->createElement("Type");
	type.appendChild(doc->createTextNode(brdfType));

	QDomElement params = doc->createElement("Parameters");

	QDomElement basicParams = doc->createElement("BasicParameters");
	basicParams.setAttribute("Lambda",m_BasicOptions->lambda());
	basicParams.setAttribute("SubstrateX",m_BasicOptions->substrate().X);
	basicParams.setAttribute("SubstrateY",m_BasicOptions->substrate().Y);
	basicParams.setAttribute("Type",m_BasicOptions->type());

	QDomElement mieParams = doc->createElement("MieScattererParameters");
	mieParams.setAttribute("Lambda", m_MieScattererOptions->lambda());
	mieParams.setAttribute("MediumX", m_MieScattererOptions->medium().X);
	mieParams.setAttribute("MediumY", m_MieScattererOptions->medium().Y);
	mieParams.setAttribute("Radius", m_MieScattererOptions->radius());
	mieParams.setAttribute("SphereX", m_MieScattererOptions->sphere().X);
	mieParams.setAttribute("SphereY", m_MieScattererOptions->sphere().Y);

	QDomElement subsurfaceParticleParams = doc->createElement("SubsurfaceParticleParameters");
	subsurfaceParticleParams.setAttribute("Density",m_SpecificSubsurfaceParticle->density());
	subsurfaceParticleParams.setAttribute("Depth",m_SpecificSubsurfaceParticle->depth());
	subsurfaceParticleParams.setAttribute("Alpha",m_SpecificSubsurfaceParticle->alpha());
	subsurfaceParticleParams.setAttribute("Beta",m_SpecificSubsurfaceParticle->beta());

	params.appendChild(basicParams);
	params.appendChild(mieParams);
	params.appendChild(subsurfaceParticleParams);

	material.appendChild(type);
	material.appendChild(params);

	parent->appendChild(material);
*/
}

void 
SubsurfaceParticleBRDF::loadXML(QDomElement params){

	/*
	BasicParameters *m_BasicOptions = dynamic_cast<BasicParameters*>(guiParams[0]);
	MieScattererParameters *m_MieScattererOptions = dynamic_cast<MieScattererParameters*>(guiParams[1]);
	SubsurfaceParticleParameters *m_SpecificSubsurfaceParticle = dynamic_cast<SubsurfaceParticleParameters*>(guiParams[2]);

	QDomNode n = params.firstChild();
	while(!n.isNull()) {  // BRDF file
		QDomElement e = n.toElement();
		
		if(!e.isNull()) {

			if(e.tagName() == "BasicParameters"){
			
				m_BasicOptions->setLambda(e.attribute("Lambda").toFloat());
				m_BasicOptions->setSubstrate(Vec2f(e.attribute("SubstrateX").toFloat(),e.attribute("SubstrateY").toFloat()));
				m_BasicOptions->setType(e.attribute("Type").toInt());
			}

			if(e.tagName() == "MieScattererParameters"){
				m_MieScattererOptions->setLambda(e.attribute("Lambda").toFloat());
				m_MieScattererOptions->setMedium(Vec2f(e.attribute("MediumX").toFloat(),e.attribute("MediumY").toFloat()));
				m_MieScattererOptions->setRadius(e.attribute("Radius").toFloat());
				m_MieScattererOptions->setSphere(Vec2f(e.attribute("SphereX").toFloat(),e.attribute("SphereY").toFloat()));
			}
		
			if(e.tagName() == "SubsurfaceParticleParameters"){
				m_SpecificSubsurfaceParticle->setDensity(e.attribute("Density").toFloat());
				m_SpecificSubsurfaceParticle->setDepth(e.attribute("Depth").toFloat());
				m_SpecificSubsurfaceParticle->setAlpha(e.attribute("Alpha").toFloat());
				m_SpecificSubsurfaceParticle->setBeta(e.attribute("Beta").toFloat());
			}

		}
		n = n.nextSibling();
	}

	guiUpdated();
	*/
}


