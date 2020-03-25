#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	settings.loadFile("settings.xml");
	
	width = ofGetWidth();
    height = ofGetHeight();
	halfWidth = width / 2;
	halfHeight = height / 2;

	corner0 = 0;
	corner1 = 1;
	corner2 = 2;
	corner3 = 3;

	ofHideCursor();

    checkerboard.load("textures/checkerboard.png");
         
    fbo1.allocate(width, height, GL_RGBA);
	// a nested fbo may need to be flipped vertically
	// https://forum.openframeworks.cc/t/flipping-fbo-or-its-texture-upside-down/19021/3
	//fbo1.getTextureReference().getTextureData().bFlipTexture = true;
    
	fbo2.allocate(width, height, GL_RGBA);

    plane1.set(width, height);   // dimensions for width and height in pixels
    plane1.setPosition(width/2, height/2, 0); // position in x y z
    plane1.setOrientation(glm::vec3(180,0,0));
    plane1.setResolution(2, 2); // resolution as columns and rows
    plane1.mapTexCoordsFromTexture(fbo1.getTexture()); // *** don't forget this ***
    
	loadKeystoneSettings();

    keystoneStep = 10;
    keystoneIndex = 2; // upper left
    keystoneHandleColor = ofColor(255,0,0);
    keystoneHandleSize = 50;
    keystoneHandleStroke = 10;
    
    modeSelector = EDIT;
       
    // old oF default is 96 - but this results in fonts looking larger than in other programs.
    ofTrueTypeFont::setGlobalDpi(72);

    bgColor = ofColor(0);
    
    fontSize = settings.getValue("settings:font_size", fontSizeOrig);
    fontLineHeight = 68.0f;
    fontLetterSpacing = 1.035;
    fontSelector = settings.getValue("settings:font_selector", 0);
    fontSizeChangeIncrement = 4;

	fontsList.push_back("fonts/verdana.ttf");
	fontsList.push_back("fonts/vcr_osd_mono.ttf");
	fontsList.push_back("fonts/arcade.ttf");
    fontsList.push_back("fonts/arcade_classic.ttf");
    fontsList.push_back("fonts/stitch_warrior.ttf");
    
    initFonts();   
  
    fontLeftMargin = (float) width * settings.getValue("settings:font_left_margin", fontLeftMarginOrig);
    fontTopMargin = (float) height * settings.getValue("settings:font_top_margin", fontTopMarginOrig);
    
    fontColor = ofColor(255);
    bgColor = ofColor(0);    

	displayString = settings.getValue("settings:display_string", "...");
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
		ofVec2f center = ofVec2f(plane1.getMesh().getVertex(keystoneIndex).x + plane1.getPosition().x, fbo1.getHeight() - (plane1.getMesh().getVertex(keystoneIndex).y + plane1.getPosition().y));

		for (int i = 0; i < plane1.getMesh().getNumVertices(); i++) {
			ofVec2f v = getDistance(center, plane1.getMesh().getVertex(i));
			ofSetColor(0, 255, 0);
			ofLine(center.x, center.y, v.x + plane1.getPosition().x, fbo1.getHeight() - (v.y + plane1.getPosition().y));
	    }
		
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
		} else if (keyIsArrow(key)) {
			textStartPoint(key);
		} else if (key == OF_KEY_TAB || key == OF_KEY_PAGE_UP || key == OF_KEY_PAGE_DOWN || keyIsModifier(key)) { 
			// filter unwanted keys here
		} else {
			displayString.append(1, (char)key);
		}
    } else if (modeSelector == KEYSTONE) {
        if (key == '1') {
            keystoneIndex = corner2;
        } else if (key == '2') {
            keystoneIndex = corner3;
        } else if (key == '3') {
            keystoneIndex = corner0;
        } else if (key == '4') {
            keystoneIndex = corner1;
        } else if (keyIsArrow(key)) {
            keystoneVertex(keystoneIndex, key);
        } else if (!keyIsNumber(key) && key != OF_KEY_END) {
            modeSelector = EDIT;
        }
    }
}

