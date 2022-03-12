BRDFLab version 0.8.2

Contact:
Adrià Forés Herranz - afores 'at' ima.udg.edu
Sumanta Pattanaik
Carles Bosch
Xavier Pueyo

BRDFLab is a novel system for interactive modeling and designing of arbitrary BRDFs. The system is
able to deal with BRDFs defined in a variety of forms, such as analytical models, measured data or data obtained
by simulation. The system also allows designing BRDFs from scratch using a combination of different analytical
lobes. Using the programmable graphics hardware, it then performs interactive display of the designed BRDF,
and its rendering on objects lit by complex illumination. The system also allows the fitting of an input BRDF
defined in any form to our analytical lobe combination, so that it can be efficiently evaluated with GPU based
rendering. The idea behind this work is to make available a general system for designing, fitting and rendering
BRDFs, that is intuitive and interactive in nature. We plan to use this as a tool for simulation and modeling of
complex physically-based BRDFs, and thus provide access to a larger variety of material models to the rendering
community.

Requirements:
- Nvidia 8000 Series or higher graphics card.

How to begin:
You can create a new model from scratch, being a parametric, simulated or measured one. Also, you can open previously 
saved files, which can contain for example a measured data and its fittings performed. This can be seen in the database
files (XML), at Data/brdfs/, there you can see a set of materials predefined. Try to open one of this materials, if the 
system detects that you do not have the required material data, it will be downloaded automatically.

Key functions:
- Left, Right, Up, Down, M, N: Move light source along X, Y and Z axes (only point light).
- W, Q: Wireframe/solid mode.
- P: Take a snapshot.

Mouse functions:
- Use the right button by move the camera around the object/BRDF.
- Use the left button or the mouse wheel to zoom in and out.

Adding new analytical BRDF models:
- You can define new analytical models using an XML file. Existing models can be found under Data/analyticModels/. 
- The following items should be defined for each analytical BRDF model:
	- Model parameters
	- Model equation (parsed by muParser; look at existing examples to see how to code this part)
	- GLSL equation
	- GLSL probability distribution function (PDF)
	- GLSL importance sampling
- Some tips about the XML implementation:
	- GLSL function names and definition (parameters) should match the ones in the examples (e.g. modelNamePDF).
	- '<' character gives problems with the XML parser, as it interprets it as XML code. Use 'U+003C' instead, 
	  which is the ascii code for the character. Or simply use '>', swapping the statement.
	- All the BRDF parameters are in tangent space, and the normal is at (0,0,1).
	- You can use the following GLSL functions in your shader code, as they are already defined:
		- float fresnel(float F0, float dotVH)
		- vec2 cart2spherical(vec3 pos)
		- vec3 spherical2cart(vec2 uv)
	- Take a look at Data/shaders/parts/headerEnvMap.glsl to see their implementation.

Webpage:
http://brdflab.sourceforge.net

Libraries used:
- Ogre 3D as a rendering engine (http://www.ogre3d.org/)
- Qt for the GUI (http://qt.nokia.com/)
- Levmar for non-linear optimization (http://www.ics.forth.gr/~lourakis/levmar/)
- NLopt also for optimization (http://ab-initio.mit.edu/wiki/index.php/NLopt)
- Scatmech C++ optics library (http://physics.nist.gov/Divisions/Div844/facilities/scatmech/html/index.htm)
- muParser for parsing analytical expressions (http://muparser.sourceforge.net/)
- Qtpropertybrowser for editing properties in Qt (http://doc.trolltech.com/solutions/4/qtpropertybrowser/)

GPL code also used on the distribution:
- Some code of GPU Based Importance Sampling (http://graphics.cs.ucf.edu/gpusampling/) from Mark Colbert and Jaroslav Krivanek.
- MERL database reading file (http://www.merl.com/brdf/) from Wojciech Matusik, Hanspeter Pfister, Matt Brand, and Leonard McMillan.


Changelog:
---------

Version 0.8.2:
- Analytical models can be easily added using XML descriptions (using muParser and GLSL code)
- Added NLopt as optimizer along with more metrics
- Added more analytical models

Version 0.5:
- Fixed minor issue when opening Measured data.
- Added the ability to display measured data with points.

Version 0.4:
- Env map rendering now supports all Analitical BRDF models, and all possible combinations of them.
- When open a measurement database file (XML) the system automatically downloads the measured data, if required. 
- When you create an analytical BRDF model, you are able to see the importance samples used on Environment map rendering, on the 
brdf display.
- Many object models from the Stanford 3D Scanning Repository (http://graphics.stanford.edu/data/3Dscanrep/) added.
- Many env. maps from Paul Debevec (http://www.debevec.org/) added.
- Minor bugfixes solved.
- GPU Simulation temporaly disabled for now, major changes are coming.

Version 0.3:
- Support for multiple fittings.
- Improvement on Grooves rendering.

Version 0.2:
- XML write / load working for all BRDF models
- Groove simulation added
- View -> XML option also working 
- Tools -> Export BRDF Data option added to export the simulation results of our system.

Version 0.1:
- First public release
