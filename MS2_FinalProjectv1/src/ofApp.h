#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofxOpenCv.h"
#include "ofxBox2d.h"
#include "ofxTrueTypeFontUC.h"
//#include "ofxKinect.h"
#include "ofMain.h"

#define MAX_DEVICES 2

// A simple little Data class. This is were
// you can store anything you want.
class Data {
public:
    ofColor color;
    string  name;
    int		id;
    int		UserID;
    bool bHit;
    int myFont;
};


// ------------------------------------------------- a simple extended box2d circle
class CustomParticle : public ofxBox2dCircle {
    
public:
    
    ofxTrueTypeFontUC myFontc;
    ofxTrueTypeFontUC myFontk;
    
    void setupTheCustomData(ofTrueTypeFont setFont) {
        
        myFontc.loadFont("Sharp.ttf", 14, true, true);
        myFontk.loadFont("Korean.ttf", 14, true, true);
        
        static int colors[] = {0x303642, 0xe87358, 0x6bb8c2};
        
        static string abc[] = {"你好","Ciao","Bonjour","Hello","¡Hola!","Kumusta","Namaste","Salaam","안녕하세요","こんにちは"};
        int arraysize = sizeof(abc)/sizeof(abc[0]);
        int getNum = (int)ofRandom(0, arraysize);
        
        // we are using a Data pointer because
        // box2d needs to have a pointer not
        // a referance
        setData(new Data());
        Data * theData = (Data*)getData();
        
        theData->id = ofRandom(0, 100);
        //        theData->name += abc[(int)ofRandom(0, abc.size())];
        theData->name += abc[getNum];
        theData->bHit = false;
        
        if(getNum == 0) theData->myFont = 0;
        else theData->myFont = 1;
        
        theData->color.setHex(colors[(int)ofRandom(0, 3)]);
        
//        printf("setting the custom data!\n");
    }
    
    void draw() {
        Data* theData = (Data*)getData();
        if(theData) {
            
            // Evan though we know the data object lets just
            // see how we can get the data out from box2d
            // you would use this when using a contact listener
            // or tapping into box2d's solver.
            float radius = getRadius();
            ofPushMatrix();
            ofTranslate(getPosition());
            ofRotateZ(getRotation());
            ofSetColor(theData->color);
            ofFill();
//            ofRectangle(0,0,100,100);
//            ofDrawCircle(0, 0, radius);
            
            float textSize = radius/10;
            ofPushMatrix();
//            ofScale(textSize, textSize);
            
            if(theData->bHit){
                ofSetColor(255,0,0);
            }
            else{
                ofSetColor(theData->color);
            }
//            myFont.drawString(theData->name, -textSize*5, textSize);
            
            if(theData->myFont == 0){
                myFontc.drawString(theData->name, -textSize*5, textSize);
            }
            else{
                myFontk.drawString(theData->name, -textSize*5, textSize);
            }

            
//            ofDrawBitmapString(theData->name, -textSize, textSize);
            ofPopMatrix();
            
            ofPopMatrix();
        }
    }
    
};

class ofApp : public ofBaseApp{

public:
    
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

private:
    
    void handEvent(ofxOpenNIHandEvent & event);

	ofxOpenNI openNIkinect;
//    ofxKinect kinect;
    
    ofxCvColorImage         colorImg;
    ofxCvGrayscaleImage     grayImage; // grayscale depth image
    ofxCvGrayscaleImage		debugImage;
    ofxCvGrayscaleImage     grayThreshNear; // the near thresholded image
    ofxCvGrayscaleImage     grayThreshFar; // the far thresholded image
    
    ofxCvContourFinder      contourFinder;
    
    bool                    bThreshWithOpenCV;
    bool                    bDrawPointCloud;
    
    int                     nearThreshold;
    int                     farThreshold;
    int                     angle;
    bool                    mode;
    ofTrueTypeFont          verdana;
//    ofxTrueTypeFontUC       ucVerdana;
    
    float                                   px, py;
    bool                                    bDrawLines;
    bool                                    bMouseForce;
    
    ofxBox2d                                box2d;			  //	the box2d world
    ofPolyline                              drawing;		  //	we draw with this first
    ofxBox2dEdge                            edgeLine;		  //	the box2d edge/line shape (min 2 points)
    vector		<shared_ptr<ofxBox2dCircle> >	circles;		  //	default box2d circles
    vector		<shared_ptr<ofxBox2dRect> >		boxes;			  //	defalut box2d rects
    vector      <ofPtr<ofxBox2dCircle> >        contourCircles;
    vector		<shared_ptr<ofxBox2dRect> >		contourBoxes;			  //	defalut box2d rects
    
    vector <shared_ptr<CustomParticle> >		particles;
    
    // this is the function for contacts
    void contactStart(ofxBox2dContactArgs &e);
    void contactEnd(ofxBox2dContactArgs &e);

    
};

#endif
