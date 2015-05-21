#include "testApp.h"
#include "stdio.h"
#include "ofUtils.h"
#include "global.h"
//--------------------------------------------------------------
void testApp::setup() {
    
    // Setup Environment Variables
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    // ofSetFrameRate(framerate);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofLogToConsole();
    
    //fbo
    maskFbo.allocate(resX,resY);
    fbo.allocate(resX,resY);
    
    
    
    
#ifdef TARGET_OPENGLES
    shader.load("shaders_gles/alphamask.vert","shaders_gles/alphamask.frag");
#else
    if(ofGetGLProgrammableRenderer()){
    	string vertex = "#version 150\n\
    	\n\
		uniform mat4 projectionMatrix;\n\
		uniform mat4 modelViewMatrix;\n\
    	uniform mat4 modelViewProjectionMatrix;\n\
    	\n\
    	\n\
    	in vec4  position;\n\
    	in vec2  texcoord;\n\
    	\n\
    	out vec2 texCoordVarying;\n\
    	\n\
    	void main()\n\
    	{\n\
        texCoordVarying = texcoord;\
        gl_Position = modelViewProjectionMatrix * position;\n\
    	}";
		string fragment = "#version 150\n\
		\n\
		uniform sampler2DRect tex0;\
		uniform sampler2DRect maskTex;\
        in vec2 texCoordVarying;\n\
		\
        out vec4 fragColor;\n\
		void main (void){\
		vec2 pos = texCoordVarying;\
		\
		vec3 src = texture(tex0, pos).rgb;\
		float mask = texture(maskTex, pos).r;\
		\
		fragColor = vec4( src , mask);\
		}";
		shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
		shader.bindDefaults();
		shader.linkProgram();
    }else{
		string shaderProgram = "#version 120\n \
		#extension GL_ARB_texture_rectangle : enable\n \
		\
		uniform sampler2DRect tex0;\
		uniform sampler2DRect maskTex;\
		\
		void main (void){\
		vec2 pos = gl_TexCoord[0].st;\
		\
		vec3 src = texture2DRect(tex0, pos).rgb;\
		float mask = texture2DRect(maskTex, pos).r;\
		\
		gl_FragColor = vec4( src , mask);\
		}";
		shader.setupShaderFromSource(GL_FRAGMENT_SHADER, shaderProgram);
		shader.linkProgram();
    }
#endif
    
    
    maskFbo.begin();
    ofClear(0,0,0,255);
    maskFbo.end();
    
    fbo.begin();
    ofClear(0,0,0,255);
    fbo.end();
    
    // Video Configuration
    
    videoDevices = vidGrabber.listDevices();
    ofSetVerticalSync(true);
    vidGrabber.setDeviceID(0);
    //vidGrabber.initGrabber(resX, resY);
    
    
    
    
    
    // pointer for us, so no need to delete later.
    vidRecorder = ofPtr<ofQTKitGrabber>( new ofQTKitGrabber() );
    
    // 2. Set our video grabber to use this source.
    vidGrabber.setGrabber(vidRecorder);
    
    // 3. Make lists of our audio and video devices.
    videoDevicesss = vidRecorder->listVideoDevices();
    
    // 3a. Optionally add audio to the recording stream.
    // vidRecorder->setAudioDeviceID(2);
    // vidRecorder->setUseAudio(true);
    
	// 4. Register for events so we'll know when videos finish saving.
	ofAddListener(vidRecorder->videoSavedEvent, this, &testApp::videoSaved);
	
    // 4a.  If you would like to list available video codecs on your system,
    // uncomment the following code.
    // vector<string> videoCodecs = vidRecorder->listVideoCodecs();
    // for(size_t i = 0; i < videoCodecs.size(); i++){
    //     ofLogVerbose("Available Video Codecs") << videoCodecs[i];
    // }
	
	// 4b. You can set a custom / non-default codec in the following ways if desired.
    // vidRecorder->setVideoCodec("QTCompressionOptionsJPEGVideo");
    // vidRecorder->setVideoCodec(videoCodecs[2]);
	
    // 5. Initialize the grabber.
    vidGrabber.initGrabber(resX, resY);
    
    // If desired, you can disable the preview video.  This can
    // help help speed up recording and remove recording glitches.
    // vidRecorder->initGrabberWithoutPreview();
    
    // 6. Initialize recording on the grabber.  Call initRecording()
    // once after you've initialized the grabber.
    vidRecorder->initRecording();
    
    // 7. If you'd like to launch the newly created video in Quicktime
    // you can enable it here.
    bLaunchInQuicktime = true;
    
    
    
    
    
    
    
    
    // OpenNI Setup
    openNIDevice.setup();
    
    openNIDevice.getDepthTextureReference();
    openNIDevice.getFrameRate();
    
    // openNIDevice.addImageGenerator();
    openNIDevice.addDepthGenerator();
    openNIDevice.addGestureGenerator();
    openNIDevice.setRegister(true);
    openNIDevice.setMirror(true);
    openNIDevice.addHandsGenerator();
    openNIDevice.setMaxNumHands(2);
    
    openNIDevice.addAllGestures();
    openNIDevice.addUserGenerator();
    
    openNIDevice.start();
    
    // Resources
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
    
    // Video Recorder
    fileName = "interaction";
    fileExt = ".mkv"; //
}

