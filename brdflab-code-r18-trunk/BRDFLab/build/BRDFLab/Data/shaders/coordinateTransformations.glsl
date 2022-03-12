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


// Spherical to Cartesian transformation
vec3 spherical2cart(vec2 uv)
{
	vec3 val = vec3(0.,0.,0.);

	val.x = sin(uv.y)*cos(uv.x);
	val.y = sin(uv.y)*sin(uv.x);
	val.z = cos(uv.y);

	return val;
}

// Cartesian to Spherical transformation
vec2 cart2spherical(vec3 pos)
{
	// Code from Colbert et al.

	vec3 npos = normalize(pos);
	
	float phi = atan(npos.y,npos.x);
	phi = (phi>0.)?phi:((2.*3.1415926535) + phi);
	
	float theta = acos(npos.z);
	
	return vec2(phi,theta);

//	vec2 uv;
//
//	float p = sqrt(pos.x*pos.x+pos.y*pos.y+pos.z*pos.z);
//	float s = sqrt(pos.x*pos.x+pos.y*pos.y);
//
//	if(0 <= pos.x){
//		uv.x = asin(pos.y/s);
//	}else{
//		uv.x = 3.141519 - asin(pos.y/s);
//	}
//
//	uv.y = acos(pos.z/p);
//
//	return uv;
}

