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

attribute vec4 in_vertex;

uniform vec3 displayChannel;
uniform vec3 incidentVector;					// incident direction

uniform float cubeRoot;
uniform float applyLog;

uniform sampler3D brdfData;
uniform sampler1D maxValues;

// definition of the coordinateTransformations.glsl file functions used
vec3 spherical2cart(vec2);
vec2 cart2spherical(vec3);

// Transform reflection value (cube root, logarithmic, ...)
float transformReflectionValue(float value)
{
	return ((1.-applyLog)*pow(value, cubeRoot) + applyLog*log(value+1.))*100.;
}


#define PI 3.14159265

void main()																					
{																							
	
	// Incident and exitant directions
	vec3 u = normalize(incidentVector); // incident position // light
	vec3 v = in_vertex.xyz; // exitant position // viewer

// transform the vertex position to spherical coordinates
	vec2 incident = cart2spherical(u); // phi_i, theta_i
	vec2 outgoing = cart2spherical(v); // phi_o, theta_o
	
	// rotation about phi angle.
	outgoing.x -= incident.x;
	
//	if(outgoing.x < 0)
//		outgoing.x += 2.*PI;
	
	vec2 sphericalCoordsVertex = outgoing;	
	
	// normalize to access the texture
//	incident.x /= 2.*PI;
	incident.y /= PI/2.;
	outgoing.x /= 2.*PI;
	outgoing.y /= PI/2.;
	
	// get the max intensity with this incoming theta
	float thetaMaxVary = 1./90.;
	thetaMaxVary= mix(0.+thetaMaxVary, 1.-thetaMaxVary, incident.y);
	float incidentThetaIndex = length(texture1D(maxValues,thetaMaxVary).rgb*displayChannel);
	
	// increment half a texture's pixel to read the center of the texel when accessing the 3D texture.
	vec2 vary = vec2(1/180.,1./90.);

	outgoing.x = mix(0.+vary.x, 1.-vary.x, outgoing.x);
	outgoing.y = mix(0.+vary.y, 1.-vary.y, outgoing.y);
	incident.y = mix(0.+vary.y, 1.-vary.y, incident.y);

	vec2 positionPhiInc = outgoing + vec2(1./180.,0.);
	vec2 positionThetaInc = outgoing + vec2(0.,1./90.);


	vec3 intensity = texture3D(brdfData,vec3(outgoing, incident.y)).xyz;
	vec3 positionPhiIncIntensity = texture3D(brdfData,vec3(positionPhiInc, incident.y)).xyz;
	vec3 positionThetaIncIntensity = texture3D(brdfData,vec3(positionThetaInc, incident.y)).xyz;	
	
	// compute displacement of the point according to the channel
	float mainIntensity = length(displayChannel*intensity);
	mainIntensity = transformReflectionValue(mainIntensity);

	float phiIntensity = length(displayChannel*positionPhiIncIntensity);
	phiIntensity = transformReflectionValue(phiIntensity);
	
	float thetaIntensity = length(displayChannel*positionThetaIncIntensity);
	thetaIntensity = transformReflectionValue(thetaIntensity);
	
	// compute vertex position and auxiliary points in world space.
	vec3 v0 = vec3(in_vertex.xyz * max(0.,mainIntensity));
	
	// convert the texture access coordinates to spherical coordinates, and from spherical coordinates to cartesian coordinates. 
	// after that, multiply the resulting vertex by the intensity to get the final position in world space.
	vec3 v1 = spherical2cart(vec2(positionPhiInc.x*2.*PI,positionPhiInc.y*PI/2.))*phiIntensity;
	vec3 v2 = spherical2cart(vec2(positionThetaInc.x*2.*PI,positionThetaInc.y*PI/2.))*thetaIntensity;
	
	// normal computation
	vec3 v01 = normalize(v1-v0);
	vec3 v02 = normalize(v2-v0);
	
	vec3 normal = cross(v02,v01);
	
	// set the final position for the vertex
	gl_Position = gl_ModelViewProjectionMatrix*vec4(vec3(in_vertex.xyz * max(0.,mainIntensity)),1.);
	
	// pass the information to the fragment shader
	gl_TexCoord[0] = vec4(v0, 1.); // vertex position
	gl_TexCoord[1] = vec4(normal,1.); // normal
	
	// color
	float minA = 0.;
	float maxA = pow(incidentThetaIndex,cubeRoot)*100.;
	
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