//--------------------------------------------------------------
void testApp::update(){
    
    // Inputs
    openNIDevice.update();
    vidGrabber.update();
    if(recordedVideoPlayback.isLoaded()){
        recordedVideoPlayback.update();
    }
    
    // Outputs
    updateCoordinates();
    updateInteraction();
    
    
    //Matt this updates the mask so only its position is revealed
    maskFbo.begin(); //Turn this off to see a cool spray paint effect
    ofClear(0,0,0,255);
    maskFbo.end();
    
    
    if(interacting){
        if(!bRecording){
            // start a new recording.
            // before starting, make sure that the video file
            // is already in use by us (i.e. being played), or else
            // we won't be able to record over it.
            if(recordedVideoPlayback.isLoaded()){
                recordedVideoPlayback.close();
            }
            vidRecorder->startRecording("MyMovieFile.mov");
            bRecording = true;
        }
    }
    else{
        //if it is recording, stop
        if(vidRecorder->isRecording()){
            vidRecorder->stopRecording();
        }
        bRecording = false;
    }
    
    
    
    
    
    
    
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofBackground(0);
    
    ofFill();
    
    
    fbo.draw(0,0);
    
    
    // Start User Interaction
    if(openNIDevice.getNumTrackedHands() > 0){
        
        if(openNIDevice.getNumTrackedHands()==1)
            moveGesture();
        
        
        if(openNIDevice.getNumTrackedHands()==2)
            resizeGesture();
        
        
        drawHands();
    }
//    else
//    {
//        // No hands - reset interaction
//        
//        if(openNIDevice.getNumTrackedUsers() > 0)
//            openNIDevice.resetUserTracking(0);
//    }
//    
    displayDebug();
    
    playInteractions();
    
    
    
    
    
    
    
    // draw the playback video
    if(recordedVideoPlayback.isLoaded()){
        ofPushStyle();
        // fit it into the preview window, but use the correct aspect ratio
        ofRectangle recordedRect(ofRectangle(200,200,w,h));
        recordedVideoPlayback.draw(recordedRect);
        ofPopStyle();
    }
    
    ofPushStyle();
    ofNoFill();
    
    
    ofRectangle(hand1pos.x, hand1pos.y, w, h);
    ofPopStyle();
    
    
    
    
    
    //  }
}
//--------------------------------------------------------------
void testApp::exit(){
    openNIDevice.stop();
    
    vidGrabber.close();
    if (MemoryPlayback.isLoaded()) {
        MemoryPlayback.close();
    }
}

//--------------------------------------------------------------
// Update Methods
//--------------------------------------------------------------
/*
 Sets Coordinates For Each Hand
 */
void testApp::updateCoordinates(){
    int numHands = openNIDevice.getNumTrackedHands();
    
    if(numHands > 0){
        interacting = true;
        
        if(numHands == 1)
        {
            // Get Hand
            ofxOpenNIHand & hand = openNIDevice.getTrackedHand(0);
            
            hand1pos.set(coordinateProcessor(hand.getPosition()));
            // hand2pos.zero(); // null inactive hand
            hand2pos.x = 0; // null inactive hand
            hand2pos.y = 0;
        }
        if(numHands == 2)
        {
            // Get Hands
            ofxOpenNIHand & handA = openNIDevice.getTrackedHand(0);
            ofxOpenNIHand & handB = openNIDevice.getTrackedHand(1);
            
            // Find Hand 1 (Hand 1 must always be to the left of the interaction window)
            if(handA.getPosition().x < handB.getPosition().x)
            {
                // HandA = hand1
                hand1pos.set(coordinateProcessor(handA.getPosition()));
                hand2pos.set(coordinateProcessor(handB.getPosition()));
            }
            else {
                // HandB = hand1
                hand1pos.set(coordinateProcessor(handB.getPosition()));
                hand2pos.set(coordinateProcessor(handA.getPosition()));
            }
        }
        // Run interaction pre processor
        interactionPreProcessor(); //  Sets interaction window size
    }
    else
    {
        // No hands (null each hand)
        interacting = false;
        hand1pos.zero();
        hand2pos.zero();
        w = NULL;
        h = NULL;
    }
}

