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


vec2 cart2spherical2( vec3 pos )
{

	// code from colbert et al.

	vec3 npos = normalize(pos);
	
	float phi = atan(npos.y,npos.x);
	phi = (phi>0.)?phi:((2.*3.1415926535) + phi);
	
	float theta = acos(npos.z);
	
	return vec2(phi,theta);
}

float LambertBRDF(vec3 u, vec3 v, float albedo)		// TODO: remove?
{
	return albedo;
}


float BlinnPhongBRDF(vec3 u, vec3 v, float n)
{
	vec3 h = normalize(u+v);
	float dotNH = h.z;
	float nfactor = (n+2.) * 0.159155;	// 1/(2.*PI)
	
	// TODO: need to check dotp > 0 or lit already does max(dotp,0)?
	if(dotNH > 0.)
		return pow(dotNH,n) * nfactor;
	else
		return 0.;

// cg version
// 	return lit(dotNH,dotNH,n).z * nfactor; 		// TODO: better than pow(dotp,n) ?
}

float BlinnPhongPDF(vec3 u, vec3 v, float n)
{
	vec3 h = normalize(u+v);

	return (n+1.)*pow(abs(u.z),n)/(6.28319*4.*dot(v,h));
}

float LafortuneBRDF(vec3 u, vec3 v, float Cx, float Cy, float Cz, float n)
{
	float dotp = dot(u*vec3(Cx,Cy,Cz), v);
	float nfactor = (n+2.) /(6.28319*pow(max(abs(Cx),abs(Cz)),n));	 		// TODO: Cy?
	
	return pow(max(0.,dotp),n) * nfactor;
}

float LafortunePDF(vec3 u, vec3 v, float n)
{
	return (n+1.)*pow(u.z,n)/6.28319;//*sin(spherical.y);
}


#define EPSILON 1.0e-5

float fresnel(float F0, float dotVH)
{
	return F0 + (1.-F0) * pow(max(0.,1.-dotVH), 5.);
}

// u -> L, v -> V
float AshikminBRDF(vec3 u, vec3 v, float F0, float e)
{
	vec3 h = normalize(u+v);
	vec3 n = vec3(0.,0.,1.);
	/*
	//cook-torrance :D
	float m = e;
	vec3 delta = acos(dot(n,h));
	float G = min(1.,min((2*dot(n,h)*dot(n,v))/dot(v,h), (2*dot(n,h)*dot(n,u))/dot(v,h)));
	float D = 1/(m*m*pow(cos(delta),4.))*exp(-pow(tan(delta)/m,2.));
	
	return 1/3.14159265 * (D*G)/(dot(n,u)*dot(n,v)) *fresnel(F0, dot(v,h));
	*/
	//ashikhmin
	return (e+1.)/(8.*3.14159265)*((pow(dot(n,h),e))/(dot(v,h)*max(dot(n,u),dot(n,v))))*fresnel(F0,dot(v,h));
	
}

float AshikminPDF(vec3 u, vec3 v, float F0, float e)
{
	vec3 h = normalize(u+v);
	float D = (e+2.)*pow(max(0.,abs(h.z)), e)/6.28319; 
	
	return D / (2.*3.14159*4.*dot(v,h));	
}


// u -> L, v -> V
float CookTorranceBRDF(vec3 u, vec3 v, float F0, float m)
{
	vec3 h = normalize(u+v);
	vec3 n = vec3(0.,0.,1.);
	
	float delta = acos(dot(n,h));
	float G = min(1.,min((2.*dot(n,h)*dot(n,v))/dot(v,h), (2.*dot(n,h)*dot(n,u))/dot(v,h)));
	float D = 1./(m*m*pow(cos(delta),4.))*exp(-pow(tan(delta)/m,2.));
	
	return 1./3.14159265 * (D*G)/(dot(n,u)*dot(n,v)) *fresnel(F0, dot(v,h));
}

float CookTorrancePDF(vec3 u, vec3 v, float F0, float m)
{

	// u vector is the sample direction, not the incident direction
//	vec2 sph = cart2spherical2(u);

	float n = 0.;
	
	if(m != 0.)
		n = 2.*3.14159/(4.*m*m)-1.;

	return (n+1.)/(2.*3.14159)*pow(u.z,n);
	
//	return 1./(m*m*3.14159*pow(cos(sph.y),3.))*exp(-pow(tan(sph.y),2.)/(m*m));
}


