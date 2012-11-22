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



namespace {

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

/**
 * Builds our help menu UI.
 */
/*Control* createHelp( osgViewer::View* view )
{
    static char* text[] = {
        "left mouse :",        "pan",
        "middle mouse :",      "rotate",
        "right mouse :",       "continuous zoom",
        "double-click :",      "zoom to point",
        "scroll wheel :",      "zoom in/out",
        "arrows :",            "pan",
        "1-6 :",               "fly to preset viewpoints",
        "shift-right-mouse :", "locked panning",
        "u :",                 "toggle azimuth lock",
        "c :",                 "toggle perspective/ortho",
        "t :",                 "toggle tethering"
    };

    Grid* g = new Grid();
    for( unsigned i=0; i<sizeof(text)/sizeof(text[0]); ++i ) {
        unsigned c = i % 2;
        unsigned r = i / 2;
        g->setControl( c, r, new LabelControl(text[i]) );
    }

    VBox* v = new VBox();
    v->addControl( g );

    return v;
}*/


/**
 * Some preset viewpoints to show off the setViewpoint function.
 */
static Viewpoint VPs[] = {
    Viewpoint( "Africa",        osg::Vec3d(    0.0,   0.0, 0.0 ), 0.0, -90.0, 10e6 ),
    Viewpoint( "California",    osg::Vec3d( -121.0,  34.0, 0.0 ), 0.0, -90.0, 6e6 ),
    Viewpoint( "Europe",        osg::Vec3d(    0.0,  45.0, 0.0 ), 0.0, -90.0, 4e6 ),
    Viewpoint( "Washington DC", osg::Vec3d(  -77.0,  38.0, 0.0 ), 0.0, -90.0, 1e6 ),
    Viewpoint( "Australia",     osg::Vec3d(  135.0, -20.0, 0.0 ), 0.0, -90.0, 2e6 ),
    Viewpoint( "Boston",        osg::Vec3d( -71.096936, 42.332771, 0 ), 0.0, -90, 1e5 )
    };


/**
 * Handler that demonstrates the "viewpoint" functionality in
 *  osgEarthUtil::EarthManipulator. Press a number key to fly to a viewpoint.
 */
struct FlyToViewpointHandler : public osgGA::GUIEventHandler {
    FlyToViewpointHandler( EarthManipulator* manip ) : _manip(manip) { }

    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa ) {
        if ( ea.getEventType() == ea.KEYDOWN && ea.getKey() >= '1' && ea.getKey() <= '6' ) {
            _manip->setViewpoint( VPs[ea.getKey()-'1'], 4.0 );
            aa.requestRedraw();
            }
        return false;
        }

    osg::observer_ptr<EarthManipulator> _manip;
    };


/**
 * Handler to toggle "azimuth locking", which locks the camera's relative Azimuth
 * while panning. For example, it can maintain "north-up" as you pan around. The
 * caveat is that when azimuth is locked you cannot cross the poles.
 */
