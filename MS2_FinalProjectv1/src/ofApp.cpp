#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetLogLevel(OF_LOG_VERBOSE);

    nearThreshold = 230;
    farThreshold = 70;
    bThreshWithOpenCV = true;
    
    openNIkinect.setup();
    openNIkinect.setRegister(true);
    openNIkinect.setMirror(true);
    openNIkinect.addImageGenerator();
    openNIkinect.addDepthGenerator();
    openNIkinect.addUserGenerator();
    openNIkinect.setMaxNumUsers(4);
    
    mode = false;
    
    // setup the hand generator
    openNIkinect.addHandsGenerator();
    
    // add all focus gestures (ie., wave, click, raise arm)
    openNIkinect.addAllHandFocusGestures();
    
    // or you can add them one at a time
    //vector<string> gestureNames = openNIDevice.getAvailableGestures(); // you can use this to get a list of gestures
                                                                         // prints to console and/or you can use the returned vector
    openNIkinect.addHandFocusGesture("Wave");
    
    // set properties for all user masks and point clouds
    openNIkinect.setUseMaskPixelsAllUsers(true); // if you just want pixels, use this set to true
    openNIkinect.setUseMaskTextureAllUsers(true); // this turns on mask pixels internally AND creates mask textures efficiently
//    openNIkinect.setPointCloudDrawSizeAllUsers(2); // size of each 'point' in the point cloud
    
    openNIkinect.setMaxNumHands(4);
//
//    for(int i = 0; i < openNIkinect.getMaxNumHands(); i++){
//        ofxOpenNIDepthThreshold depthThreshold = ofxOpenNIDepthThreshold(0, 0, false, true, true, true, true); 
//        // ofxOpenNIDepthThreshold is overloaded, has defaults and can take a lot of different parameters, eg:
//        // (ofxOpenNIROI OR) int _nearThreshold, int _farThreshold, bool _bUsePointCloud = false, bool _bUseMaskPixels = true, 
//        // bool _bUseMaskTexture = true, bool _bUseDepthPixels = false, bool _bUseDepthTexture = false, 
//        // int _pointCloudDrawSize = 2, int _pointCloudResolution = 2
//        openNIkinect.addDepthThreshold(depthThreshold);
//    }
    
    openNIkinect.start();
    
    verdana.load(ofToDataPath("Sharp.ttf"), 18);
    
    ofSetVerticalSync(true);
    ofBackground(234,234,234);
    ofSetLogLevel(OF_LOG_NOTICE);
    
    bMouseForce = false;
    
    box2d.init();
    box2d.enableEvents();   // <-- turn on the event listener
    box2d.setGravity(0, 10);
    box2d.createGround();
    box2d.createBounds();
    box2d.setFPS(60.0);
    
    box2d.registerGrabbing();
    
    // register the listener so that we get the events
    ofAddListener(box2d.contactStartEvents, this, &ofApp::contactStart);
    ofAddListener(box2d.contactEndEvents, this, &ofApp::contactEnd);
    
    for(int i=0; i <= 50 ; i++){
        shared_ptr<CustomParticle> p = shared_ptr<CustomParticle>(new CustomParticle);
        p.get()->setPhysics(1.0, 0.5, 0.3);
        p.get()->setup(box2d.getWorld(),ofRandom(0, 1024) , ofRandom(0, 960), ofRandom(30, 50));
        p.get()->setVelocity(ofRandom(-30, 30), ofRandom(-30, 30));
        p.get()->setupTheCustomData(verdana);
        particles.push_back(p);
    }

}

