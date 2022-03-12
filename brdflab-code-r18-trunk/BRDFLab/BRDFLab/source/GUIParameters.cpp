#include "GUIParameters.h"

GUIParameters::GUIParameters(QObject *parent):
	QObject(parent)
{
	brdf = dynamic_cast<AnalyticBRDF*>(parent);
}

void
update(){

	brdf->guiUpdated();

}