/*
 Modifies Hand Positions To Consider Interaction Size
 */
void testApp::interactionPreProcessor()
{
    if(hand2pos.x != 0 && hand2pos.y != 0)
    {
        // Set Interaction Window Size
        w = round(hand2pos.x - hand1pos.x);
        
        if(hand1pos.y > hand2pos.y){ // Check if the y axis is inverted
            h = round(hand1pos.y - hand2pos.y);
        }
        else
        {
            h = round(hand2pos.y - hand1pos.y);
        }
        
    }
    else
    {
        if(hand1pos.x + w/2 > resX - 5 )
        {
            tempRectPosX = resX - 5 - w/2;
        }
        else if(hand1pos.x - w/2 < 0 + 5)
        {
            tempRectPosX = 5 + w/2;
        }
        else
        {
            tempRectPosX = hand1pos.x;
        }
        if(hand1pos.y + h/2 > resY - 5 )
        {
            tempRectPosY = resY - 5 - h/2;
        }
        else if(hand1pos.y - h/2 < 0 + 5)
        {
            tempRectPosY = 5 + h/2;
        }
        else
        {
            tempRectPosY = hand1pos.y;
        }
        
    }
}


void testApp::updateInteraction(){
    
    
    maskFbo.begin();
    if(hand1pos.y < hand2pos.y || openNIDevice.getNumTrackedHands()==1 ){
        ofRect(hand1pos.x, hand1pos.y, w, h);
    } else
    {
        ofRect(hand1pos.x, hand1pos.y-h, w, h);
    }
    
    maskFbo.end();
    
    // HERE the shader-masking happends
    //
    fbo.begin();
    // Cleaning everthing with alpha mask on 0 in order to make it transparent for default
    ofClear(0, 0, 0, 0);
    
    shader.begin();
    shader.setUniformTexture("maskTex", maskFbo.getTextureReference(), 1 );
    
    vidGrabber.draw(0,0);
    
    shader.end();
    fbo.end();
    
}

void testApp::updateRecorder(){
    
    
    
    
    
    
}

// Resize Gesture
void testApp::resizeGesture(){
    if(!interacting) interacting = true; // Start Interaction
    
    // Resize Interaction Window
    if(hand1pos.y < hand2pos.y)//Matt put this to autocorrect position of window
    {
        //interactionWindow.set(hand1pos.x, hand1pos.y, w, h);
        
        // ofRect(hand1pos.x, hand1pos.y, w, h);
    }
    else
    {
        //interactionWindow.set(hand1pos.x, hand1pos.y-h, w, h);
        //ofRect(hand1pos.x, hand1pos.y-h, w, h);
    }
}

// Move Gesture
void testApp::moveGesture(){
    if(interacting){ // Check if interacting
        
        // Move Interaction Window
        interactionWindow.set(hand1pos.x, hand1pos.y, w, h);
    }
    else
    {
        cout << "Move Gesture Without Interaction\n";
        ofLog(OF_LOG_NOTICE, "Move Gesture Without Interaction");
    }
}


//--------------------------------------------------------------
// Draw Methods
//--------------------------------------------------------------


/*
 Draws the position of the users hands onscreen
 */
void testApp::drawHands(){
    //ofSetColor(255,0,0); // red
    
    if(hand1pos.x != 0 && hand1pos.y != 0)
    {
        ofRect(hand1pos.x-5, hand1pos.y-5, 5, 5);
        
    }
    if(hand2pos.x != 0 && hand2pos.y != 0)
    {
        ofRect(hand2pos.x-5, hand2pos.y-5, 5, 5);
        
    }
    
}

void testApp::playInteractions(){
    
    
    //   // Draw playback video
    //    if(MemoryPlayback.isLoaded())
    //    {
    //        ofPushStyle();
    //        ofFill();
    //        ofSetColor(255);
    //       ofRectangle recordedRect(ofRectangle(0,0, MemoryPlayback.getWidth(), MemoryPlayback.getHeight()));
    //        recordedRect.scaleTo(ofRectangle(0,0,1920,1080));
    //        MemoryPlayback.draw(recordedRect);
    //        ofPopStyle();
    //    }
}

//--------------------------------------------------------------
// Event Handlers
//--------------------------------------------------------------
void testApp::userEvent(ofxOpenNIUserEvent & event){
    
}
//--------------------------------------------------------------
// Helper Functions
//--------------------------------------------------------------