//--------------------------------------------------------------
void ofApp::update(){
    openNIkinect.update();
    
    if(openNIkinect.isNewFrame())
    {
        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(openNIkinect.getDepthPixels().getChannel(1));
        
        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        if(bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
            
        } else {
            
            // or we do it ourselves - show people how they can work with the pixels
            ofPixels & pix = grayImage.getPixels();
            long numPixels = pix.size();
            for(int i = 0; i < numPixels; i++) {
                if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                    pix[i] = 100;
                } else {
                    pix[i] = 200;
                }
            }
        }
        
        // update the cv images
        grayImage.flagImageChanged();
        contourFinder.findContours(grayImage, 10, (openNIkinect.getWidth()*openNIkinect.getHeight())/2, 20, false);
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourCircles.clear();
        // detected object (Blobs) the number of analysis
        for (int i = 0; i < contourFinder.nBlobs; i++){
            grayImage.drawBlobIntoMe(contourFinder.blobs[i], 0xc1cacc);

            for (int j = 0; j < contourFinder.blobs[i].pts.size(); j += 4) {
                // extracting coordinates at equal intervals along the contour line
                ofPoint pos = contourFinder.blobs[i].pts[j];
                // arranged in outline ofxBox2dCircle add
                ofPtr<ofxBox2dCircle> circle = ofPtr<ofxBox2dCircle>(new ofxBox2dCircle);
                circle.get()->setup(box2d.getWorld(), (pos.x*2), (pos.y*2), 4);
                // Vector array contourCircles added to the
                contourCircles.push_back(circle);
            }
        }
    }
    if(openNIkinect.CreateNewWord())
    {
        int createID = openNIkinect.getCreateUserID();
        for(int c=0; c<=15 ;c++)
        {
            shared_ptr<CustomParticle> p = shared_ptr<CustomParticle>(new CustomParticle);
            p.get()->setPhysics(1.0, 0.5, 0.3);
            p.get()->setup(box2d.getWorld(),ofRandom(0, 1024) , ofRandom(0, 780), ofRandom(30, 50));
            p.get()->setVelocity(ofRandom(-30, 30), ofRandom(-30, 30));
            p.get()->setupTheCustomData(verdana);
            particles.push_back(p);
        }
        openNIkinect.ifCreateNewWord(false);
    }
    
    box2d.update();
    for (int i = 0; i < openNIkinect.getNumTrackedUsers(); i++){
        float strength = 8.0f;
        float damping  = 0.7f;
        float minDis   = 100;
            
        ofxOpenNIUser user = openNIkinect.getTrackedUser(i);
        for(int b=(0+(15*i)); b<boxes.size(); b++) {
            boxes[b].get()->addAttractionPoint((user.getJoint(JOINT_TORSO).getProjectivePosition().x*2), (user.getJoint(JOINT_TORSO).getProjectivePosition().y*2), strength);
                boxes[b].get()->setDamping(damping, damping);
        }
        for(int c=(0+(15*i)); c<particles.size(); c++) {
            particles[c].get()->addAttractionPoint((user.getJoint(JOINT_TORSO).getProjectivePosition().x*2), (user.getJoint(JOINT_TORSO).getProjectivePosition().y*2), strength);
            particles[c].get()->setDamping(damping, damping);
        }
    
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(!mode){
        ofSetColor(255, 0, 0);
        ofDrawCircle(50,50, 10);
    }else{
        ofSetColor(0, 255, 0);
        ofDrawCircle(50, 50, 10);
    }

    ofPushMatrix();
    
//    ofTranslate(200, 50, - 100);
//    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
//    ofScale(1.5, 1.5);
    
    // iterate through users
    for (int i = 0; i < openNIkinect.getNumTrackedUsers(); i++){
        
        ofxOpenNIUser user = openNIkinect.getTrackedUser(i);
//        user.setMaskPixelFormat(OF_PIXELS_MONO);
//        user.drawMask();
        
        //grayImage.setFromPixels(user.getMaskPixels().getChannel(0));
        //grayImage.draw(0,0);
        //contourFinder.findContours(grayImage, 10, (openNIkinect.getWidth()*openNIkinect.getHeight())/2, 20, false);
        //contourFinder.draw(0, 0, 640, 480);
    }
    ofPopMatrix();
    
    ofPushMatrix();
    // get number of current hands
    int numHands = openNIkinect.getNumTrackedHands();
    
    // iterate through users
    for (int i = 0; i < numHands; i++){
        
        // get a reference to this user
        ofxOpenNIHand & hand = openNIkinect.getTrackedHand(i);
        
        // get hand position
        ofPoint & handPosition = hand.getPosition();
        // do something with the positions like:
        
        // draw a rect at the position (don't confuse this with the debug draw which shows circles!!)
        ofSetColor(255,0,0);
        ofRect(handPosition.x, handPosition.y, 10, 10);
        
    }
    ofPopMatrix();
    
    ofPushMatrix();
    ofScale(2,2);
    // draw debug (ie., image, depth, skeleton)
//    colorImg.draw(0, 0,1280, 960);
    
//    colorImg.setFromPixels(openNIkinect.getImagePixels());
//    colorImg.draw(0, 0,640, 480);

    grayImage.draw(0, 0,640, 480);
    
    contourFinder.draw(0, 0, 640, 480);
//    ofScale(1,1);
    openNIkinect.drawSkeletons(0, 0, 1280, 960);
    //    openNIkinect.getUseMaskTextureAllUsers();
    ofScale(1,1);
    ofPopMatrix();

    
    ofFill ();
    ofSetColor(0, 255, 0);
 
    for(int i=0; i<particles.size(); i++) {
        particles[i].get()->draw();
    }
    
    string info = "";
//    info += "Press [s] to draw a line strip ["+ofToString(bDrawLines)+"]\n";
//    info += "Press [f] to toggle Mouse Force ["+ofToString(bMouseForce)+"]\n";
//    info += "Press [c] for circles\n";
//    info += "Press [b] for blocks\n";
//    info += "Press [z] for custom particle\n";
    info += "Total Bodies: "+ofToString(box2d.getBodyCount())+"\n";
    info += "Total Joints: "+ofToString(box2d.getJointCount())+"\n\n";
    info += "FPS: "+ofToString(ofGetFrameRate())+"\n";
    ofSetHexColor(0x444342);
    verdana.drawString(info,30,30);

    
//	verdana.drawString(msg, 20, 480 - 20);
}