float WardBRDF(vec3 u, vec3 v, float ax)
{
//	vec3 h = normalize(u+v);
//	vec2 half = cart2spherical2(h);
	
//	return (1./(4.*3.14159265*ax*ay*sqrt(u.z*v.z)))*exp(-pow(tan(half.y),2.)*(pow(cos(half.x),2.)/(ax*ax)+pow(sin(half.x),2.)/(ay*ay)));
	
	vec3 n = vec3(0.,0.,1.);
	
	vec3 h = normalize(u+v);
	
	float delta = acos(dot(n,h));
	
	//ward
	return 1./sqrt(dot(n,u)*dot(n,v)) * exp((-pow(tan(delta),2.)/(ax*ax))/(4.*3.1415926535*ax*ax));

}

float WardPDF(vec3 u, vec3 v, float ax)
{
	vec3 h = normalize(u+v);
	vec2 half = cart2spherical2(h);

	return 1./(4.*3.14159*ax*ax*dot(h,v)*pow(h.z,3.))*exp(-(pow(tan(half.y),2.))*(pow(cos(half.x),2.)/(ax*ax)+pow(sin(half.x),2.)/ax*ax));
}


float WardDuerBRDF(vec3 u, vec3 v, float ax)
{
//	vec3 h = normalize(u+v);
//	vec2 half = cart2spherical2(h);
	
//	return (1./(4.*3.14159265*ax*ay*sqrt(u.z*v.z)))*exp(-pow(tan(half.y),2.)*(pow(cos(half.x),2.)/(ax*ax)+pow(sin(half.x),2.)/(ay*ay)));
	
	vec3 n = vec3(0.,0.,1.);
	
	vec3 h = normalize(u+v);
	
	float delta = acos(dot(n,h));
		
	//ward-duer
	return 1./(dot(n,u)*dot(n,v)) * exp((-pow(tan(delta),2.)/(ax*ax))/(4.*3.1415926535*ax*ax));


}



float Li_sparseBRDF(vec3 u, vec3 v)
{

	vec2 incoming = cart2spherical2(u);
	/*
	vec3 i = v;

	// if phi_in is greather than 0, rotate the model.
	if(incoming.x > 0.0001){
			i.x = cos(-incoming.x)*v.x - sin(-incoming.x)*v.y;
			i.y = sin(-incoming.x)*v.x + cos(-incoming.x)*v.y;
	}

*/
	
	vec2 outgoing = cart2spherical2(v);
	
	vec3 direction = vec3(incoming.y,outgoing.y, outgoing.x-incoming.x);
	
	float m = 1./cos(direction.x) + 1./cos(direction.y);
	float delta = sqrt(pow(tan(direction.x),2.)+pow(tan(direction.y),2.)-2.*tan(direction.x)*tan(direction.y)*cos(direction.z));
	float t = acos((2./m)*sqrt(pow(delta,2.)+pow(tan(direction.x)*tan(direction.y)*sin(direction.z),2.)));
	float xi = acos(cos(direction.x)*cos(direction.y)+sin(direction.x)*sin(direction.y)*cos(direction.z));
	
	
	return (m/3.14159265)*(t-sin(t)*cos(t)-1.)+(1.+cos(xi))/(2.*cos(direction.x)*cos(direction.y));
	

}

float Ross_thickBRDF(vec3 u, vec3 v){

	vec2 incoming = cart2spherical2(u);
	
	/*
	vec3 i = v;

	// if phi_in is greather than 0, rotate the model.
	if(incoming.x > 0.0001){
			i.x = cos(-incoming.x)*v.x - sin(-incoming.x)*v.y;
			i.y = sin(-incoming.x)*v.x + cos(-incoming.x)*v.y;
	}
*/

	
	vec2 outgoing = cart2spherical2(v);
	
	vec3 direction = vec3(incoming.y,outgoing.y, outgoing.x-incoming.x);
	
	float m = 1./cos(direction.x) + 1./cos(direction.y);
	float delta = sqrt(pow(tan(direction.x),2.)+pow(tan(direction.y),2.)-2.*tan(direction.x)*tan(direction.y)*cos(direction.z));
	float t = acos((2./m)*sqrt(pow(delta,2.)+pow(tan(direction.x)*tan(direction.y)*sin(direction.z),2.)));
	float xi = acos(cos(direction.x)*cos(direction.y)+sin(direction.x)*sin(direction.y)*cos(direction.z));

	return (4./(3.*3.14159265*(cos(direction.x)*cos(direction.y))))*(((3.14159265/2.)-xi)*cos(xi)+sin(xi))*(1.+pow(1.+(xi/0.0261799387799149),-1.))-(1./3.);


}


