		//same
		float value = u.z * brdf / pdf;
		
		//same
		if( value > 0.){				
		
			// before accessing the environment map, transform from tangent space to world space.
			vec3 incomingWorld = localToWorld*u;
			
			//float distortion = 4*pow(1.2,2.)*pow(abs(u.z)+1.,2.);
			float distortion = 1.;

			// compute LoD			
			//float LoD = 6.01329 - log2(pdf)*0.5;
			float LoD = max(preLoD-0.5*log2(pdf*distortion),0.);

			vec3 L = getSample(front,back,vec4(incomingWorld,LoD)).rgb;
		
			lobeColor.xyz += L*value;		
			
			lobeColor.a++;
		}
		
	}
	
	// average by the number of samples
	if(lobeColor.a > 0.)
		lobeColor.xyz /= lobeColor.a;
	else
		lobeColor = vec4(0.,0.,0.,0.);
	
	color += lobeColor.xyz*sfactor;
	
	//reset vars.
	lobeColor = vec4(0.,0.,0.,0.);
	sfactor = vec3(0.,0.,0.);