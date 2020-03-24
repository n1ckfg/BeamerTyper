#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    width = ofGetWidth();
    height = ofGetHeight();
    
    checkerboard.load("textures/checkerboard.png");
         
    fbo1.allocate(width, height, GL_RGBA);
    fbo2.allocate(width, height, GL_RGBA);

    plane1.set(width, height);   // dimensions for width and height in pixels
    plane1.setPosition(width/2, height/2, 0); // position in x y z
    plane1.setOrientation(glm::vec3(180,0,0));
    plane1.setResolution(2, 2); // this resolution (as columns and rows) is enough
    plane1.mapTexCoordsFromTexture(fbo1.getTexture()); // *** don't forget this ***
    
    saveKeystoneVertsOrig();
    keystoneStep = 10;
    keystoneIndex = 2; // upper left
    keystoneHandleColor = ofColor(255,0,0);
    keystoneHandleSize = 50;
    keystoneHandleStroke = 10;
    
    modeSelector = EDIT;
       
    // old oF default is 96 - but this results in fonts looking larger than in other programs.
    ofTrueTypeFont::setGlobalDpi(72);

    bgColor = ofColor(0);
    
    fontSize = 72;
    fontLineHeight = 68.0f;
    fontLetterSpacing = 1.035;
    fontSelector = 0;
    fontSizeChangeIncrement = 4;

	fontsList.push_back("fonts/verdana.ttf");
	fontsList.push_back("fonts/vcr_osd_mono.ttf");
	fontsList.push_back("fonts/arcade.ttf");
    fontsList.push_back("fonts/arcade_classic.ttf");
    fontsList.push_back("fonts/stitch_warrior.ttf");
    
    initFonts();   
  
    fontLeftMargin = 90;
    fontTopMargin = (height/2) + (fontSize/2);
    
    fontColor = ofColor(255);
    bgColor = ofColor(0);    
}

//--------------------------------------------------------------
void ofApp::update() {
    fbo1.begin();
    ofClear(255,255,255, 0);
    
    if (modeSelector == EDIT) {
        ofBackground(bgColor);
		ofSetColor(fontColor);
        fonts[fontSelector].drawString(displayString, fontLeftMargin, fontTopMargin + fontLineHeight);     
    } else if (modeSelector == KEYSTONE) {
        ofSetColor(255);
        checkerboard.draw(0,0,width,height);
    }
    ofSetColor(255); // why does this work?
    fbo1.end();

    fbo2.begin();
    ofClear(255,255,255, 0);

    fbo1.getTexture().bind();
    plane1.draw();
    fbo1.getTexture().unbind();

    if (modeSelector == KEYSTONE) {
        ofVec2f center = ofVec2f(plane1.getMesh().getVertex(keystoneIndex).x + plane1.getPosition().x, fbo2.getHeight() - (plane1.getMesh().getVertex(keystoneIndex).y + plane1.getPosition().y));
        ofPath circle;
        circle.setFillColor(ofColor(255,0,0));
        circle.arc(center, keystoneHandleSize + (keystoneHandleStroke/2), keystoneHandleSize + (keystoneHandleStroke/2), 0, 360);
        circle.close();
        circle.arc(center, keystoneHandleSize - (keystoneHandleStroke/2), keystoneHandleSize - (keystoneHandleStroke/2), 0, 360);
        circle.draw();
    }
    fbo2.end();
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0);
    fbo2.draw(0,0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) { 
    if (modeSelector == EDIT) {
        if (key == OF_KEY_DEL || key == OF_KEY_BACKSPACE) {
            if (displayString.length() > 0) {
                displayString = displayString.substr(0, displayString.length()-1);
            }
		} else if (key == OF_KEY_RETURN) {
			displayString = "";
		} else if (keyIsNumber(key)) {
			modeSelector = KEYSTONE;
		} else if (keyIsArrow(key)) {
			textStartPoint(key);
		} else {
			if (key != OF_KEY_LEFT_SHIFT && key != OF_KEY_RIGHT_SHIFT && key != OF_KEY_CONTROL && key != OF_KEY_ALT) { // filter unwanted keys here
				displayString.append(1, (char)key);
			}
		}
    } else if (modeSelector == KEYSTONE) {
        if (key == '1') {
            keystoneIndex = 2;
        } else if (key == '2') {
            keystoneIndex = 3;
        } else if (key == '3') {
            keystoneIndex = 0;
        } else if (key == '4') {
            keystoneIndex = 1;
        } else if (key == '5') {
            loadKeystoneVertsOrig();
        } else if (keyIsArrow(key)){
            keystoneVertex(keystoneIndex, key);
        } else {
            modeSelector = EDIT;
        }
     }
}

//--------------------------------------------------------------
void ofApp::textStartPoint(int key) {
	if (key == OF_KEY_UP) {
		fontTopMargin -= keystoneStep;
	} else if (key == OF_KEY_DOWN) {
		fontTopMargin += keystoneStep;
	} else if (key == OF_KEY_LEFT) {
		fontLeftMargin -= keystoneStep;
	} else if (key == OF_KEY_RIGHT) {
		fontLeftMargin += keystoneStep;
	}
}

void ofApp::keystoneVertex(int index, int key) {
    ofVec3f v = plane1.getMesh().getVertex(index);
    
    if (key == OF_KEY_UP) {
        v.y += keystoneStep;
    } else if (key == OF_KEY_DOWN) {
        v.y -= keystoneStep;
    } else if (key == OF_KEY_LEFT) {
        v.x -= keystoneStep;
    } else if (key == OF_KEY_RIGHT) {
        v.x += keystoneStep;
    }
    
    plane1.getMesh().setVertex(index, v);
}

void ofApp::saveKeystoneVertsOrig() {
    for (int i=0; i<plane1.getMesh().getVertices().size(); i++) {
        ofVec3f v = plane1.getMesh().getVertex(i);
        keystoneVertsOrig.push_back(v);
    }
}

//--------------------------------------------------------------
void ofApp::loadKeystoneVertsOrig() {
    for (int i=0; i<keystoneVertsOrig.size(); i++) {
        plane1.getMesh().setVertex(i, keystoneVertsOrig[i]);
    }
}

void ofApp::initFonts() {
   fonts.clear();
    for (int i=0; i<fontsList.size(); i++) {
        ofTrueTypeFont font;
        font.loadFont(fontsList[i], fontSize, true, true);
        fonts.push_back(font);
        fonts[i].setLineHeight(fontLineHeight);
        fonts[i].setLetterSpacing(fontLetterSpacing);
    }
}

//--------------------------------------------------------------

bool ofApp::keyIsArrow(int key) {
    if (key == OF_KEY_UP || key == OF_KEY_DOWN || key == OF_KEY_LEFT || key == OF_KEY_RIGHT) {
        return true;
    } else {
        return false;
    }
}

bool ofApp::keyIsNumber(int key) {
    if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9') {
        return true;
    } else {
        return false;
    }
}