void ofApp::keyReleased(int key) {
	if (key == OF_KEY_HOME) {
		saveSettings();
	} else if (key == OF_KEY_END) {
		init();
	} else if (keyIsAlt(key)) {
		modeSelector = KEYSTONE;
	}

	if (modeSelector == EDIT) {
		if (key == OF_KEY_PAGE_DOWN) {
			fontSize -= fontSizeChangeIncrement;
			if (fontSize < 10) fontSize = 10;
			refreshFont();
		} else if (key == OF_KEY_PAGE_UP) {
			fontSize += fontSizeChangeIncrement;
			refreshFont();
		} else if (key == OF_KEY_TAB) {
			fontSelector++;
			if (fontSelector > fonts.size() - 1) fontSelector = 0;
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
	ofVec3f v1 = plane1.getMesh().getVertex(index);

	for (int i = 0; i < plane1.getMesh().getNumVertices(); i++) {
		if (i != index) {
			ofVec3f v2 = plane1.getMesh().getVertex(i);

			plane1.getMesh().setVertex(i, getDistance(v1, v2));
		}
	}
}

ofVec3f ofApp::getDistance(ofVec3f v1, ofVec3f v2) {
	float s = (((float)width - v1.distance(v2)) / (float)width) * 0.001;
	float x = ofLerp(v2.x, v1.x, s);
	float y = ofLerp(v2.y, v1.y, s);
	return ofVec3f(x, y, 0);
}


void ofApp::saveKeystoneSettings() {
	ofVec3f v0 = plane1.getMesh().getVertex(corner0);
	ofVec3f v1 = plane1.getMesh().getVertex(corner1);
	ofVec3f v2 = plane1.getMesh().getVertex(corner2);
	ofVec3f v3 = plane1.getMesh().getVertex(corner3);

	float x0 = v0.x / (float)halfWidth;
	float y0 = v0.y / (float)halfHeight;
	float x1 = v1.x / (float)halfWidth;
	float y1 = v1.y / (float)halfHeight;
	float x2 = v2.x / (float)halfWidth;
	float y2 = v2.y / (float)halfHeight;
	float x3 = v3.x / (float)halfWidth;
	float y3 = v3.y / (float)halfHeight;

	settings.setValue("settings:key_x0", x0);
	settings.setValue("settings:key_y0", y0);
	settings.setValue("settings:key_x1", x1);
	settings.setValue("settings:key_y1", y1);
	settings.setValue("settings:key_x2", x2);
	settings.setValue("settings:key_y2", y2);
	settings.setValue("settings:key_x3", x3);
	settings.setValue("settings:key_y3", y3);
}

void ofApp::loadKeystoneSettings() {
	saveKeystoneVertsOrig();

	float x0 = (float) settings.getValue("settings:key_x0", -1.0) * (float) halfWidth;
	float y0 = (float) settings.getValue("settings:key_y0", -1.0) * (float) halfHeight;
	float x1 = (float) settings.getValue("settings:key_x1", 1.0) * (float) halfWidth;
	float y1 = (float) settings.getValue("settings:key_y1", -1.0) * (float) halfHeight;
	float x2 = (float) settings.getValue("settings:key_x2", -1.0) * (float) halfWidth;
	float y2 = (float) settings.getValue("settings:key_y2", 1.0) * (float) halfHeight;
	float x3 = (float) settings.getValue("settings:key_x3", 1.0) * (float) halfWidth;
	float y3 = (float) settings.getValue("settings:key_y3", 1.0) * (float) halfHeight;

	ofVec3f v0 = ofVec3f(x0, y0, 0);
	ofVec3f v1 = ofVec3f(x1, y1, 0);
	ofVec3f v2 = ofVec3f(x2, y2, 0);
	ofVec3f v3 = ofVec3f(x3, y3, 0);

	plane1.getMesh().setVertex(corner0, v0);
	plane1.getMesh().setVertex(corner1, v1);
	plane1.getMesh().setVertex(corner2, v2);
	plane1.getMesh().setVertex(corner3, v3);
}

void ofApp::saveKeystoneVertsOrig() {
    for (int i=0; i<plane1.getMesh().getVertices().size(); i++) {
        ofVec3f v = plane1.getMesh().getVertex(i);
        keystoneVertsOrig.push_back(v);
		cout << "v" << i << ": " << keystoneVertsOrig[i] << endl;
	}
}

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

void ofApp::refreshFont() {
	fonts[fontSelector].loadFont(fontsList[fontSelector], fontSize, true, true);
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

bool ofApp::keyIsControl(int key) {
	if (key == OF_KEY_CONTROL || key == OF_KEY_RIGHT_CONTROL || key == OF_KEY_LEFT_CONTROL) {
		return true;
	} else {
		return false;
	}
}

bool ofApp::keyIsCommand(int key) {
	if (key == OF_KEY_COMMAND || key == OF_KEY_RIGHT_COMMAND || key == OF_KEY_LEFT_COMMAND) {
		return true;
	} else {
		return false;
	}
}

bool ofApp::keyIsAlt(int key) {
	if (key == OF_KEY_ALT || key == OF_KEY_RIGHT_ALT || key == OF_KEY_LEFT_ALT) {
		return true;
	} else {
		return false;
	}
}

bool ofApp::keyIsShift(int key) {
	if (key == OF_KEY_SHIFT || key == OF_KEY_RIGHT_SHIFT || key == OF_KEY_LEFT_SHIFT) {
		return true;
	} else {
		return false;
	}
}

bool ofApp::keyIsModifier(int key) {
	if (keyIsControl(key) || keyIsCommand(key) || keyIsAlt(key) || keyIsShift(key)) {
		return true;
	} else {
		return false;
	}
}

void ofApp::saveSettings() {
	saveKeystoneSettings();
	settings.setValue("settings:font_left_margin", fontLeftMargin / (float) width);
	settings.setValue("settings:font_top_margin", fontTopMargin / (float) height);
	settings.setValue("settings:font_selector", fontSelector);
	settings.setValue("settings:font_size", fontSize);
	settings.setValue("settings:display_string", displayString);

	settings.saveFile("settings.xml");
}

void ofApp::init() {
	loadKeystoneVertsOrig();
	fontLeftMargin = fontLeftMarginOrig * (float)width;
	fontTopMargin = fontTopMarginOrig * (float)height;
	fontSelector = 0;
	fontSize = fontSizeOrig;
	initFonts();
}