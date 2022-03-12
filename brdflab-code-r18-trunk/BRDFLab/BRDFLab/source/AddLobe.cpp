#include "AddLobe.h"


AddLobe::AddLobe(QObject *parent):
	QObject(parent),
	m_lobe(None)
	{
		brdf = dynamic_cast<AnalyticBRDF*>(parent);
	}

void 
AddLobe::setLobe(const LobesOptions l){

	switch(l){
		case Lambertian:
			brdf->addLobe(ParametricLOBE::Lambertian);
			break;
		case BlinnPhong:
			brdf->addLobe(ParametricLOBE::BlinnPhong);
			break;
		case LafortuneIso:
			brdf->addLobe(ParametricLOBE::LafortuneIso);			
			break;
		case LafortuneAniso:
			brdf->addLobe(ParametricLOBE::LafortuneAniso);			
			break;
		case AshikhminShirley:
			brdf->addLobe(ParametricLOBE::AshikhminShirley);			
			break;
	}

	m_lobe = None;
}