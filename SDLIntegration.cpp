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

#include <osgViewer/Viewer>

#include "SDLIntegration.h"

#include <SDL.h>
#include <SDL_events.h>
#include <SDL_joystick.h>

#include <iostream>

static osgEarth::Viewpoint VPs[] = {
    osgEarth::Viewpoint( "PPPP", osg::Vec3d( -87.90483,   42.53757, 0.0 ), 0.0, -90.0, 2500 ),
    osgEarth::Viewpoint( "OCPP", osg::Vec3d( -87.82904,  42.84204, 0.0 ), 0.0, -90.0, 2500 ),
    osgEarth::Viewpoint( "VAPP", osg::Vec3d(    -87.92394, 43.03054, 0.0 ), 0.0, -90.0, 2500 ),
    osgEarth::Viewpoint( "GTPP", osg::Vec3d(  -88.15118,  43.19663, 0.0 ), 0.0, -90.0, 2500 ),
    osgEarth::Viewpoint( "PIPP", osg::Vec3d(  -87.3967, 46.57869, 0.0 ), 0.0, -90.0, 2500 ),
    osgEarth::Viewpoint( "MOB",  osg::Vec3d(  -87.90320, 43.03925, 0 ), 0.0, -90, 500 )
};

SDLIntegration::SDLIntegration(osgEarth::Util::EarthManipulator* manip, osg::Group* tower, osgEarth::MapNode* mapNode) {
    _verbose = true; //print out help messages

    //initialize SDL
    if ( SDL_Init(SDL_INIT_JOYSTICK) < 0 ) {
        fprintf(stderr, "Unable to initialize SDL %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    int numJoysticks =  SDL_NumJoysticks();
    if (_verbose) {
        std::cout<<"number of joysticks "<<numJoysticks<<std::endl;
        for(int i=0; i<numJoysticks; ++i) {
            std::cout<<"Joystick name '"<<SDL_JoystickName(i)<<"'"<<std::endl;
        }
    }

    _joystick = numJoysticks>0 ? SDL_JoystickOpen(0) : 0;


    _numAxes = _joystick ? SDL_JoystickNumAxes(_joystick) : 0;
    _numButtons = _joystick ? SDL_JoystickNumButtons(_joystick) : 0;

    if(_verbose) {
        std::cout << "numAxes: " << _numAxes << std::endl;
        std::cout << "numButtons: " << _numButtons << std::endl;
    }

    //set the default values
    _panX = 0;
    _panY = 0;
    _rotateX = 0;
    _rotateY = 0;
    _POInum = -1;

    //EarthManipulator stuff
    _manip = manip;
    _tower = tower;
    _mapNode = mapNode;
}
SDLIntegration::~SDLIntegration() {
//Do Nothing
}

/**
 * handles the input from the controller
 **/
void SDLIntegration::input(SDL_Event event) {
    switch (event.type) {
    case SDL_JOYBUTTONDOWN:
        SDLIntegration::buttonDownNavigate(event.jbutton.button);
        break;
    case SDL_JOYBUTTONUP:
        SDLIntegration::buttonUpNavigate(event.jbutton.button);
        break;
    case SDL_JOYAXISMOTION:
        SDLIntegration::axesMovementNavigate(event.jaxis.axis, event.jaxis.value);
        break;
    }
    if(_zoom) {
        _manip->zoom(_zoomX,_zoomY);
    }
    _manip->pan(_panX,_panY);
    _manip->rotate(_rotateX, _rotateY);
//        if(_objectMode) {
//                //handle object movement
//        }
}


void SDLIntegration::buttonDownNavigate(Uint8 bID) {
    switch (bID) {
    case SDLIntegration::PS3ButtonSelect:
        //exit the program. this button will actually close the program unlike
        //the ESC key
        exit(-1);
        break;
    case SDLIntegration::PS3ButtonPS3Button:
        //kind of a hack, but I'm unsure of how to use ACTION_HOME, but that should be used instead
        _manip->setViewpoint(osgEarth::Viewpoint( osg::Vec3d( -82.968, 35.839, 0 ), 0, -10.0, 150000 ));
        break;
    case SDLIntegration::PS3ButtonDPadUp:
        //zoom in
        _zoomX -= 0.025;
        _zoomY -= 0.025;
        _zoom = true;
        break;
    case SDLIntegration::PS3ButtonDPadDown:
        //zoom out
        _zoomX += 0.025;
        _zoomY += 0.025;
        _zoom = true;
        break;

    case SDLIntegration::PS3ButtonR1:
        //go to the next POI
        SDLIntegration::viewID(true);
        _manip->setViewpoint( VPs[SDLIntegration::_POInum], 4.0 );
        break;
    case SDLIntegration::PS3ButtonL1:
        //go to the previous POI
        SDLIntegration::viewID(false);
        _manip->setViewpoint( VPs[SDLIntegration::_POInum], 4.0 );
        break;


    case SDLIntegration::PS3ButtonTriangle:
        //remove the last tower that was added
        _tower->removeChild(_tower->getNumChildren()-1, 1);
        break;

    case SDLIntegration::PS3ButtonX:
        /*
         * This button changes the mode to object placement.
         * The size of the tower is statically set. When the user clicks
         * on the X button, it places a tower in the middle of the
         * screen at a height close to the correct height.
         */
        _geode = new osg::Geode();
        _geode->addDrawable( new osg::ShapeDrawable(new osg::Cylinder(osg::Vec3d(0,0,0), 2, 85.3)));
        osgEarth::Util::ObjectPlacer placer( _mapNode );
        double hasml;
        double hae;
        _elevation = new osgEarth::Util::ElevationManager(_mapNode->getMap());
        _elevation->getElevation(_manip->getViewpoint().getFocalPoint().x(),_manip->getViewpoint().getFocalPoint().y(), 0.0, _mapNode->getMap()->getSRS(), hasml, hae);
        _tower->addChild( placer.placeNode( _geode, _manip->getViewpoint().getFocalPoint().y(),  _manip->getViewpoint().getFocalPoint().x(), hasml ));
        break;
    }

}

void SDLIntegration::buttonUpNavigate(Uint8 bID) {
    switch(bID) {
    case SDLIntegration::PS3ButtonDPadUp:
        //stop zooming in and reset the zoom level
        _zoom = false;
        _zoomX = 0.0;
        _zoomY = 0.0;
        break;
    case SDLIntegration::PS3ButtonDPadDown:
        //stop zooming out and reset the zoom level
        _zoom = false;
        _zoomX = 0.0;
        _zoomY = 0.0;
        break;

    }
}

void SDLIntegration::axesMovementNavigate(Uint8 aID, Sint16 value) {
    switch (aID) {
    case SDLIntegration::PS3AxesLeftX:
        //move left or right when moving the left stick left or right
        _panX = -0.000005*value;
        break;
    case SDLIntegration::PS3AxesLeftY:
        //move forwards or backwards when moving the left stick up or down
        _panY = 0.000005*value;
        break;
    case SDLIntegration::PS3AxesRightX:
        //change the rotation of the camera based on the right stick
        _rotateX = -0.000003*value;
        break;
    case SDLIntegration::PS3AxesRightY:
        //change the pitch of the camera from the up/down of the right stick
        _rotateY = osg::DegreesToRadians(0.00005*value);
    }
}

/**
 * this handles the POI jumping by either increasing or decreasing the POInum and
 * wrapping it when needed.
 **/
void SDLIntegration::viewID(bool positiveNum) {
    if(positiveNum) {
        SDLIntegration::_POInum = (SDLIntegration::_POInum < 5) ? SDLIntegration::_POInum+1 : 0;
    } else {
        SDLIntegration::_POInum = (SDLIntegration::_POInum > 0) ? SDLIntegration::_POInum-1 : 5;
    }
}
