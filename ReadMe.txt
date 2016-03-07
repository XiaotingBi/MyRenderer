This is a homework project in course CS580 at USC.

In this project, I build a small but useful graphics library; we'll call it the Gz library, and all functions, vars, etc, will be named accordingly. Certain standards will apply to make the code interfaces consistent. Consistency will be established by the use of prepared include files and an Application Program Interface (API).

This renderer realizes: 
	(1) scan converter using LEE Rasterization Method;
	(2) vertices transformation from world space to screen space;
	(3) three shading modes: flat shading, Gouraud shading and Phong shading;
	(4) texture mapping with perspective correction (image texture mapping and procedural texture mapping);
	(5) antialiasing with unweighted or weighted filters.

Most of major functions are implemented in "rend.cpp", "disp.cpp" and "Application5.cpp" files.
