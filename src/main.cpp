
#include "ofApp.h"
#include "ofMain.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main(){

	ofSetupOpenGL(1366,768, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
    
    ofRunApp(new ofApp());
}

// First Tasks:

// Make sure the rectangle stays within the hands and when it is one hand it stays a certain size and follows it in the center.

//When using one hand or a hand goes away the program knows to stay at a fixed width and height.


//Need to set minimum size for one hand interaction so it can be seen
//One hand interaction needs to be revised, it does not have a fixed window yet and there is interferance with another hand while it happens

//The variables that limit the window needs to be redone as well, currently it does not follow the hands when close to the sides

//Having  global size works currently, but once the project goes over two screeens there will be a problem with sizes as ofGetHeight() etc. take in both screens.

//There are two webcam grabbers, one is for the actual webcam and the other was for CamTwist. The CamTwist one is what would be recorded to get the entire screen.
// You need to make it so both webcams can be working at once.