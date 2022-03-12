#include "RawDataBRDF.h"

using namespace std;

RawDataBRDF::RawDataBRDF( int channels, QString dataFileName ) :
BRDF(channels,QString("RawData")),
fileName(dataFileName)
{

}




RawDataBRDF::~RawDataBRDF()
{
	u.clear();
	v.clear();
	y.clear();
}


void
RawDataBRDF::readMeasuredData(){

	// clear old data, if any
	u.clear();
	v.clear();
	y.clear();

	cout << fileName.toStdString() << endl;

	// open file
	QFile file(fileName);

	if (!file.open(QIODevice::ReadOnly))
		return;

	QTextStream in(&file);
	float thetaI, phiI, thetaO, phiO, r, g, b;
	char coma;

	cout << "init ... " << endl;

	// store data on u v and y	
	while(!in.atEnd()){
		

		in >> thetaI;
		in >> coma;

		in >> phiI;
		in >> coma;

		in >> thetaO;
		in >> coma;

		in >> phiO;
		in >> coma;

		in >> r;
		in >> coma;

		in >> g;		
		in >> coma;

		in >> b;
		in >> coma;

	

	
		cout << thetaI << " " << phiI << " " << thetaO << " " << phiO << " " << r << " " << g << " " << b << endl;

		u.push_back(BRDF::spherical2cart(Ogre::Vector2(phiI,thetaI)));
		v.push_back(BRDF::spherical2cart(Ogre::Vector2(phiO,thetaO)));
		y.push_back(Ogre::Vector3(r,g,b));
	}


	// close file
	file.close();

}


