#include <Ogre.h>


/**
	
	direction:

	direction[0] = theta_in
	direction[1] = theta_out
	direction[2] = phi_out

	*/
	double m(Ogre::Vector3 direction){

		return 1/Ogre::Math::Cos(direction[0]) + 1/Ogre::Math::Cos(direction[1]);

	}

	/**
	
	direction:

	direction[0] = theta_in
	direction[1] = theta_out
	direction[2] = phi_out

	*/
	double deltaModelInversion(Ogre::Vector3 direction){
			
		return Ogre::Math::Sqrt(Ogre::Math::Pow(Ogre::Math::Tan(direction[0]),2.)
								+ Ogre::Math::Pow(Ogre::Math::Tan(direction[1]),2.)
								- 2*Ogre::Math::Tan(direction[0])*Ogre::Math::Tan(direction[1])*Ogre::Math::Cos(direction[2]));
	
	}	


	/**
	
	direction:

	direction[0] = theta_in
	direction[1] = theta_out
	direction[2] = phi_out

	*/	
	double t(Ogre::Vector3 direction){
		return Ogre::Math::ACos((2/m(direction))*Ogre::Math::Sqrt(Ogre::Math::Pow(deltaModelInversion(direction),2.)+Ogre::Math::Pow(Ogre::Math::Tan(direction[0])*Ogre::Math::Tan(direction[1])*Ogre::Math::Sin(direction[2]),2.))).valueRadians();
	}




	/**
	
	direction:

	direction[0] = theta_in
	direction[1] = theta_out
	direction[2] = phi_out

	*/
	double xi(Ogre::Vector3 direction){

		return Ogre::Math::ACos(Ogre::Math::Cos(direction[0])*Ogre::Math::Cos(direction[1])+Ogre::Math::Sin(direction[0])*Ogre::Math::Sin(direction[1])*Ogre::Math::Cos(direction[2])).valueRadians();
	}

	double xi0(){
		return Ogre::Degree(1.5).valueRadians();
	}
