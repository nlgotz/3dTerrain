3DTerrain
=========

What is it?
-----------
3DTerrain is a C++ program that allows a user to navigate around a 3D globe and see the elevation by changing the angle and zoom of the view. The camera can be controlled with either the mouse or a Playstation3 controller. The PS3 controller is the preferred method to control the program as it has a more natural feel. To get the controller uses SDL to get the event data.

About
-----
The aim of the project is to be able to place radio towers and use the height of the tower and the terrain to see if a site is feasible. This won't work as a way to completely plan sites as this program doesn't take into account trees and buildings, but this is a good start to see if a site would even potentially work.

Links
-----
<https://github.com/nlgotz/3dTerrain>

Why?
----
This is my final project for UWP Fall 2012 CS420-001 Computer Graphics.

Building and running
--------------------
To build 3DTerrain (This has only been tested on Ubuntu 12.04.1 x86_64):
cd <3DTerrain folder>
cmake .
make
sudo make install

The last step is optional and will install 3DTerrain to /usr/local/bin and allow you to just use 3DTerrain <path to .earth file> 

to execute:
./bin/3DTerrain <path to .earth file>

To improve performance, seeding the cache can be helpful. To do this run:
osgeath_cache --seed <path to .earth file>

Next Steps
----------
- Improve precaching
- Make POI programmable from XML file
- Improve object placement to allow for multiple objects and have it independent of the camera.

Controls
--------
3DTerrain is best controlled by a PS3 controller, but some features can be done with the mouse.

PS3 Controls:
Left stick - pan (move forward, backward, left, right)
Right stick - rotate (x-axis) and tilt (y-axis)
DPad Up/Down - Zoom in/out
L1/R1 - navigate POI (points of interest) by default these are WI/MI power plants
x - place a tower at the center of the screen
triangle - remove the last tower
Select - exit the program
PS3 button - reset view to the original view.

Mouse:
Left - pan
Right - zoom
Middle - rotate/tilt
scroll up/down - zoom in/out

Keyboard:
ESC - should exit the program, but right now it just freezes the screen
f - toggle fullscreen

Examples
--------
The examples folder contains 1 example .earth file. In order to actually work, the tiff.earth file needs to be modified so that the urls point to actual files

Elevation Sources
-------------------
The best source for NED files is from the USGS National Map (http://nationalmap.gov/viewer.html)

Some states also provide their own lower resolution (and older style DEM files).
Wisconsin's DNR provides both 30M and 10M resolution DEM and NED files that can be found at http://dnr.wi.gov/maps/gis/geolibrary.html


Licensing
---------
3DTerrain is free software: you can redistribute it and/or modify
it under the terms of the Lesser GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

3DTerrain is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
Lesser GNU General Public License for more details.

You should have received a copy of the Lesser GNU General Public License
along with 3DTerrain.  If not, see <http://www.gnu.org/licenses/>.
