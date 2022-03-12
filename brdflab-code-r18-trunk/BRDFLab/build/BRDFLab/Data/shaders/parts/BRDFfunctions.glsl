
// This file contains common BRDF functions used by analytical shaders
// (Fresnel, distribution functions, ...)

// BRDF Fresnel factor (Schlick's approximation)
float fresnel(float F0, float dotVH)
{
	return F0 + (1.-F0) * pow(max(0.,1.-dotVH), 5.);
}

