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

#ifndef BRDF_H
#define BRDF_H

//include OGRE for using Vector3 types and that stuff.
#include <Ogre.h>

#include <QWidget>
#include <QString>
#include <QDomDocument>
#include <iostream>

class BRDF : public QWidget
{
	Q_OBJECT

public:
	enum Channels {All, Red, Green, Blue};

//	Channels getChannels() { return Channel; }
	int getNChannels() {return nChannels; }

	static Ogre::Vector3 spherical2cart(Ogre::Vector2 i){

		// 0 -> phi
		// 1 -> theta

		Ogre::Vector3 out;

		out[0] = sin(i[1])*cos(i[0]);
		out[1] = sin(i[1])*sin(i[0]);
		out[2] = cos(i[1]);
		
		return out;
	}


	static Ogre::Vector2 cart2spherical( Ogre::Vector3 pos )
	{

		// 0 -> phi
		// 1 -> theta

		// code from colbert et al.
		pos.normalise();
		
		Ogre::Radian phi = Ogre::Math::ATan2(pos[1],pos[0]);

		float fphi = phi.valueRadians();

		fphi = (fphi>0.)?fphi:((2.*Ogre::Math::PI) + fphi);
		
		Ogre::Radian theta = Ogre::Math::ACos(pos[2]);

		
		return Ogre::Vector2(fphi,theta.valueRadians());
	}



	QString getMaterialType(){ return brdfType;}
	virtual QString getMaterialName()=0;
	virtual void writeXML(QDomDocument *doc, QDomElement *parent)=0;


	~BRDF(){
	
	}

	public slots:
		virtual void changeDisplayChannel(int x){
			displayChannel = x;
		}
		virtual void setCubeRoot(bool cr)=0;
		void setActiveBRDF(bool a){ active = a; }
		void setMaterial(int mat){ 

			materialToUpdate = mat;
			updateMaterial();
		}
		int getMaterialToUpdate(){
			return materialToUpdate;
		}
		virtual void updateMaterial() = 0;


signals:
		void brdfModified();

protected:
//	BRDF();
	BRDF(int channels, QString brdftype):
		nChannels(channels),
		brdfType(brdftype),
		displayChannel(0),
		cubeRoot(false),
		materialToUpdate(-1),
		active(false)
	{}






protected:

	int nChannels;
	int displayChannel;
	int materialToUpdate;

	bool cubeRoot;
	bool active;


	QString brdfType;

};


#endif