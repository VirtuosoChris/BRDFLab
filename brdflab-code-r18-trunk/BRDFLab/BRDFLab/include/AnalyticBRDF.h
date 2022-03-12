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


#ifndef AnalyticBRDF_H
#define AnalyticBRDF_H

#include "BRDF.h" 

#include <QWidget>
#include <QInputDialog>
#include <QDir>
#include <QtCore/QLocale>
#include <QTextStream>
#include <QtProperty>

#include "ParametricLOBE.h"
#include "LobeParameter.h"

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"


class AnalyticBRDF: public BRDF
{
	Q_OBJECT

public:
	enum LobeCombination {Linear = 0, Fixed = 1};
	
	AnalyticBRDF(int channels,LobeCombination lc,QString name);
	~AnalyticBRDF();

	void setShadersToDefault();

	void setGUIManagers(QtVariantPropertyManager *vM,QtTreePropertyBrowser *vE);
	
	bool linearCombination(){ return (combinationType==Linear); }
	void addLobe(QString name);
	void evalBRDFLobes(double *p, Ogre::Vector3 u, Ogre::Vector3 v, double *vArray);
	void evalBRDF(double *p, Ogre::Vector3 u, Ogre::Vector3 v, double *value);
	void showParameters();
	void initForOptimize();
	void loadXML(QDomElement param);
	void writeMaterialXML(QDomDocument *doc, QDomElement *parent);
	void writeXML(QDomDocument *doc, QDomElement *parent);
	void setMaterialParameters(double *p);
	void setLightParameters(Ogre::String materialName);
	void setBRDFAsFitted(bool option){ fitted = option; setCubeRoot(cubeRoot);}
	void setParametersToGUI();
	void setEnvMapParameters(Ogre::String materialName);
	int getLobeIndex(QString name);
	void unlinkGUI();
	void linkGUI();
	
	//GUI
	void recreateParameterGUI(bool enabled = true);
	
	void generateSampleDirections(Ogre::Vector3 u, std::vector<Ogre::Vector3> *directions);
	void generateShaders();

	
		
	int getNLobes() { return lobes.size(); }
	int getNParameters() { return nParameters; }
	double* getParameters() { return params; }
	double* getScalingFactors() { return scaleFactors; }
	double* getLowerBound() { if(lb == NULL) initForOptimize(); return lb; }
	double* getUpperBound() { if(ub == NULL) initForOptimize(); return ub; }
	QString getMaterialName() {return materialName; }
	
	void resetMaterial();
	void clear();


	public slots:
			void guiUpdated();
			void setMaterialLobeViewer();
			void setMaterialObjectRenderer();
			void addLobe();
			void changeDisplayChannel(int x);
			void setCubeRoot(bool cr);
			void updateMaterial();

signals:
		void updateImportanceSampleDirections();

public:

	std::vector<ParametricLOBE*> lobes; // Analytic BRDF is a Linear/Fixed combination of Lobes
						  // Fixed Combination: brdf[k](u,v) = sum_i *lobes[i](u,v,p[k])
	double *scaleFactors; // Linear Combination: brdf[k](u,v) = sum_i scaleFactors[i*nChannels+k]*lobes[i](u,v,p)
						  //       Size of the scaleFactors is nChannels*lobes.size()



	int nParameters;
	double *params, *lb, *ub; // For Fixed Combination: Parameters varies with Channel
						      // For Linear Combination: Parameter are independent of Channel
							  // The storage order is: 
							  //   For each lobe, 
							  //       parameters for channel 1 followed by parameters for channel 2 and so on
	LobeCombination combinationType;
	QString materialName;
	
	bool fitted;

private:
	void updateParameterVector(int nParametersOld);
	void addFileContent(QTextStream *out, QString path);
	void generateDisplayLobeShader(bool returnBlack = false);
	void generatePointLightShader(bool returnBlack = false);
	void generateEnvMapShader(bool returnBlack = false);
	void writeGLSLEquations( QTextStream* out,  bool writeLambertian = true, bool writeOtherEquations = false );
	void writeComputeBRDFFunction( QTextStream* out);
	
	void setParametersLobeDisplayShader( Ogre::GpuProgramParametersSharedPtr gpuVertexParam );

	void writeEnvMapLobes( QTextStream *out );
	QChar getPDFVectorParameter( QString pdfGLSL );
	static std::vector<ParametricLOBE*> avaliableLobes;
	QtVariantPropertyManager *variantManager;
	QtTreePropertyBrowser *variantEditor;


};

#endif

