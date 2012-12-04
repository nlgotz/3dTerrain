/* -*-c++-*- */
/* 3DTerrain - main display of program
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

#include <string>

#include <osg/Notify>
#include <osg/Timer>
#include <osg/ShapeDrawable>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgEarth/GeoMath>
#include <osgEarth/MapNode>
#include <osgEarth/TerrainEngineNode>
#include <osgEarth/Viewpoint>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osgEarthAnnotation/LocalGeometryNode>
#include <osgEarthSymbology/Style>

#include <osgEarthUtil/ObjectPlacer>
#include <osgEarthUtil/ElevationManager>


#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_events.h>
#include <SDL/SDL_joystick.h>
#include "SDLIntegration.h"

using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;
using namespace osgEarth::Annotation;

#define D2R (osg::PI/180.0)
#define R2D (180.0/osg::PI)

int main(int argc, char** argv) {
    osg::ArgumentParser arguments(&argc,argv);

    // init SDL
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    // Starting with SDL 1.2.10, passing in 0 will use the system's current resolution.
    unsigned int windowWidth = 0;
    unsigned int windowHeight = 0;

    // Passing in 0 for bitdepth also uses the system's current bitdepth. This works before 1.2.10 too.
    unsigned int bitDepth = 0;

    // If not linked to SDL 1.2.10+, then we must use hardcoded values
    const SDL_version* linked_version = SDL_Linked_Version();
    if(linked_version->major == 1 && linked_version->minor == 2) {
        if(linked_version->patch < 10) {
            windowWidth = 1280;
            windowHeight = 1024;
        }
    }

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    // set up the surface to render to
    SDL_Surface* screen = SDL_SetVideoMode(windowWidth, windowHeight, bitDepth, SDL_OPENGL | SDL_FULLSCREEN | SDL_RESIZABLE);
    if ( screen == NULL ) {
        std::cerr<<"Unable to set "<<windowWidth<<"x"<<windowHeight<<" video: %s\n"<< SDL_GetError()<<std::endl;
        exit(1);
    }

    SDL_EnableUNICODE(1);

    // If we used 0 to set the fields, query the values so we can pass it to osgViewer
    windowWidth = screen->w;
    windowHeight = screen->h;

    osgViewer::Viewer viewer(arguments);
    //SDL window embedder that causes my laptop to crash
    //osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> gw = viewer.setUpViewerAsEmbeddedInWindow(0,0,windowWidth,windowHeight);

    // install the programmable manipulator.
    EarthManipulator* manip = new EarthManipulator();
    viewer.setCameraManipulator( manip );


    osg::Node* earthNode = MapNodeHelper().load( arguments, &viewer);
    if (!earthNode) {
        OE_WARN << "Unable to load earth model." << std::endl;
        return -1;
    }

    osg::Group* root = new osg::Group();
    root->addChild( earthNode );

    osgEarth::MapNode* mapNode = osgEarth::MapNode::findMapNode( earthNode );
    if ( mapNode ) {
        if ( mapNode )
            manip->setNode( mapNode->getTerrainEngine() );
        //sets the home viewpoint
        if ( mapNode->getMap()->isGeocentric() ) {
            manip->setHomeViewpoint(
                Viewpoint( osg::Vec3d( -82.968, 35.839, 0 ), 0, -10.0, 150000 ) );

        }
    }

    //tower is used for placing radio towers in SDLIntegration
    osg::Group* tower = new osg::Group();
    root->addChild(tower);

    //set root as the main scene
    viewer.setSceneData( root );

    //set the mouse actions
    manip->getSettings()->bindMouse(
        EarthManipulator::ACTION_EARTH_DRAG,
        osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON,
        osgGA::GUIEventAdapter::MODKEY_SHIFT );



    manip->getSettings()->setArcViewpointTransitions( true );

    viewer.realize();


    //initialize the controller
    SDLIntegration controller(manip, tower, mapNode);
    SDL_Event event; //handles SDL events

    osg::notify(osg::INFO)<<"USE_SDL"<<std::endl;

    bool done = false;
    /*
     * this while loop handles all of the SDL input into OSG
     */
    while( !done ) {
        while ( SDL_PollEvent(&event) ) {

            //This handles all input from the controller
            controller.input(event);
            //std::cout << manip->getViewpoint().getFocalPoint().x() << std::endl;
            //handle all the other SDL event stuff
            switch (event.type) {

            case SDL_VIDEORESIZE:
                SDL_SetVideoMode(event.resize.w, event.resize.h, bitDepth, SDL_OPENGL | SDL_RESIZABLE);
                //gw->resized(0, 0, event.resize.w, event.resize.h );
                break;


            case SDL_KEYUP:
                /*
                 * Escape doesn't work properly because OSG won't embed into SDL
                 * on my laptop properly. Maybe after the final, I'll try fixing
                 * this, but I've already had to reinstall everything once and
                 * don't want to do that again, less than 2 weeks from finals
                 */
                if(event.key.keysym.sym==SDLK_ESCAPE) {
                    exit(-1);
                }
                if (event.key.keysym.sym=='f') {
                    SDL_WM_ToggleFullScreen(screen);
                    //gw->resized(0, 0, screen->w, screen->h );
                }
                break;

            case SDL_QUIT:
                done = true;
            }
        }

        if (done) continue;


        // draw the new frame
        viewer.frame();

        // Swap Buffers
        SDL_GL_SwapBuffers();
    }
    return 0;
}