//--------------------------------------------------------------
void ofApp::contactStart(ofxBox2dContactArgs &e) {
    //    printf("%c",e.a->GetType());
    
    //    ofxBox2dCircle sd = e.a->GetShape();
    //    string a = e.a->GetUserData().id;
    
    if(e.a != NULL && e.b != NULL) {
        
        // if we collide with the ground we do not
        // want to play a sound. this is how you do that
        if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle) {
            Data * aData = (Data*)e.a->GetBody()->GetUserData();
            Data * bData = (Data*)e.b->GetBody()->GetUserData();
            
            if(aData) {
                aData -> bHit = true;
            }
            
            if(bData) {
                bData->bHit = true;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::contactEnd(ofxBox2dContactArgs &e) {
    if(e.a != NULL && e.b != NULL) {
        Data * aData = (Data*)e.a->GetBody()->GetUserData();
        Data * bData = (Data*)e.b->GetBody()->GetUserData();
        
        if(aData) {
            aData -> bHit = false;
        }
        
        if(bData) {
            bData->bHit = false;
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::handEvent(ofxOpenNIHandEvent & event){
    // show hand event messages in the console
    ofLogNotice() << getHandStatusAsString(event.handStatus) << "for hand" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void ofApp::exit(){
    openNIkinect.stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'b') {
        float w = ofRandom(10, 30);
        float h = ofRandom(10, 30);
        boxes.push_back(shared_ptr<ofxBox2dRect>(new ofxBox2dRect));
        boxes.back().get()->setPhysics(0.5, 0.53, 0.3);
        boxes.back().get()->setup(box2d.getWorld(), mouseX, mouseY, w, h);
        boxes.back().get()->draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

void ofApp::mousePressed(int x, int y, int button) {
    shared_ptr<CustomParticle> p = shared_ptr<CustomParticle>(new CustomParticle);
    p.get()->setPhysics(1.0, 0.5, 0.3);
    p.get()->setup(box2d.getWorld(), x, y, ofRandom(30, 50));
    p.get()->setVelocity(ofRandom(-30, 30), ofRandom(-30, 30));
    p.get()->setupTheCustomData(verdana);
    particles.push_back(p);
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
