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
To build 3DTerrain (please note, this won't work right now, you need to modify two of the cmake files in the osgearth project for this to work (this will be fixed soon)):

make application_3DTerrain

To run from the osgearth folder:

./bin/3DTerrain <path to .earth file>

Next Steps
----------
- Fix cmake to work outside osgEarth directoy
- Add Tower Placement
- Improve caching and precaching
- Clean up SDL code

Right now, the project only builds if it is in the applications folder of osgEarth, the most immediated goal is to redo the cmake and make files to work outside of the osgEarth folder.

As part of my final project, the user will be able to place radio towers in the terrain and (may) be able to scale the tower. The tower will most likely be implemented via a glutwireframe object.

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