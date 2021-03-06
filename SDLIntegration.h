/* -*-c++-*- */
/* SDLIntegration - connects the controller
* Copyright 2012 Nathan Gotz
* https://github.com/nlgotz/3dTerrain
*
*    This file is part of 3DTerrain.
*
*    3DTerrain is free software: you can redistribute it and/or modify
*    it under the terms of the Lesser GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    3DTerrain is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    Lesser GNU General Public License for more details.
*
*    You should have received a copy of the Lesser GNU General Public License
*    along with 3DTerrain.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SDLINTEGRATION
#define SDLINTEGRATION

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_joystick.h>

#include <osgViewer/Viewer>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/ObjectPlacer>
#include <osg/ShapeDrawable>
#include <osgEarthUtil/ElevationManager>

#include <vector>
#include <map>

class SDLIntegration : public osgGA::GUIEventHandler {

private:
    void viewID(bool positiveNum);


public:
    SDLIntegration(osgEarth::Util::EarthManipulator* manip, osg::Group* tower, osgEarth::MapNode* mapNode);
    ~SDLIntegration();
    void input(SDL_Event event);




protected:
    bool            _objectMode;
    void buttonDownNavigate(Uint8 bID);
    void buttonUpNavigate(Uint8 bID);
    void axesMovementNavigate(Uint8 aID, Sint16 value);

    SDL_Joystick*   _joystick;
    SDL_Event      _event;
    osgEarth::Util::EarthManipulator* _manip;
    osg::Group* _tower;
    osgEarth::MapNode* _mapNode;
    int             _numAxes;
    int             _numButtons;
    bool            _verbose;
    //this stuff should be put into structs, but that's a low priority
    double          _panX;
    double          _panY;
    double          _rotateX;
    double          _rotateY;
    bool            _zoom;
    double          _zoomX;
    double          _zoomY;
    int _POInum;
    osgEarth::Util::ElevationManager* _elevation;
    osg::Geode* _geode;



    //button mapping
    enum PS3Button {
        PS3ButtonSelect		= 0,
        PS3ButtonL3		= 1,
        PS3ButtonR3		= 2,
        PS3ButtonStart		= 3,
        PS3ButtonDPadUp		= 4,
        PS3ButtonDPadRight	= 5,
        PS3ButtonDPadDown	= 6,
        PS3ButtonDPadLeft	= 7,
        PS3ButtonL2		= 8,
        PS3ButtonR2		= 9,
        PS3ButtonL1		= 10,
        PS3ButtonR1		= 11,
        PS3ButtonTriangle	= 12,
        PS3ButtonCircle		= 13,
        PS3ButtonX		= 14,
        PS3ButtonSquare		= 15,
        PS3ButtonPS3Button	= 16,
    };

    /*
     * axes mapping
     * there are more axes available, but I'm not positive on what each one
     * does to name them yet.
     */
    enum PS3Axes {
        PS3AxesLeftX	= 0,
        PS3AxesLeftY	= 1,
        PS3AxesRightX	= 2,
        PS3AxesRightY	= 3,
        PS3AxesRoll     = 4,
        PS3AxesPitch    = 5,
        PS3AxesAccel    = 6,
    };
};

#endif
