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
#version 120

attribute vec4 vertex;

//uniform vec3 displayChannel;
//uniform vec3 incidentVector;					// incident direction

uniform float minV;
uniform float maxV;

uniform float cubeRoot;
uniform float applyLog;

uniform sampler2D brdfData;

uniform float thetaSize;

// definition of the coordinateTransformations.glsl file functions used
vec3 spherical2cart(vec2);
vec2 cart2spherical(vec3);

#define PI 3.14159265

void main()																					
{																							
	
	// Incident and exitant directions
	vec3 v = vertex.xyz; // exitant position // viewer

	vec2 outgoing = cart2spherical(v); // phi_o, theta_o

	
	vec2 sphericalCoordsVertex = outgoing;	
	
	// normalize to access the texture
	outgoing.x /= 2.*PI;
	outgoing.y /= PI/2.;
	
	// increment half a texture's pixel to read the center of the texel when accessing the 3D texture.
	vec2 vary = vec2(1/(thetaSize),1./(thetaSize));

//	outgoing.x = mix(0.+vary.x, 1.-vary.x, outgoing.x);
	outgoing.y = mix(0.+vary.x, 1.-vary.x, outgoing.y);
	
	vec2 positionPhiInc = outgoing + vec2(1./thetaSize,0.);
	
//	if(positionPhiInc.x > 1.)
//		positionPhiInc.x -= 1.;
	
	vec2 positionThetaInc = outgoing + vec2(0.,1./thetaSize);

//	if(positionThetaInc.y > 1.)
//		positionThetaInc.y -= 1.;

	
	float intensity = texture2D(brdfData,outgoing).x;
	float positionPhiIncIntensity = texture2D(brdfData,positionPhiInc).x;
	float positionThetaIncIntensity = texture2D(brdfData,positionThetaInc).x;
	
	// compute displacement of the point according to the channel
	float mainIntensity = ((1.-applyLog)*pow(intensity,cubeRoot)+applyLog*log(intensity+1.))*100.;
	float phiIntensity = ((1.-applyLog)*pow(positionPhiIncIntensity,cubeRoot)+applyLog*log(positionPhiIncIntensity+1.))*100.;
	float thetaInstensity = ((1.-applyLog)*pow(positionThetaIncIntensity,cubeRoot)+applyLog*log(positionThetaIncIntensity+1.))*100.;
	
	// compute vertex position and auxiliary points in world space.
	vec3 v0 = vertex.xyz * max(mainIntensity,0.);
		
	// convert the texture access coordinates to spherical coordinates, and from spherical coordinates to cartesian coordinates. 
	// after that, multiply the resulting vertex by the intensity to get the final position in world space.
	vec3 v1 = spherical2cart(vec2(positionPhiInc.x*2.*PI,positionPhiInc.y*PI/2.))*phiIntensity;
	vec3 v2 = spherical2cart(vec2(positionThetaInc.x*2.*PI,positionThetaInc.y*PI/2.))*thetaInstensity;
	
	// normal computation
	vec3 v01 = normalize(v1-v0);
	vec3 v02 = normalize(v2-v0);
	
	vec3 normal = cross(v02,v01);

	// set the final position for the vertex
	gl_Position = gl_ModelViewProjectionMatrix*vec4(v0,1.);
	
	// pass the information to the fragment shader
	gl_TexCoord[0] = vec4(v0, 1.); // vertex position
	gl_TexCoord[1] = vec4(normal,1.); // normal
	
	
	//color...
	float minA = pow(minV,cubeRoot)*100.;
	float maxA = pow(maxV,cubeRoot)*100.;


	if(mainIntensity>maxA)
		mainIntensity = maxA;
	if(mainIntensity<minA) 
		mainIntensity = minA;


	mainIntensity = (mainIntensity-minA)/(maxA-minA);

	if(mainIntensity<=0.25){
		gl_TexCoord[2] = vec4(0.,4.*mainIntensity,1.,1.);
	}else if(mainIntensity<=0.5){
		gl_TexCoord[2] = vec4(0.,1.,2.-4.*mainIntensity,1.);
	}else if(mainIntensity<=0.75){
		gl_TexCoord[2] = vec4(4.*mainIntensity-2.,1.,0.,1.);
	}else{
		gl_TexCoord[2] = vec4(1.,4.-4.*mainIntensity,0.,1.);
	}

}