ofPoint testApp::coordinateProcessor(ofPoint coordinates)
{
    
    float x = coordinates.x;
    float y = coordinates.y;
    // Multiplies coordinates by the ratio between the max interaction size possible & the desired resoloution
    x = x * (resX / resizeMaxWidth);
    y = y * (resY / resizeMaxHeight);
    
    // Trim coordinates to ensure its possible to reach the top & left boundraies
    // x = x + trimX;
    // y = y + trimY;
    
    // Prevents coodinates from exceeding the bounds of the screen
    if(x > resX) x = resX;
    if(y > resY) y = resY;
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    
    // Returns results as type ofPoint
    ofPoint result;
    result.set(x, y);
    return result;
}



// Displays Debug Information
void testApp::displayDebug(){
    if(enableDebug)
    {
        //ofRectangle previewWindow(1760,960,160,120); // shows live feed
        int startY = 450;
        
        
        
        // draw video device selection
        ofDrawBitmapString("Video Device", 0, startY+=20);
        for(int i = 0; i < videoDevices.size(); i++){
            ofPushStyle();
            if(i == 0){
                ofSetColor(255, 100, 100); // make current device display orange
            }
            else{
                ofSetColor(255);
            }
            ofDrawBitmapString(videoDevices[i].deviceName, 0, startY+=20);
            ofPopStyle();
        }
        
        
        ofPushStyle();
        //ofSetColor(255);
        
        startY+=20;
        
        ofDrawBitmapString("FPS:" + ofToString(round(ofGetFrameRate())), 0, startY+=20);
        
        if(w)
            ofDrawBitmapString("Width: " + ofToString(w), 0, startY+=20);
        
        if(h)
            ofDrawBitmapString("Height: " + ofToString(h), 0, startY+=20);
        
        
        ofDrawBitmapString("Interacting? " + ofToString(interacting?"Yes":"No"), 0, startY+=20);
        ofDrawBitmapString("Recording? " + ofToString(bRecording?"Yes":"No"), 0, startY+=20);
        ofDrawBitmapString("Max Width: " + ofToString(resX) + " Max Height: " + ofToString(tempRectPosX), 20, 800);
        /* ofDrawBitmapString((bRecording?"Pause":"Start") + ofToString(" recording: r"), 0, startY+=20);
         ofDrawBitmapString((bRecording?"Close current video file: c":""), 0, startY+=20);
         
         ofDrawBitmapString("Total Video Played: " + ofToString(iVidPlayCount), 0, startY+=20);
         ofDrawBitmapString("Total Times Played: " + ofToString(ofGetElapsedTimeMillis()/1000 ), 0, startY+=20);
         ofDrawBitmapString("Total Interaction Played: " + ofToString(iInteractionTimer), 0, startY+=20);
         ofDrawBitmapString("Current Video Time: " + ofToString(iVidTimer), 0, startY+=20);
         ofDrawBitmapString("Largest Video Time: " + ofToString(iTotalVidTime), 0, startY+=20);
         ofDrawBitmapString("Are You Interacting: " + ofToString(bInteracting?"Yes":"No"), 0, startY+=20);
         startY+=20;
         ofDrawBitmapString("Controls", 0, startY+=20);
         ofDrawBitmapString("' ' space bar to toggle recording", 0,startY+=20);
         ofDrawBitmapString("'v' switches audio device", 0, startY+=20);
         ofDrawBitmapString("'a' switches audio device", 0, startY+=20);
         */
        
        if(openNIDevice.getNumTrackedUsers() > 0){
            ofDrawBitmapString("User is interacting", 0, startY+=20);
            for (int i = 0; i < openNIDevice.getNumTrackedUsers(); i++) {
                ofxOpenNIUser & user = openNIDevice.getTrackedUser(i);
            }
            if(openNIDevice.getNumTrackedHands() > 0){
                ofDrawBitmapString("Hands Tracking: " + ofToString(openNIDevice.getNumTrackedHands()), 0, startY+=20);
            }
            
            ofPopStyle();
        }
    }
    
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == ' '){
        
        //if it is recording, stop
        if(vidRecorder->isRecording()){
            vidRecorder->stopRecording();
        }
        else {
            // otherwise start a new recording.
            // before starting, make sure that the video file
            // is already in use by us (i.e. being played), or else
            // we won't be able to record over it.
            if(recordedVideoPlayback.isLoaded()){
                recordedVideoPlayback.close();
            }
	        vidRecorder->startRecording("MyMovieFile.mov");
        }
    }
    
    
    
    
    
    
    
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}


void testApp::videoSaved(ofVideoSavedEventArgs& e){
    if(e.error.empty()){
     	if(e.error.empty()){
            recordedVideoPlayback.loadMovie(e.videoPath);
            recordedVideoPlayback.play();
            
            if(bLaunchInQuicktime) {
                ofSystem("open " + e.videoPath);
            }
        }
        else {
            ofLogError("videoSavedEvent") << "Video save error: " << e.error;
        }
    }
}


//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}
