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
files (XML),at Data/brdfs/, there you can see a set of materials predefined. Try to open one of this materials, if the system 
detects that you do not have the required material data, it will be downloaded automatically.

Key functions:
- After clicking the render windows, the W key sets the wireframe mode, and Q key sets the solid mode.
- Use the arrows to move the point light, when exists.

Mouse functions:
- use the right button by move the camera around the object.
- use the left button or the mouse wheel to zoom in and out.

Webpage:
http://brdflab.sourceforge.net

Libraries used:
- Ogre 3D as a rendering engine (http://www.ogre3d.org/)
- Qt for the GUI (http://qt.nokia.com/)
- Levmar as a Non-linear optimization functions (http://www.ics.forth.gr/~lourakis/levmar/)
- Scatmech C++ optics library (http://physics.nist.gov/Divisions/Div844/facilities/scatmech/html/index.htm)

GPL code also used on the distribution:
- Some code of GPU Based Importance Sampling (http://graphics.cs.ucf.edu/gpusampling/) from Mark Colbert and Jaroslav Krivanek.
- MERL database reading file (http://www.merl.com/brdf/) from Wojciech Matusik, Hanspeter Pfister, Matt Brand, and Leonard McMillan
- QPropertyEditor (http://www.qt-apps.org/content/show.php/QPropertyEditor?content=68684) to enchnance the GUI interface from Volker Wiendl.
