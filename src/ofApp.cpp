#include "ofApp.h"
#include "stdio.h"
#include "ofUtils.h"
#include "global.h"
//--------------------------------------------------------------
void ofApp::setup() {
    
    // Setup Environment Variables
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    // ofSetFrameRate(framerate);
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofLogToConsole();
    //timer
    startTime = ofGetElapsedTimeMillis();
     bTimerReached = false;
    
    //fbo
    maskFbo.allocate(resX,resY);
    fbo.allocate(resX,resY);
    
    //set uo two object balls
    yellowBall.x = ofRandomWidth();
    yellowBall.y = ofRandomHeight();
    yellowBall.vx = ofRandom(-10,10);
    yellowBall.vy = ofRandom(-10,10);
    
    whiteBall.x = ofRandomWidth();
    whiteBall.y = ofRandomHeight();
    whiteBall.vx = ofRandom(-10,10);
    whiteBall.vy = ofRandom(-10,10);
    

    y1 = 100;
    x1 = 100;
    y2 = 200;
    x2 = 200;
    
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
    ofBackground(0,0,0);
    
    
    
    bTimerReached = false;
    startTime = ofGetElapsedTimeMillis();  // get the start time
    endTime = (int)ofRandom(1000, 3000);
    
    
    
    
    
    
    
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
    ofAddListener(vidRecorder->videoSavedEvent, this, &ofApp::videoSaved);
    
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
    
    
    
    
    //dsjhfufkh
    
    
    
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
    
    
    //gesture
    // setup the hand generator
    
    // add all focus gestures (ie., wave, click, raise arm)
    openNIDevice.addAllHandFocusGestures();
    
    // or you can add them one at a time
    vector<string> gestureNames = openNIDevice.getAvailableGestures(); // you can use this to get a list of gestures
    // prints to console and/or you can use the returned vector
    openNIDevice.addHandFocusGesture("Wave");
    
    
    //possible
    
    
    //openNIDevice.isGestureAvailable("Wave");
    //openNIDevice.isGestureOn();
    
    
    
    
    
    
    
    
    openNIDevice.start();
    
    
    
    // Resources
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
    
    // Video Recorder
    fileName = "interaction";
    fileExt = ".mkv"; //
    
    
    
    
    
    
    
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
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
    
    
    
    
    // ball xy
    
    yellowBall.x = yellowBall.x + yellowBall.vx;
    yellowBall.y = yellowBall.y + yellowBall.vy;
    
    whiteBall.x = whiteBall.x + whiteBall.vx;
    whiteBall.y = whiteBall.y + whiteBall.vy;
    
    if (yellowBall.x <0 || ofGetWidth()/2>yellowBall.x ) {
        yellowBall.vx = -yellowBall.vx;
    }
    if (yellowBall.y<0 || yellowBall.x>ofGetHeight()) {
        yellowBall.vy =-yellowBall.vy;
    }
    
    //white
    if (whiteBall.x<0 || whiteBall.x > ofGetWidth()/2) {
        whiteBall.vx = -whiteBall.vx;
    }
    
    if (whiteBall.y<0 || whiteBall.y > ofGetHeight()) {
        whiteBall.vy = -whiteBall.vy;
    }
    
    
   //yellow
    if (yellowBall.x<0 || yellowBall.x > ofGetWidth()) {
        yellowBall.vx = -yellowBall.vx;
    }
    
    if (yellowBall.y<0 || yellowBall.y > ofGetHeight()) {
        yellowBall.vy = -yellowBall.vy;
    }

    
    
    
    //screenSaver();
    
}








//
//  while(interacting == false && ofGetElapsedTimeMillis()/30 ){
//        float x1 = 550+300*cos(ofGetElapsedTimef()*1.0f);// this
//        float y1 = 300+300*sin(ofGetElapsedTimef()/3.5f);// this
//
//
//        ofEnableAlphaBlending();
//        //ofFill();
//
//        ofCircle(x1,y1,40);// i want
//
//        //ofRectangle(hand1pos.x, hand1pos.y, w, h);
//        ofDisableAlphaBlending();
//    }