#define LAMBERT 		0.		// lobe type
#define BLINNPHONG 		1.
#define LAFORTUNEISO	2.
#define LAFORTUNEANISO	3.
#define ASHIKMIN		4.
#define WARD			5.
#define LISPARSE		6.
#define ROSSTHICK		7.
#define COOKTORRANCE	8.
#define WARDDUER		9.

// returns BRDF value, and parameters passed.
// where j is the initial index.
float computeLobe(float lobe_type, vec3 u, vec3 v, inout int j, float params[100]){

	float value = 0.;

	if (lobe_type == LAMBERT)
	{
		value = LambertBRDF(u, v, params[j]);		// parameter should be 1 for linear implementation
		++j;
	}
	else if (lobe_type == BLINNPHONG)
	{
		value = BlinnPhongBRDF(u, v, params[j]);		// n
		++j;
	}
	else if (lobe_type == LAFORTUNEANISO)
	{
		value = LafortuneBRDF(u, v, params[j], params[j+1], params[j+2], params[j+3]);		// Cx, Cy, Cz, n
		j += 4;
	}
	else if (lobe_type == LAFORTUNEISO)
	{
		value = LafortuneBRDF(u, v, params[j], params[j], params[j+1], params[j+2]);		// Cx, Cy, Cz, n
		j += 3;
	}
	else if (lobe_type == ASHIKMIN)
	{
		value = AshikminBRDF(u, v, params[j], params[j+1]);		// F(0), n
		j += 2;
	}
	else if(lobe_type == WARD)
	{
		value = WardBRDF(u, v, params[j]);		
		++j;
	}
	else if(lobe_type == WARDDUER)
	{
		value = WardDuerBRDF(u, v, params[j]);		
		++j;
	}
	else if(lobe_type == LISPARSE)
	{
		value = Li_sparseBRDF(u, v);		
	}
	else if(lobe_type == ROSSTHICK) //TODO: add the parameter!!!
	{
		value = Ross_thickBRDF(u, v);		// xi0	
	}
	else if(lobe_type == COOKTORRANCE)
	{
		value = CookTorranceBRDF(u,v,params[j], params[j+1]);
		j += 2;
	}

	return value;
}

vec4 computeBRDF(vec3 u, vec3 v, float fixed, float channels, float lobes, vec3 scale, float params[100])
{
	vec3 color = vec3(0.,0.,0.);		// outgoing reflection (if channels == 1, only color.r is used)
	int j = 0;			// current position in params array

	// for each lobe ...
 	for (float i = 0.; i < lobes; ++i)		// TODO: add j < MAX_PARAMS ?
	{
		// Lobe type
		float lobe_type = params[j++];
		
		// Scale factor (albedo)
		vec3 sfactor = vec3(0.,0.,0.); 			// TODO: allow more than 3 channels
		
		if(fixed == 1.){
		
			// fixed combination of lobes, each lobe only represents a channel.
			// we use the scalingfactors vec3, to represent the lobe color.
			
			if(channels == 1.){
				sfactor = vec3(1.,1.,1.);
			}else{
				if(mod(i,channels) == 0.)
					sfactor.r = 1.;
				else if(mod(i,channels) == 1.)
					sfactor.g = 1.;
				else
					sfactor.b = 1.;
			}	
		}else{
			// linear combination of lobes, each lobe has nChannels scaling factors
			if(channels == 1.){
				sfactor.x = params[j];
				sfactor.y = params[j];
				sfactor.z = params[j];
				j++;
			}else{
				sfactor.r = params[j++];
				sfactor.g = params[j++];
				sfactor.b = params[j++];		
			}
		}
		
		// Evaluate lobe		
		float value = computeLobe(lobe_type, u, v, j, params);
		
		color += sfactor * value * scale;
	}

	return vec4(color.xyz, max((color.r + color.g + color.b),0.));//(intensity;
}
