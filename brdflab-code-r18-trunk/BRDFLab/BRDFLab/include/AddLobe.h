#ifndef AddLobe_H_
#define AddLobe_H_

#include <Qt/qobject.h>

#include "AnalyticBRDF.h"
#include "ParametricLOBE.h"

class AnalyticBRDF;


class AddLobe : public QObject
{
	Q_OBJECT

	Q_PROPERTY(LobesOptions Model READ lobe WRITE setLobe DESIGNABLE true USER true)
	Q_ENUMS(LobesOptions)
	

public:

	enum LobesOptions { None, Lambertian, BlinnPhong, LafortuneIso, LafortuneAniso, AshikhminShirley };

	AddLobe(QObject* parent = 0);

	//virtual ~AddLobe();

	LobesOptions lobe() const { return m_lobe; }
	void setLobe(const LobesOptions l);

private:

	AnalyticBRDF *brdf;
	LobesOptions m_lobe;

};
#endif
