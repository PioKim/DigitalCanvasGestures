#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"

#define MAX_DEVICES 2

class ofApp : public ofBaseApp{
public:
    
    bool debug;
    
    // temp
    int tempRectPosX = 0;
    int tempRectPosY = 0;
    
    // Enviroment
    int kinectX = 640;
    int kinectY = 480;
    int resX = ofGetWidth();
    int resY = ofGetHeight();
    int framerate = 30;
    
    // Interaction Variables
    float x = 0;
    float y = 0;
    float w = 0;
    float h = 0;
    
    float sw ;
    float sh ;
    bool interacting = false;
    bool waving= false;
    ofRectangle interactionWindow;
    ofImage videoFeed;
    
    float x1;
    float y1;
    float x2;
    float y2;
    
    
    // Hands
    ofPoint hand1pos;
    ofPoint hand2pos;
    //circle
    float 	counter;

    
    
    
    struct yBall {
        int x = ofRandom(ofGetWidth()/2, ofGetWidth());
        int y = ofRandom(0, ofGetHeight());
        int vx ;
        int vy  ;  };

    
    
    
    
    struct wBall {
        int x = ofRandom(0, ofGetWidth()/2);
        int y = ofRandom(0, ofGetHeight());
        int vx ;
        int vy  ;  };

    //Ball
    yBall yellowBall;
    wBall whiteBall;
    

    
    // Interaction Config
    // int resizeMaxWidth = 1464;
    // int resizeMaxHeight = 883;
    int resizeMaxWidth = 640;
    int resizeMaxHeight = 480;
    int trimX = -150;
    int trimY = -200;
    
    
    GestureGenerator gesture_generator;
    ofxOpenNIGestureEvent gesture;
    
    
    // Video
    vector<ofVideoDevice> videoDevices;
    ofVideoGrabber      vidGrabber;
    
    
    ofPtr<ofQTKitGrabber>	vidRecorder;
    
    
    ofVideoPlayer recordedVideoPlayback;
    
    void videoSaved(ofVideoSavedEventArgs& e);
    vector<string> videoDevicesss;
    
    bool bLaunchInQuicktime;
    
    
    //ofxVideoRecorder    vidRecorder;
    ofSoundStream       soundStream;
    bool isRecording;
    int sampleRate;
    int channels;
    string fileName;
    string fileExt;
    ofFbo recordFbo;
    ofPixels recordPixels;
    ofImage recordingWindow;
    
    // OpenFrameworks Methods
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    // Digital Canvas Methods
    ofPoint coordinateProcessor(ofPoint);
    void interactionPreProcessor();
    void updateInteraction();
    void drawInteraction();
    
    void drawHands();
    void startRecording();
    void updateRecorder();
    void stopRecording();
    void saveVideo();
    void swapVideos();
    void playInteractions();
    void resizeGesture();
    void moveGesture();
    void displaySilhouette();
    void displayDebug();
    void updateCoordinates();
    
    void countDown();
    void screenSaver();
    
    // New Variables
    bool enableDebug = true;
    int recordingStartTime = 0;
    ofImage userMask;
    
    SceneMetaData sceneMetaData;
    unsigned short* depthPixels;
    
    ofEasyCam cam;
    
    ofShader    shader;
    ofPixels maskPixels;
    
    ofImage maskImage;
    ofxOpenNI openNIDevice;
    // Events
    void userEvent(ofxOpenNIUserEvent & event);
    void gestureEvent(ofxOpenNIGestureEvent & event);
    
    
    int nearThreshold = 100;
    int farThreshold = 10000;
    unsigned char *		depth_pixels;
    
    
    
    //Touch events
    //    void TouchDown(Blob b);
    //    void TouchMoves(Blob b);
    //    void TouchUp(Blob b);
    
    
    //FBO for webcam
    ofFbo       maskFbo;
    ofFbo       fbo;
    
    
    
    //webCam
    //ofVideoGrabber VidGrabber;
    
    //recorder
    //ofPtr<ofQTKitGrabber> fakeRecorder; //This is still needed for vid recorder to work
    
    
    //screensaver
    int interactionTime ;
    bool saver;
    int screenSaverTime;
    //timer
    float startTime; // store when we start time timer
    float endTime; // when do want to stop the timer
    bool  bTimerReached; // used as a trigger when we hit the timer

    
    
    //Video
    ofVideoPlayer MemoryPlayback;
    
    
    //blending
    ofBlendMode blendMode;
    
    // Check video should record once
    bool bRecording = false;
    
    //ofFbo recordFbo;
    // ofPixels recordPixels;
    
    int iVidPlayCount = 0;
    int iTotalVideos = 3;
    bool bUpperHalf = false;
    
    // Video recording variables
    /////// SHIT LIST ////////
    int iInteractionTimer = 0;
    int iStartTimer = 0;
    int iVidTimer = 0;
    int iTotalVidTime = 0;
    
    bool bIncludeVid = false;
    bool bVidRecording = false;
    bool b0Record = true;
    
    //Hand tracking event
    
private:
    
    
    ofTrueTypeFont verdana;
    
};

#endif
