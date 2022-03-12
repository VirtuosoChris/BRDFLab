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


#ifndef TrianglesBRDF_H
#define TrianglesBRDF_H

#include "GPUSimulationBRDF.h" 
#include "OgreSurfaceViewer.h"


#include <QTime>

class OgreSurfaceViewer;

class TrianglesBRDF: public GPUSimulationBRDF
{
	Q_OBJECT
public:

	TrianglesBRDF();

	~TrianglesBRDF();
	
/*
		if(Ogre::TextureManager::getSingleton().resourceExists(textureName)){
			Ogre::TextureManager::getSingleton().remove(textureName);

		}
*/		
	

	int setMaterialInfo(Ogre::String materialName);

	void writeXML(QDomDocument *doc, QDomElement *parent){}

	float createDistribution();
	void createTrianglesFromDistribution(float min);
	void createTextures();
	void updateMaterial();
	void normalDistribution(float mu, float sigma, float *A, float length);
	double radicalInverse(int i);

	int* getIndex() {return index; }
	Ogre::Vector3* getVertex() {return vertex; }
	Ogre::Vector3* getVertexNormals() {return vertexNormal; }
	int getNTriangles() {return nTriangles; }
	int getNVertex() {return width*width; }
	int getWidth() {return width; }
	int getIndexTexWidth() {return indexTexWidth; }
	float getMaxHeight() {return maxHeight; }
	void recreateParameterGUI();

	public slots:
		void guiUpdated();

signals:
		void updateSurfaceViewer(int m);
		void updateBRDFViewer();


protected:
	Ogre::Vector3 computeNormal(int v0, int v1, int v2);


	float *points;
	
	int indexTexWidth;

	Ogre::Vector3 *vertex;
	Ogre::Vector3 *triangleNormal;
	Ogre::Vector3 *vertexNormal;
	int *index;
	int nTriangles;
	float maxHeight;

	Ogre::String vertexTexture;
	Ogre::String indexTexture;

	int width;
	int distribution;
	float scale;
	
};



#endif