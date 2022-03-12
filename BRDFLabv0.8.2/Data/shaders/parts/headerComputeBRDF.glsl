
vec4 computeBRDF(vec3 u, vec3 v)
{
	vec3 color = vec3(0.,0.,0.);		// outgoing reflection (if channels == 1, only color.r is used)
	vec3 sfactor = vec3(0.,0.,0.); 	
	int j = 0;							// current position in params array
	float value = 0.;
	
	// call each BRDF Lobe, generated on the fly
	