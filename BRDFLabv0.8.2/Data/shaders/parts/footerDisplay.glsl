void main()																					
{																							
	//Transform the vertex (ModelViewProj matrix)											
//	gl_Position = ftransform();			
	
	// Incident and exitant directions
	vec3 u = normalize(incidentVector); // incident position // light
	vec3 v = vertex.xyz; // exitant position // viewer

// transform the vertex position to spherical coordinates
	vec2 uv = cart2spherical(v);

	vec3 vPlusU = spherical2cart(uv + vec2(0.01,0.));
	vec3 vPlusV = spherical2cart(uv + vec2(0.,0.01));
	
	// compute the intensitys at each position
	
	float mainIntensity = computeBRDF(u, v).w;
	mainIntensity = ((1.-applyLog)*pow(mainIntensity,cubeRoot)+applyLog*log(mainIntensity+1.))*100.;
	
	float uIntensity = computeBRDF(u, vPlusU).w;
	uIntensity = ((1.-applyLog)*pow(uIntensity,cubeRoot)+applyLog*log(uIntensity+1.))*100.;
	
	float vIntensity = computeBRDF(u, vPlusV).w;
	vIntensity = ((1.-applyLog)*pow(vIntensity,cubeRoot)+applyLog*log(vIntensity+1.))*100.;
	
	vec3 mainPosition = v*mainIntensity;
	vPlusU = vPlusU *uIntensity;
	vPlusV = vPlusV *vIntensity;
	
//vectors
	vec3 vectorU = normalize(vPlusU-mainPosition);
	vec3 vectorV = normalize(vPlusV-mainPosition);
	
	vec3 vnormal = cross(vectorV,vectorU);
	vectorU = cross(vnormal,vectorV);
	vnormal = cross(vectorV,vectorU);

	// pass the position to the fragment shader
	gl_TexCoord[0] =  vertex;

	// pass the normal to the fragment shader
	gl_TexCoord[1] = vec4(normalize(vnormal),1.);//vec4(cross(vectorV,vectorU),1.);
	
	// Displace point 
	// TODO: pow 0.33 in scatmech
	vec4 dpos = vec4(vertex.xyz * max(mainIntensity,0.), 1);

	// Output point in clip space

	gl_Position = gl_ModelViewProjectionMatrix*dpos;

														
}
