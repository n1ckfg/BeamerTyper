#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp {
    
    public:
        int width, height, halfWidth, halfHeight;
    
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
		void refreshFont();

        int fontSelector;
    
        int fontSize;
        float fontLetterSpacing;
        float fontLineHeight;
        int fontLeftMargin;
        int fontTopMargin;
		float fontLeftMarginOrig = 0.05;
		float fontTopMarginOrig = 0.5;
		int fontSizeOrig = 72;
        int fontSizeChangeIncrement;
    
        // ~ ~ ~ ~ ~ ~ ~
  
        ofImage checkerboard;
        ofFbo fbo1, fbo2;
    
        ofPlanePrimitive plane1;
        int keystoneIndex;
        float keystoneStep;
        void keystoneVertex(int index, int key);
        vector<ofVec3f> keystoneVertsOrig;
		int corner0, corner1, corner2, corner3;
        int vertsPerSide;
        float warpForce = 0.001;
    
        void saveKeystoneVertsOrig();
        void loadKeystoneVertsOrig();
		void saveKeystoneSettings();
		void loadKeystoneSettings();

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

		void textStartPoint(int key);

		void saveSettings();

        ofVec3f warpGrid(ofVec3f v1, ofVec3f v2);
        void showWarp(ofVec3f v1, ofVec3f v2);

		void init();

};

