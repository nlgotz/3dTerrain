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


SDLIntegration::SDLIntegration() {
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
    }
SDLIntegration::~SDLIntegration() {
//Do Nothing
    }
