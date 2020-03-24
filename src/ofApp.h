#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp {
    
    public:
        int width;
        int height;
    
        void setup();
        void update();
        void draw();       
		void keyPressed(int key);
		void keyReleased(int key);

		string displayString;
		ofxXmlSettings settings;

        enum ModeSelector { KEYSTONE, EDIT };
        ModeSelector modeSelector;
       
        ofColor bgColor = ofColor(0);
		ofColor fontColor = ofColor(255);

        vector<string> fontsList;
        vector<ofTrueTypeFont> fonts;
		void initFonts();
		void refreshFonts();

        int fontSelector;
    
        int fontSize;
        float fontLetterSpacing;
        float fontLineHeight;
        int fontLeftMargin;
        int fontTopMargin;
        int fontSizeChangeIncrement;
    
        // ~ ~ ~ ~ ~ ~ ~
  
        ofImage checkerboard;
        ofFbo fbo1, fbo2;
    
        ofPlanePrimitive plane1;
        int keystoneIndex;
        float keystoneStep;
        void keystoneVertex(int index, int key);
        vector<ofVec3f> keystoneVertsOrig;
        void saveKeystoneVertsOrig();
        void loadKeystoneVertsOrig();
        int keystoneHandleSize;
        int keystoneHandleStroke;
        ofColor keystoneHandleColor;

        bool keyIsArrow(int key);
        bool keyIsNumber(int key);
		bool keyIsControl(int key);
		bool keyIsCommand(int key);
		bool keyIsAlt(int key);
		bool keyIsShift(int key);
		bool keyIsModifier(int key);
		bool keyIsControlOrCommand(int key);

		bool ctrlIsPressed = false;

		void textStartPoint(int key);
};

