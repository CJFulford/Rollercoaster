# Rollercoaster

Cody Fulford

Rollercoaster animation project for CPSC 587 - Computer Animation W17 at the University of Calgary.

Libraries Used: Glad V4.5, GLFW x32, GLM

controls: 
  click and drag the mouse to rotate the camera around the coaster.
  Use the WASD keys to move the camera around the scene
  press 'C' to toggle first person mode. camera rotation and movement is disabled in first person mode
  
if running this program on linux, type make in the command line to run the make file then type ./Rollercoaster to run the program.
I have been compiling this code on visual studio so i dont have a windows make file

Track curve has been created using the control points and a b-spline curve. The curve creation follows the brute-force psudo code in Dr. Samavati's CPSC 589/689 Course Notes.

The tilt of the track factors in how fast the cart is going at that point and gravity.

The velocity of the carts are based on kinetic and potential energy with a minimum energy that results in a min speed of 1m/s

The cars are not considered seperatly and only the physics of the firsst car are considered

The Final.zip folder contains all the final code and libraries needed to run the project. this does not contain the readme
