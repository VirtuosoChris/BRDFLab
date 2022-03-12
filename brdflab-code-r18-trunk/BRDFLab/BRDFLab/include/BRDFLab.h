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


#ifndef BRDFLab_H
#define BRDFLab_H

//include the GUI
#include "../../build/BRDFLab/ui_MainWindow.h"

//include the syntax highlight
#include "XML_Editor.h"
#include "Console.h"


//BRDFs
#include "ParametricLOBE.h"


//Measured data
#include "MERLBRDFDatabase.h"
#include "CarPaintBRDFDatabase.h"
#include "RawDataBRDF.h"


//Fitting methods
#include "LMFitting.h"


#include <QMainWindow>

#include <QFileDialog>
#include <QTextEdit>
#include <QTextStream>

#include <QVBoxLayout>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProgressDialog>

#include "BRDFViewer.h"
#include "PropertyEditorWidget.h"
#include "PropertyBRDFEditorWidget.h"
#include "ObjectRenderer.h"
#include "OgreSimulationWindow.h"


//scatmech models
#include "SubsurfaceParticleBRDF.h"
#include "DoubleInteractionBRDF.h"

// simulated BRDF
#include "ReliefBRDF.h"
#include "SpheresBRDF.h"
#include "TrianglesBRDF.h"
#include "GroovesBRDF.h"


#include "OgreSurfaceViewer.h"


class BRDFViewer;



class BRDFLab : public QMainWindow, public Ui::MainWindow
{

	Q_OBJECT

public:
	BRDFLab();
	~BRDFLab();
	BRDF* getMainBRDF() { return mainBRDF; }
	AnalyticBRDF* getFittingBRDF() { return fittedBRDF; }
	void convertBobModel();
	bool downloadFile(QString url, QString pathToFile);

	private slots:
		void open();
		void xmlView();
		void brdfView();
		void surfaceVisualizationView();
		void objectRenderer();
		void newMERL(QString modelName = "");
		void newCarPaint(QString modelName = "");
		void newRawData(QString modelName = "");
		void newParametric(QString name="", QString combinationType="", int nChannels=0 );
		void newScatmech(QString model ="");
		bool saveAs();
		void save();
		void newFitting(QDomElement *e=NULL);
//		void fit();
		void resizeEvent( QResizeEvent * event );
		void hideWindows();
		void newHeightField(QString texName="");		
		void newSpheres(int nSpheres = 0, float radius = 0);
		void newTriangles();
		void newGrooves(double width=0., double spacing=0., double botAngle=0., double angle=0., int groovesPerSide=0, int texelsPerGroove=0);
		void exportBRDFData();
		void about();
		void setActiveBRDF(int m);
		void lightTypeChanged(int l);
		void updateSurfaceViewer(int m);
		void updateBRDFViewer();
		void updateProgressBar(qint64 current,qint64 max);
		void setBRDFModified();
		void optimizeAll();



		
private:
	
	void enableBRDFOptions();
	void disableBRDFOptions();

	bool createNewModel();

	void cleanEnvironment();
	void writeXML();
	void writeXMLProgramInformation(QDomDocument *doc, QDomElement *parent);

	// save/restore info
	bool brdfModified;
	QString fileName;

	// main widget
	QWidget *mainWidget;

	// mainBRDF
	BRDF *mainBRDF;

	// fitted BRDF
	AnalyticBRDF *fittedBRDF;
	

	// download files
	QProgressDialog *bar;

	// GUI
	PropertyBRDFEditorWidget *pew;
	PropertyEditorWidget *pew2 ;
	
	// XML hightlight
	XmlHighlighter *highlighter;

	//displaying information windows
	QTextEdit *textEdit;
	BRDFViewer *window;
	ObjectRenderer *objectWindow;
	OgreSurfaceViewer *surfaceViewer;
	OgreGroovesWindow *ogw;
};

#endif