//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0);
    
    
    
    
    
    
    
    ofFill();
    
    fbo.draw(0,0);
    
    
    // Start User Interaction
    
    if(openNIDevice.getNumTrackedHands()==1){
        moveGesture();
    }
    
    drawHands();
    
    displayDebug();
    
    playInteractions();
    
    
    //timer bar
    if(interacting == true){
        bTimerReached = false;
        startTime = ofGetElapsedTimeMillis();  // get the start time
        endTime = (int)ofRandom(10000, 10000); // in milliseconds
    }
    ofPushStyle();
    float barWidth = 150;
    float timer = ofGetElapsedTimeMillis() - startTime;
    float pct = ofMap(timer, 0.0, endTime, 0.0, 1.0, true);
    ofSetHexColor(0xf02589);
    ofRect((ofGetWidth()-barWidth)/8, ofGetHeight()/8, barWidth*pct, 30);//this

    string  info = "Screen saverPercentage: "+ofToString(pct*100, 1)+"%\n";
    ofSetColor(255);
    ofDrawBitmapString(info, 20, 20);
    ofPopStyle();
    
    
    
    screenSaver();
    


    
    // draw the playback video
    if(recordedVideoPlayback.isLoaded()){
        ofPushStyle();
        // fit it into the preview window, but use the correct aspect ratio
        ofRectangle recordedRect(ofRectangle(200,200,w,h));
        recordedVideoPlayback.draw(recordedRect);
        ofPopStyle();
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
   // screenSaver();// screen saver
    
    ofPushStyle();
    ofNoFill();
    
    
    ofRectangle(hand1pos.x, hand1pos.y, w, h);
    ofPopStyle();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    //  }
}
//--------------------------------------------------------------
void ofApp::exit(){
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
 screen saver
 */
void ofApp::screenSaver(){
    float timer = ofGetElapsedTimeMillis() - startTime;

    interactionTime = ofGetElapsedTimef();
    
    
    
    
    
    
    
    
    if(timer >= endTime ) {
        bTimerReached = true;

        
//        float x1 =300+220*cos(ofGetElapsedTimef()/5.0f);// this control x moving and speed
//        float y1 =400+220*cos(ofGetElapsedTimef()/4.2f);// this control y moving and speed

        
       // float x2 = 1000+220*sin(ofGetElapsedTimef()/5.0f);// this control x moving and speed
       // float y2 = 400+220*sin(ofGetElapsedTimef()/10.0f);// this control y moving and speed
        

        x1 += ofRandom(0, 2) - 1 ;// this control x moving and speed
        y1 += ofRandom(0, 2) - 1;// this control y moving and speed
        
        
          x2 = 1000+220*sin(ofGetElapsedTimef()/5.0f);// this control x moving and speed
          y2 = 400+220*sin(ofGetElapsedTimef()/10.0f);// this control y moving and speed

        
        
        
        
        ofCircle(x1,y1,20);

        ofCircle(x2,y2,20);
        
        
        if(x2 != 0 && y2 !=0){
        sw = round(x2 - x1);
            
            
        if(y1 > y2){ //
            sh = round(y1 - y2);
        }
        else
        {
            sh = round(y2 - y1);
        }
        }

        maskFbo.begin();
        if(y1 < y2 ){
            ofRect(x1, y1, sw, sh);
        }else{
            ofRect(x1, y1-sh, sw, sh);
        }
        
        maskFbo.end();
        
        
        
        
        
        
        
        
        
//        ofSetColor(255, 255, 0);
//        ofCircle(yellowBall.x, yellowBall.y, 20);
//        
//        ofSetColor(255, 255, 255);
//        ofCircle(whiteBall.x, whiteBall.y, 20);
}
}
    













/*
 Sets Coordinates For Each Hand
 */
void ofApp::updateCoordinates(){
    int numHands = openNIDevice.getNumTrackedHands();
    
    if(numHands > 0){
        interacting = true;
        
        if(numHands == 1)
        {
            // Get Hand
            ofxOpenNIHand & handA = openNIDevice.getTrackedHand(0);
            
            hand1pos.set(coordinateProcessor(handA.getPosition()));
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
    else if(numHands ==0 )
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
void ofApp::interactionPreProcessor()
{
    if(hand2pos.x != 0 && hand2pos.y != 0){
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


void ofApp::updateInteraction(){
    
    
    maskFbo.begin();
    if(hand1pos.y < hand2pos.y ||openNIDevice.getNumTrackedHands()==1){
        ofRect(hand1pos.x, hand1pos.y, w, h);
    }else{
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




void ofApp::updateRecorder(){
    
    
    
    
    
    
}

// Resize Gesture
void ofApp::resizeGesture(){
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
void ofApp::moveGesture(){
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
void ofApp::drawHands(){
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

void ofApp::playInteractions(){
    
    
    
    
    
    
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




//--------------------------------------------------------------
// Helper Functions
//--------------------------------------------------------------

ofPoint ofApp::coordinateProcessor(ofPoint coordinates)
{
    
    int x = coordinates.x;
    int y = coordinates.y;
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
void ofApp::displayDebug(){
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
                //ofSetColor(255);
            }
            ofDrawBitmapString(videoDevices[i].deviceName, 0, startY+=20);
            ofPopStyle();
        }
        
        
        ofPushStyle();
        //ofSetColor(255);
        
        
        startY+=20;
        
        ofDrawBitmapString("FPS:" + ofToString(round(ofGetFrameRate())), 0, startY+=20);
        
        ofDrawBitmapString("Total Times Played: " + ofToString(interactionTime), 0, startY+=20);
        
        
        
        
       
        
        
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
        
        
        // ofDrawBitmapString("Waving?: " + ofToString(waving? "Yes":"No"), 0, startY+=20);
        
        // ofDrawBitmapString("Waving?: " + ofToString(openNIDevice.isGestureOn()), 0, startY+=20);
        
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
void ofApp::keyPressed(int key){
    //    if(key == ' '){
    //
    //        //if it is recording, stop
    //        if(vidRecorder->isRecording()){
    //            vidRecorder->stopRecording();
    //        }
    //        else {
    //            // otherwise start a new recording.
    //            // before starting, make sure that the video file
    //            // is already in use by us (i.e. being played), or else
    //            // we won't be able to record over it.
    //            if(recordedVideoPlayback.isLoaded()){
    //                recordedVideoPlayback.close();
    //            }
    //            vidRecorder->startRecording("MyMovieFile.mov");
    //        }
}










//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}


void ofApp::videoSaved(ofVideoSavedEventArgs& e){
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
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