struct LockAzimuthHandler : public osgGA::GUIEventHandler {
    LockAzimuthHandler(char key, EarthManipulator* manip)
        : _key(key), _manip(manip) { }

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
        if (ea.getEventType() == ea.KEYDOWN && ea.getKey() == _key) {
            bool lockAzimuth = _manip->getSettings()->getLockAzimuthWhilePanning();
            _manip->getSettings()->setLockAzimuthWhilePanning(!lockAzimuth);
            aa.requestRedraw();
            return true;
            }
        return false;
        }

    void getUsage(osg::ApplicationUsage& usage) const {
        using namespace std;
        usage.addKeyboardMouseBinding(string(1, _key), string("Toggle azimuth locking"));
        }

    char _key;
    osg::ref_ptr<EarthManipulator> _manip;
    };


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
    if(linked_version->major == 1 && linked_version->minor == 2)
    {
        if(linked_version->patch < 10)
        {
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
    if ( screen == NULL )
    {
        std::cerr<<"Unable to set "<<windowWidth<<"x"<<windowHeight<<" video: %s\n"<< SDL_GetError()<<std::endl;
        exit(1);
    }

    SDL_EnableUNICODE(1);

    // If we used 0 to set the fields, query the values so we can pass it to osgViewer
    windowWidth = screen->w;
    windowHeight = screen->h;

    osgViewer::Viewer viewer(arguments);
    //osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> gw = viewer.setUpViewerAsEmbeddedInWindow(0,0,windowWidth,windowHeight);


    // install the programmable manipulator.
    EarthManipulator* manip = new EarthManipulator();
    viewer.setCameraManipulator( manip );

    // UI:
    //Control* help = createHelp(&viewer);

    osg::Node* earthNode = MapNodeHelper().load( arguments, &viewer);//, help );
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

        if ( mapNode->getMap()->isGeocentric() ) {
            manip->setHomeViewpoint(
                Viewpoint( osg::Vec3d( -82.968, 35.839, 0 ), 0, -10.0, 150000 ) );
            }
        }

    viewer.setSceneData( root );

    manip->getSettings()->bindMouse(
        EarthManipulator::ACTION_EARTH_DRAG,
        osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON,
        osgGA::GUIEventAdapter::MODKEY_SHIFT );

    manip->getSettings()->setMinMaxPitch(-80.0,0.8);



    manip->getSettings()->setArcViewpointTransitions( true );

    viewer.addEventHandler(new FlyToViewpointHandler( manip ));
    viewer.addEventHandler(new LockAzimuthHandler('u', manip));

    viewer.realize();


    SDLIntegration sdlIntegration;
    SDL_Event event;

    bool zoom = false;
    double zoomX = 0.0;
    double zoomY = 0.0;

    double panX = 0.0;
    double panY = 0.0;




    osg::notify(osg::INFO)<<"USE_SDL"<<std::endl;

    bool done = false;
    while( !done ) {
        SDL_Event event;

        while ( SDL_PollEvent(&event) ) {
            switch (event.type) {

                case SDL_JOYBUTTONDOWN:

                    switch(event.jbutton.button) {
                        case PS3ButtonSelect:

                            exit(-1);
                            break;
                        case PS3ButtonDPadUp:

                            zoomX -= 0.025;
                            zoomY -= 0.025;
                            zoom = true;
                            break;
                        case PS3ButtonDPadDown:

                            zoomX += 0.025;
                            zoomY += 0.025;
                            zoom = true;
                            break;

                        }
                    break;


                case SDL_JOYBUTTONUP:

                    switch(event.jbutton.button) {
                        case PS3ButtonSelect:

                            exit(-1);
                            break;
                        case PS3ButtonDPadUp:
                            zoom = false;
                            zoomX = 0.0;
                            zoomY = 0.0;
                            break;
                        case PS3ButtonDPadDown:
                            zoom = false;
                            zoomX = 0.0;
                            zoomY = 0.0;
                            break;
                        }
                    break;

                case SDL_JOYAXISMOTION:
                    //axes movement
                    if(event.jaxis.axis==0) {
                        panX = -0.000005*event.jaxis.value;
                        }
                    if(event.jaxis.axis==1) {
                        panY = 0.000005*event.jaxis.value;
                        }


                    if(event.jaxis.axis==2) {
                        manip->rotate(-0.000003*event.jaxis.value, 0);
                        //manip->rotate(event.jaxis.value/32767.0f,0);
                        }
                    if(event.jaxis.axis==3) {
                        manip->rotate(0.0,osg::DegreesToRadians(0.00005*event.jaxis.value));
                        }


                    manip->pan(panX,panY);
                    break;

                case SDL_VIDEORESIZE:
                    SDL_SetVideoMode(event.resize.w, event.resize.h, bitDepth, SDL_OPENGL | SDL_RESIZABLE);
                    //gw->resized(0, 0, event.resize.w, event.resize.h );
                    break;


                case SDL_KEYUP:

                    if (event.key.keysym.sym=='f') {
                        SDL_WM_ToggleFullScreen(screen);
                        //gw->resized(0, 0, screen->w, screen->h );
                        }
                    break;

                case SDL_QUIT:
                    done = true;
                }
            if(zoom) {
                manip->zoom(zoomX,zoomY);
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
