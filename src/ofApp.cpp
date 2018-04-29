// http://www.translatorscafe.com/cafe/units-converter/numbers/calculator/octal-to-decimal/


#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

int octaveScaleFactor[NOTE_SIZE] = {60, 86, 36, 48, 72, 36, 36};


//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    ofEnableAntiAliasing();
    ofSetCircleResolution(16);
    
    
    printText.load("vera.ttf", 28);
    uiText.load("vera.ttf", 18);
    
//    printer.open("/dev/cu.usbserial");
    printer.open("/dev/cu.usbmodem1421");

    string _date = ofGetTimestampString();
    printer.println("------------------------");
    printer.println("------------------------");
    printer.println(_date);
    printer.println("Nokta Festival, Opole Glowne");
    printer.println("------------------------");
    printer.println(" ");

    gclef.load("GClef.png");
    fclef.load("FClef.png");
    
    ctrl12.load("1-2.png");
    
    baseSelection = 7;
    
    ofBackground(255);
    ofSetFrameRate( 60 );
    ofEnableAlphaBlending();
    
    
    guiSetting();
    
    
    cam.setDeviceID(1);
    cam.setup( 800, 600 );
    cam.setDesiredFrameRate(30);
    screenW = 600;
    screenH = 600;
    
    camColorCV.allocate(cam.getWidth(), cam.getHeight());
    downScaleCam.allocate(cam.getWidth(), cam.getHeight());
    centerCam.allocate(screenW, screenH);
    cannyInverted.allocate(screenW, screenH, OF_PIXELS_GRAY);
    
    
    float _sizeF = screenW;
    ctrlPnX = 0;
    ctrlPnY = screenW;
    ctrlPnW = screenW;
    ctrlPnH = screenH - ctrlPnY;
    
    
    bufferImg.allocate(screenW, screenW, OF_IMAGE_GRAYSCALE);
    
    
    float _widthDefault = ofGetWidth();
    pixelCircleSize = 10 / _widthDefault * _sizeF;
    ctrlRectS = 80 / _widthDefault * _sizeF;
    guideWidthStep = 96 / _widthDefault * _sizeF;
    guideHeightStep = 64 / _widthDefault * _sizeF;
    lineScoreStepX = 51 / _widthDefault * _sizeF;
    lineScoreStepY = 5 / _widthDefault * _sizeF;
    pixeShapeSize = 1 / _widthDefault * _sizeF;
    
    screenPos = ofVec2f( (ofGetWidth()-screenW) * 0.5, 0 );
    
    synthSetting();
    maxSpeed = 180;
    minSpeed = 30;
    bpm = synthMain.addParameter("tempo", 100).min(minSpeed).max(maxSpeed);
    metro = ControlMetro().bpm(4 * bpm);
    metroOut = synthMain.createOFEvent(metro);
    
    synthMain.setOutputGen(synth1 + synth2 + synth3 + synth4 + synth5 + synth6 + synth7);
    
    
    pixelStepS = 5;
    camSize = screenW;
    changedCamSize = camSize / pixelStepS;  // 90
    thresholdValue = 80;
    
    cameraScreenRatio = 1;
    
    index = -1;
    noteIndex = -1;
    
    
    float _posIndexLeft = (ofGetWidth() - screenW) * 0.25;
    float _posIndexRight = (ofGetWidth() + screenW) * 0.5 + (ofGetWidth() - screenW) * 0.25;
    float _baseCtrlPosY = 0;
    float _stepBasePos = 105 / _widthDefault * _sizeF;
    
    speedLineYMin = 350 - 20;
    speedLineYMax = 450 + 20;
    intervalLineYMin = 50 - 20;
    intervalLineYMax = 150 + 20;
    
    
    controlPos = ofPoint( _posIndexRight, 0 );
    
    speedCSize = ctrlRectS;
    speedCPos = ofPoint( controlPos.x, (speedLineYMin + speedLineYMax) * 0.5 );
    bSpeedCtrl = false;
    
    thresholdCSize = ctrlRectS * 0.5;
    thresholdCPos = ofPoint( controlPos.x, ctrlPnY + ctrlPnH * 0.5 );
    bthresholdCtrl = false;
    
    intervalSize = ctrlRectS * 0.5;
    intervalPos = ofPoint( controlPos.x, (intervalLineYMin + intervalLineYMax) * 0.5 );
    bthresholdCtrl = false;
    intervalDist = 1;
    
    cannyThreshold1 = 120;
    cannyThreshold2 = 120;
    grayThresholdTouch = 120;
    
    base4Pos = ofPoint( _posIndexLeft, _baseCtrlPosY + _stepBasePos * 1 );
    base5Pos = ofPoint( _posIndexLeft, _baseCtrlPosY + _stepBasePos * 2.5 );
    base6Pos = ofPoint( _posIndexLeft, _baseCtrlPosY + _stepBasePos * 4 );
    base7Pos = ofPoint( _posIndexLeft, _baseCtrlPosY + _stepBasePos * 5.5 );
    base8Pos = ofPoint( _posIndexLeft, _baseCtrlPosY + _stepBasePos * 7 );
    base9Pos = ofPoint( _posIndexLeft, _baseCtrlPosY + _stepBasePos * 8.5 );
    baseSize = ctrlRectS * 0.55;
    
    bCameraCapturePlay = false;
    
    scaleSetting.setup();
    
    lineScoreNumber = 23;
    
    allPlayOnOff = false;
    
    
    faceFind.setup("haarcascade_frontalface_default.xml");
    faceFind.setPreset(ObjectFinder::Fast);
    faceFind.setFindBiggestObject(true);
    
    debugView = false;
    
    
    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);
    
    
    printScoreFbo.allocate(384, ofGetWidth() * 3);
    
    
    baseNum.addListener(this, &ofApp::changedBaseNum);
    bChangedBaseNum = false;
    
    
    printButton.set((ofGetWidth() + screenW) * 0.5, 500, 212, 100);
    speedArea.set((ofGetWidth() + screenW) * 0.5, 0, 212, 200);
    intervalArea.set((ofGetWidth() + screenW) * 0.5, 300, 212, 200);
    playStopArea.set((ofGetWidth() + screenW) * 0.5, 200, 212, 100);
    
    
    ofHideCursor();
    
    printAll = false;
    printHeaderOnOff = false;
    printFooterOnOff = false;
    printImgOnOff = false;

    
    ctrl12OnOff = false;
    
    
}



//--------------------------------------------------------------
void ofApp::update(){
    

    cam.update();
    
    if(cam.isFrameNew()) {
        
        camColorCV.setFromPixels(cam.getPixels().getData(), cam.getWidth(), cam.getHeight());
        camColorCV.setROI(150, 0, 600, 600);
        camColorCV.mirror(false, true);
        centerCam.setFromPixels(camColorCV.getRoiPixels());
        
        
        faceFind.update(centerCam);
        
        float _faceWidth;
        float _faceHeight;
        float _facesize;
        
        float _sizeAdd = 20;
        if (faceFind.size()>0) {
            faceCenter = faceFind.getObject(0).getCenter();
            _faceWidth = faceFind.getObject(0).getWidth() + _sizeAdd;
            _faceHeight = faceFind.getObject(0).getHeight() + _sizeAdd;
            _facesize = max(_faceWidth, _faceHeight) * 0.5;
        } else {
            faceCenter = ofVec2f(300, 300);
            _faceWidth = 600;
            _faceHeight = 600;
            _facesize = 600;
        }
        
        
        convertColor(centerCam, gray, CV_RGB2GRAY);
        threshold(gray, gray, grayThresholdTouch);
        //                erode(gray);
        Canny(gray, edge, cannyThreshold1, cannyThreshold2, 3);
        
        //        thin(edge);
        invert(edge);
        
        edge.update();
        
        
        if ( bCameraCapturePlay ) {
            noteIndex = index;
        } else {
            
            //            convertColor(centerCam, downGray, CV_RGB2GRAY);
            //            threshold(downGray, downGray, thresholdGui);
            downGray = gray;
            Canny(downGray, downScaleEdge, cannyThreshold1, cannyThreshold2, 3);
            downScaleEdge.resize(150, 150);
            invert(downScaleEdge);
            downScaleEdge.update();
            
            //            printCam = downScaleEdge;
            //            printCam.resize(384, 384);
            
            
            unsigned char * _src = edge.getPixels().getData();
            
            noteIndex = 0;
            ofImage _tImage;
            
            pixelBright.clear();
            whitePixels.clear();
            blackPixels.clear();
            
            
            for (int i=0; i<screenW; i+=pixelStepS) {
                for (int j=0; j<screenW; j+=pixelStepS) {
                    
                    int _index = j + i * screenW;
                    float _brightness = _src[_index];
                    
                    float _distX = ofDist(faceCenter.x, faceCenter.y, j, faceCenter.y);
                    float _distY = ofDist(faceCenter.x, faceCenter.y, faceCenter.x, i);
                    
                    if ((_distX<_facesize*0.75)&&(_distY<_facesize*1.25)) {
                        pixelBright.push_back(_brightness);
                    } else {
                        pixelBright.push_back(255);
                    }
                    
                }
            }
            
            
            int _wCounter = 0;
            int _bCounter = 0;
            
            for (int i=0; i<pixelBright.size(); i++) {
                
                int _whitePixel = 255;
                
                if ( pixelBright[i] == _whitePixel ) {
                    
                    if ( _bCounter == 0 ) {
                        blackWhitePixels _bWP;
                        _bWP.indexPos = i;
                        _bWP.pixelN = _wCounter;
                        blackPixels.push_back(_bWP);
                    }
                    
                    _bCounter++;
                    _wCounter = 0;
                    
                } else {
                    
                    if ( _wCounter == 0 ) {
                        blackWhitePixels _bWP;
                        _bWP.indexPos = i;
                        _bWP.pixelN = _bCounter;
                        whitePixels.push_back(_bWP);
                    }
                    
                    _wCounter++;
                    _bCounter = 0;
                }
                
            }
            
            
            //            if (whitePixels.size()%8 !=0) {
            //                int _restNum = whitePixels.size() % 8;
            //                for (int j=0; j<=(8-_restNum); j++) {
            //                    blackWhitePixels _bWP;
            //                    _bWP.indexPos = j;
            //                    _bWP.pixelN = _bCounter;
            //                    whitePixels.push_back(_bWP);
            //                }
            //            }
            
        }
    }
    
    
    // FBO
    printScoreFbo.begin();
    ofClear(255, 255);
    drawPrintScoreFBO();
    printScoreFbo.end();
    
    
    frameRate = ofToString(ofGetFrameRate(),2);
    if (faceFind.size()>0) {
        faceNum = ofToString(faceFind.size());
    }
    if (blackPixels.size()>0) {
        noteNum = ofToString(blackPixels.size());
    }
    
    
    if (bChangedBaseNum) {
//        scoreMake();
//        printScoreMake();
        bChangedBaseNum = false;
    }
    
 
    // MARK: Printer
    if (bCameraCapturePlay) {
        if (!printer.isThreadRunning() && printAll) {
            string _date = ofGetTimestampString();
            printer.println("------------------------");
            printer.println("------------------------");
            printer.println(_date);
            printer.println("------------------------");
            printer.println(" ");
            printCamView();
            printScore();
            printAll = false;
            printHeaderOnOff = true;
            printFooterOnOff = true;
        }
        
        if (!printer.isThreadRunning() && printFooterOnOff) {
            printFooter();
            printFooterOnOff = false;
            bCameraCapturePlay = false;
            mainCaptureOff();
        }
    } else {
        
    }
    

    
}




//--------------------------------------------------------------
void ofApp::triggerReceive(float & metro){
    
    index++;
    noteIndex = index;
    
    vector< vector<int> > _list = scoreMakeOutput();
    
    trigScoreNote( _list[0], synth1, 1 );
    trigScoreNote( _list[1], synth2, 2 );
    trigScoreNote( _list[2], synth3, 3 );
    trigScoreNote( _list[3], synth4, 4 );
    trigScoreNote( _list[4], synth5, 5 );
    trigScoreNote( _list[5], synth6, 6 );
    trigScoreNote( _list[6], synth7, 7 );
    
}




//--------------------------------------------------------------
void ofApp::draw(){
    

    ofPushMatrix();
    
    float _stepLine = 15;
    float _xSizeFactor = _stepLine * 8;
    
    vector< vector<int> > _melody = melodyOutput();
    int _melodyNoteNum;
    
    if (_melody.size()>0) {
        _melodyNoteNum = _melody[0].size();
    }
    
    if (_melodyNoteNum>0) {
        int _index = noteIndex % _melodyNoteNum;
        float _x1 = ofMap(_index, 0, _melodyNoteNum, -ofGetWidth()*0.5,  -ofGetWidth());
        float _y1 = 0;
        //        ofDrawLine(_x1, _y1 + 200, _x1, _y1 - 200);
        
        ofTranslate( _x1, ofGetHeight());
        ofRotateZDeg(-90);
        printScoreFbo.draw(0, ofGetHeight() - 512 * 0.5, 384 * 0.5, ofGetWidth() * 1.5);
    }
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate((ofGetWidth() - screenW) * 0.5, 0);
    
    ofPushMatrix();
    
    if (!bCameraCapturePlay) {
        if (edge.isAllocated()) {
            ofPushStyle();
            ofSetColor( 255, 255 );
            edge.draw( 0, 0, screenW, screenH);
            ofPopStyle();
        }
    }
    
    if (bCameraCapturePlay) {
        ofPushStyle();
        ofSetColor( 255, 255 );
        ofDrawRectangle(0, 0, screenW, screenH);
        
        ofSetColor( 255, 180 );
        bufferImg.draw( 0, 0, screenW, screenH);
        ofPopStyle();
    }
    
    ofPopMatrix();
    
    
    //    ofPushStyle();
    //    ofSetColor(255,230);
    //    ofDrawRectangle(0, 0, screenW, screenH);
    //    ofPopStyle();
    
    
    ofPushStyle();
    if (bCameraCapturePlay) {
        ofSetColor( 0, 60 );
    } else {
        ofSetColor( 0, 160 );
    }
    drawTrianglePixel();
    ofPopStyle();
    
    
    if (bCameraCapturePlay) {
        drawPixelNumbersCircleNotes();
        //                drawPlayingShapeNotes();
        //                drawPixelAllNoteShape();
        
//        for (int i=0; i<noteLists.size(); i++) {
//            drawPixelAllNoteShapes( noteLists[i].noteArray, i );
//        }
//        
//        for (int i=0; i<noteLists.size(); i++) {
//            drawPlayingShapeNote( noteLists[i].noteArray, i );
//        }

        vector< vector<int> > _list = scoreMakeOutput();
        for (int i=0; i<_list.size(); i++) {
            drawPixelAllNoteShapes( _list[i], i );
            drawPlayingShapeNote( _list[i], i );
        }

    }
    
    //    ofPushStyle();
    //    ofRectMode(OF_RECTMODE_CENTER);
    //    ofNoFill();
    //    ofSetColor(0,255,0);
    //    ofDrawRectangle(faceCenter.x, faceCenter.y, 30, 30);
    //    ofPopStyle();
    
    ofPopMatrix();
    
    
    debugInformation();
    
    if (debugView ) {
        gui.draw();
    }
    
    drawBaseInterface();
    drawControlElement();
    //    drawLineScore();
    layoutLines();
    
    
    ofPushMatrix();
    ofPushStyle();
    ofSetColor(120, 70);
    ofSetLineWidth(4);
    ofDrawLine(mouseX, 0, mouseX, ofGetHeight());
    ofDrawLine(0, mouseY, ofGetWidth(), mouseY);
    ofPopStyle();
    ofPopMatrix();
    
    
    // MARK: Interface
    if (bCameraCapturePlay) {
        ofPushStyle();
        ofSetColor(ofColor::skyBlue);
        printText.drawString("PRINT", printButton.x + 48, printButton.y + 64);
        ofPopStyle();
        
        ofPushStyle();
        ofSetColor(ofColor::indianRed);
        printText.drawString("STOP", playStopArea.x + 56, playStopArea.y + 64);
        ofPopStyle();

        ofPushStyle();
        ofSetColor(ofColor::cadetBlue);
        uiText.drawString("Interval", playStopArea.x + 62, 200 - 7);
        ofPopStyle();

        ofPushStyle();
        ofSetColor(ofColor::cadetBlue);
        uiText.drawString("Tempo", playStopArea.x + 62, 500 - 7);
        ofPopStyle();

        ofPushStyle();
        ofSetColor(ofColor::cadetBlue);
        uiText.drawString("Scale", 0 + 72, 600 - 11);
        ofPopStyle();

        ctrl12OnOff = false;
        
    } else {
        ctrl12OnOff = true;
        
        ofPushStyle();
        ofSetColor(255, 200);
        ofDrawRectangle(0, 0, 212, 600);
        ofDrawRectangle(812, 0, 212, 600);
        ofDrawRectangle(0, 600, 1024, 168);
        ofPopStyle();

    }


    if ( ctrl12OnOff ) {
        
        ofPushStyle();
        ofSetColor(255, 120);
        ofDrawRectangle(212, 0, 600, 600);
        ofPopStyle();
        ctrl12.draw(212, 0);

    }

    
}



//--------------------------------------------------------------
void ofApp::layoutLines(){
    
    ofPushMatrix();
    ofPushStyle();
    ofSetColor(ofColor::peru);
    
    ofDrawLine((ofGetWidth() - screenW) * 0.5, 0, (ofGetWidth() - screenW) * 0.5, screenH);
    ofDrawLine((ofGetWidth() + screenW) * 0.5, 0, (ofGetWidth() + screenW) * 0.5, screenH);
    
    ofDrawLine(0, screenH, ofGetWidth(), screenH);
    
    ofPopStyle();
    ofPopMatrix();
}



//--------------------------------------------------------------
void ofApp::drawPrintScoreFBO(){
    
    
    ofPushMatrix();
    ofTranslate(ofGetHeight() * 0.5 - 0, 0);
    
    ofSetColor(0);
    ofDrawRectangle(0, 0, printScoreFbo.getWidth(), printScoreFbo.getHeight());
    
    ofSetColor(255);
    ofDrawRectangle(10, 10, printScoreFbo.getWidth()-20, printScoreFbo.getHeight()-20);
    
    ofRotateZDeg(90);
    
    
    ofRectMode(OF_RECT_CENTER);
    
    float _stepLine = 15;
    float _noteSize = 5;
    
    float _noteExtraLineSize = 10;
    
    float _downBaseLine = 320;
    float _upBaseLine = _downBaseLine - _stepLine * 7;
    float _xSizeFactor = _stepLine * 8;
    
    vector< vector<int> > _melody = melodyOutput();
    int _melodyNoteNum;
    
    if (_melody.size()>0) {
        _melodyNoteNum = _melody[0].size();
    }
    
    float _scoreXStart = 0;
    float _scoreXEnd = ofGetWidth() * 3 - _xSizeFactor - (ofGetWidth() * 3 - 20.0) / _melodyNoteNum * 0.5;
    
    ofPushMatrix();
    ofTranslate( (ofGetWidth() * 3 - (_scoreXEnd - _scoreXStart)) * 0.5, 0 );
    
    
    ofPushMatrix();
    
    ofPushStyle();
    ofSetColor(0, 255);
    
    ofPushMatrix();
    ofTranslate(0, _upBaseLine);
    for (int i=0; i<5; i++) {
        ofDrawLine(0, -i * _stepLine, _scoreXEnd, -i * _stepLine);
        gclef.draw(0, -_stepLine * 5, _stepLine * 2.3, _stepLine * 6.1);
    }
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(0, _downBaseLine);
    for (int i=0; i<5; i++) {
        ofDrawLine(0, -i * _stepLine, _scoreXEnd, -i * _stepLine);
        fclef.draw(0, -_stepLine * 2.7, _stepLine * 2.7, _stepLine * 2.7);
    }
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(0, _downBaseLine);
    for (int j=1; j<=_melodyNoteNum; j++) {
        float _xStep = (ofGetWidth() * 3 - 20.0) / _melodyNoteNum;
        if (j % 8 == 0) {
            float _x1 = ofMap(j, 0, _melodyNoteNum, _xSizeFactor, ofGetWidth() * 3 - _xSizeFactor);
            ofDrawLine(_x1 - _xStep * 0.5, 0, _x1 - _xStep * 0.5, -_stepLine * 11);
        }
        
    }
    ofPopMatrix();
    
    
    ofPopStyle();
    
    ofPopMatrix();
    
    
    ofPushMatrix();
    
    ofPushStyle();
    ofTranslate(0, _upBaseLine);
    ofSetLineWidth(4);
    ofSetColor(255, 0, 0, 180);
    if (_melodyNoteNum>0) {
        int _index = noteIndex % _melodyNoteNum;
        float _x1 = ofMap(_index, 0, _melodyNoteNum, _xSizeFactor, ofGetWidth() * 3 - _xSizeFactor);
        float _y1 = 0;
        ofDrawLine(_x1, 600, _x1, 600 - ofGetHeight());
    }
    ofPopStyle();
    
    
    ofPushStyle();
    ofSetColor(0, 255);
    
    float _stempLength = 37;
    float _teilWidth = 3;
    float _teilLength = 15;
    float _teilEndX = _teilLength * 0.8;
    
    
    for (int i=0; i<_melody.size(); i++) {
        
        for (int j=0; j<_melody[i].size(); j++) {
            
            float _x1 = ofMap(j, 0, _melody[i].size(), _xSizeFactor, ofGetWidth() * 3 -_xSizeFactor);
            
            if (_melody[i][j]>0) {
                
                //TODO: Fix Score
                if (i%3==0 || i%3==1) {
                    int _note = _melody[i][j] % 12;
                    int _offsetOctave = octaveScaleFactor[i];
                    int _noteOctave = (_melody[i][j] - _offsetOctave) / 12;
                    
                    float _posY = notePosition(_note, _stepLine);
                    float _yOutput = _posY - _noteOctave * _stepLine * 3.5;
                    
                    if (_yOutput == _stepLine) {
                        ofDrawLine(_x1-_noteExtraLineSize, _yOutput, _x1+_noteExtraLineSize, _yOutput);
                    }
                    
                    ofDrawCircle(_x1, _yOutput, _noteSize);
                    
                    float _offsetLine = 4;
                    ofSetLineWidth(1);
                    ofDrawLine(_x1+_offsetLine, _yOutput, _x1+_offsetLine, _yOutput - _stempLength);
                    
                    ofSetLineWidth(_teilWidth);
                    ofDrawLine(_x1+_offsetLine, _yOutput-_stempLength, _x1+_teilEndX, _yOutput - _teilLength);
                    
                } else {
                    
                    int _note = _melody[i][j] % 12;
                    int _offsetOctave = octaveScaleFactor[i];
                    int _noteOctave = (_melody[i][j] - _offsetOctave) / 12 - 2;
                    
                    float _posY = notePosition(_note, _stepLine);
                    float _yOutput = _posY - _noteOctave * _stepLine * 3.5;
                    
                    ofDrawCircle(_x1, _yOutput, _noteSize);
                    
                    ofSetLineWidth(1);
                    ofDrawLine(_x1+2, _yOutput, _x1+2, _yOutput - _stempLength);
                    
                    ofSetLineWidth(_teilWidth);
                    ofDrawLine(_x1+2, _yOutput-_stempLength, _x1+_teilEndX, _yOutput - _teilLength);
                    
                }
                
            }
            
        }
    }
    
    
    ofPopStyle();
    
    ofPopMatrix();
    
    ofPopMatrix();
    
    ofPopMatrix();
    
    
}


//--------------------------------------------------------------
void ofApp::drawScoreBaseElement(){
    
}



//--------------------------------------------------------------
int ofApp::notePosition(int _note, int _stepLine){
    
    int _y1;
    
    switch (_note) {
        case 0:
            _y1 = _stepLine;
            break;
            
        case 1:
            _y1 = _stepLine;
            break;
            
        case 2:
            _y1 = _stepLine * 0.5;
            break;
            
        case 3:
            _y1 = _stepLine * 0.5;
            break;
            
        case 4:
            _y1 = 0;
            break;
            
        case 5:
            _y1 = -_stepLine * 0.5;
            break;
            
        case 6:
            _y1 = -_stepLine * 1;
            break;
            
        case 7:
            _y1 = -_stepLine * 1;
            break;
            
        case 8:
            _y1 = -_stepLine * 1.5;
            break;
            
        case 9:
            _y1 = -_stepLine * 1.5;
            break;
            
        case 10:
            _y1 = -_stepLine * 2;
            break;
            
        case 11:
            _y1 = -_stepLine * 2;
            break;
            
            
        default:
            break;
    }
    
    
    return _y1;
    
}



//--------------------------------------------------------------
void ofApp::printScoreMake(){
    
//    vector<noteList> _noteList = scoreMakeOutput();
//
//    for (int i=0; i<melodies.size(); i++) {
//        melodies[i].melodyLine.clear();
//    }
//    
//    
//    for (int j=0; j<_noteList.size(); j++) {
//        
//        for (int i=1; i<_noteList[j].noteArray.size(); i++) {
//            
//            int _note = _noteList[j].noteArray[i];
//            int _noteOld = _noteList[j].noteArray[i-1];
//            
//            int _outputNote;
//            if ( abs(_noteOld - _note) >= intervalDist ) {
//                
//                if (_note>0) {
//                    _outputNote = scaleSetting.noteSelector(baseSelection, j+1, _note);
//                    melodies[j].melodyLine.push_back(_outputNote);
//                } else {
//                    melodies[j].melodyLine.push_back(0);
//                }
//                
//            } else {
//                melodies[j].melodyLine.push_back(0);
//            }
//            
//        }
//        
//    }
//    
    
    
}



//--------------------------------------------------------------
vector< vector<int> > ofApp::melodyOutput(){
    
    vector< vector<int> > _melody;
    
    vector< vector<int> > _noteList = scoreMakeOutput();
    
    _melody.resize(NOTE_SIZE);
    
//    for (int i=0; i<melodies.size(); i++) {
//        melodies[i].melodyLine.clear();
//    }
    
    
    for (int j=0; j<_noteList.size(); j++) {
        
        for (int i=1; i<_noteList[j].size(); i++) {
            
            int _note = _noteList[j][i];
            int _noteOld = _noteList[j][i-1];
            
            int _outputNote;
            if ( abs(_noteOld - _note) >= intervalDist ) {
                
                if (_note>0) {
                    _outputNote = scaleSetting.noteSelector(baseSelection, j+1, _note);
                    _melody[j].push_back(_outputNote);
                } else {
                    _melody[j].push_back(0);
                }
                
            } else {
                _melody[j].push_back(0);
            }
            
        }
        
    }
    
    return _melody;
    
}




//--------------------------------------------------------------
void ofApp::debugInformation(){
    
    if (debugView ) {
        
        ofPushMatrix();
        ofPushStyle();
        
        ofSetColor(255);
        printCam.draw(ofGetWidth()-175, 10, 150, 150);
        
        ofSetColor(255, 20);
        centerCam.draw(ofGetWidth()-175, 10, 150, 150);
        
        ofPopStyle();
        ofPopMatrix();
        
    }
    
    
}


//--------------------------------------------------------------
void ofApp::controlBackground(){
    
    ofPushMatrix();
    
    ofPushStyle();
    ofSetColor(ofColor::azure);
    ofDrawRectangle(printButton);
    ofPopStyle();
    
    ofPushStyle();
    ofSetColor(ofColor::honeyDew);
    ofDrawRectangle(speedArea);
    ofPopStyle();
    
    ofPushStyle();
    ofSetColor(ofColor::navajoWhite);
    ofDrawRectangle(intervalArea);
    ofPopStyle();
    
    ofPopMatrix();
    
}



//--------------------------------------------------------------
void ofApp::drawControlElement(){
    
    controlBackground();
    
    
    ofPushMatrix();
    //    ofTranslate((ofGetWidth() - 600) * 0.5, 0);
    
    ofPushStyle();
    ofSetColor( 255 );
    ofDrawRectangle( 0, ctrlPnY, ctrlPnW, ctrlPnH );
    ofSetColor( 0, 10 );
    ofPopStyle();
    
    
    ofPushMatrix();
    ofPushStyle();
    
    ofSetColor( 0, 80 );
    
    float _speedX = controlPos.x;
    float _yD = 20;
    float _thresholdX = controlPos.x;
    
    ofDrawLine( _speedX, speedLineYMin, _speedX, speedLineYMax);
    //    ofDrawLine( _thresholdX, ctrlPnY + _yD, _thresholdX, screenH - _yD);
    
    float _intervalX = controlPos.x;
    ofDrawLine( _intervalX, intervalLineYMin, _intervalX, intervalLineYMax);
    
    ofPopStyle();
    ofPopMatrix();
    
    int _alpha = 255;
    
    ofPushStyle();
    ofSetColor( 0, _alpha );
    ofSetCircleResolution(48);
    float _sX = speedCPos.x;
    float _sY = speedCPos.y;
    ofNoFill();
    ofDrawCircle( _sX, _sY, speedCSize * 0.5 );
    ofPopStyle();
    
    
    //    ofPushStyle();
    //    ofSetColor( 255, _alpha );
    //    ofNoFill();
    //    float _sizeF = 1.1;
    //    ofPoint _p1 = ofPoint(thresholdCPos.x, thresholdCPos.y - thresholdCSize * _sizeF);
    //    ofPoint _p2 = ofPoint(thresholdCPos.x - cos(ofDegToRad(30)) * thresholdCSize * _sizeF, thresholdCPos.y + sin(ofDegToRad(30)) * thresholdCSize * _sizeF);
    //    ofPoint _p3 = ofPoint(thresholdCPos.x + cos(ofDegToRad(30)) * thresholdCSize * _sizeF, thresholdCPos.y + sin(ofDegToRad(30)) * thresholdCSize * _sizeF);
    //    ofDrawTriangle( _p1, _p2, _p3 );
    //    ofPopStyle();
    
    
    ofPushStyle();
    ofSetColor( 0, _alpha );
    float _iX = intervalPos.x;
    float _iY = intervalPos.y;
    ofDrawLine( _iX - intervalSize, _iY, _iX, _iY + intervalSize );
    ofDrawLine( _iX, _iY - intervalSize, _iX + intervalSize, _iY );
    ofDrawLine( _iX + intervalSize, _iY, _iX, _iY + intervalSize );
    ofDrawLine( _iX, _iY - intervalSize, _iX - intervalSize, _iY );
    ofPopStyle();
    
    
    ofPushMatrix();
    ofPushStyle();
    
    ofSetColor( 255, 0, 0, 160 );
    int _xDefaultPos = lineScoreStepX * (lineScoreNumber-1);
    float _xL1 = ctrlPnW * 0.5 - _xDefaultPos * 0.5;
    //    ofDrawLine( _xL1, ctrlPnY + _yD, _xL1, ofGetHeight() - _yD);
    
    float _xL2 = ctrlPnW * 0.5 + _xDefaultPos * 0.5;
    //    ofDrawLine( _xL2, ctrlPnY + _yD, _xL2, ofGetHeight() - _yD);
    
    float _xM = ctrlPnW * 0.5;
    ofSetColor( 0, 40 );
    //    ofDrawLine( _xM, ctrlPnY + _yD, _xM, screenH - _yD);
    
    ofPopStyle();
    ofPopMatrix();
    
    ofPopMatrix();
    
    
    
    
}



//--------------------------------------------------------------
void ofApp::drawTrianglePixel(){
    
    int _pixelSize = pixelCircleSize;  // 10
    float _triSizeR = 1.7;
    
    ofPushMatrix();
    ofPushStyle();
    ofEnableAntiAliasing();
    
    if (whitePixels.size() > 1) {
        for (int i=0; i<whitePixels.size(); i++) {
            
            int _noteLoopIndex = ((i) % (whitePixels.size()-1))+1;
            int _pixelNumbers = whitePixels[ _noteLoopIndex ].pixelN;
            int _idPixels = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
            
            float _x = ((_idPixels) % changedCamSize) * pixelStepS * cameraScreenRatio - _pixelSize;
            float _y = (int)((_idPixels) / changedCamSize) * pixelStepS * cameraScreenRatio;
            
            ofPoint _1P = ofPoint( _x, _y - _pixelSize * _triSizeR * 0.75 );
            ofPoint _2P = ofPoint( _x - _pixelSize * _triSizeR * 0.55, _y + _pixelSize * _triSizeR * 0.25 );
            ofPoint _3P = ofPoint( _x + _pixelSize * _triSizeR * 0.55, _y + _pixelSize * _triSizeR * 0.25 );
            
            ofDrawTriangle( _1P, _2P, _3P );
            
        }
    }
    
    ofPopStyle();
    ofPopMatrix();
    
}



//--------------------------------------------------------------
void ofApp::drawPixelAllNoteShape(){
    
    ofPushMatrix();
    ofPushStyle();
    ofEnableAntiAliasing();
    
    ofSetColor( 0, 180 );
    
    for (int i=0; i<whitePixels.size(); i++) {
        
        int _noteLoopIndex = ((i) % (whitePixels.size()-1))+1;
        int _pixelNumbers = whitePixels[ _noteLoopIndex ].pixelN;
        int _idPixels = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
        
        float _x = ((_idPixels) % changedCamSize) * pixelStepS * cameraScreenRatio;
        float _y = (int)((_idPixels) / changedCamSize) * pixelStepS * cameraScreenRatio;
        ofPoint _p = ofPoint( _x, _y );
        
        float _size = ofMap( _pixelNumbers, 0, 400, 5, 100 );
        drawShape( _p, baseSelection, _size );
        
    }
    
    ofPopStyle();
    ofPopMatrix();
    
    
}



//--------------------------------------------------------------
void ofApp::drawPixelAllNoteShapes( vector<int> _vNote, int _scoreCh ){
    
    ofPushMatrix();
    ofPushStyle();
    //    ofEnableAntiAliasing();
    
    ofSetColor( 0, 60 );
    
    for (int i=0; i<whitePixels.size(); i++) {
        
        int _noteLoopIndex = ((i) % (whitePixels.size()-1))+1;
        int _pixelNumbers = whitePixels[ _noteLoopIndex ].pixelN;
        int _idPixels = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
        
        float _x = ((_idPixels) % changedCamSize) * pixelStepS * cameraScreenRatio;
        float _y = (int)((_idPixels) / changedCamSize) * pixelStepS * cameraScreenRatio;
        ofPoint _p = ofPoint( _x, _y );
        
        
        int _idLoopLine = ((i) % (whitePixels.size()-1)) + 1;
        int _idLoopLineOld = ((i + 1) % (whitePixels.size()-1)) + 1;
        
        int _note = _vNote[_idLoopLine];
        int _noteOld = _vNote[_idLoopLineOld];
        
        int _scaledNote = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
        int _scaledNoteOld = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteOld);
        
        
        if ( abs(_scaledNoteOld-_scaledNote) >= intervalDist ) {
            if (_note>0) {
                float _size = _scaledNote * pixeShapeSize;
                drawShape( _p, baseSelection, _size );
            }
        }
        
    }
    
    ofPopStyle();
    ofPopMatrix();
    
    
}



//--------------------------------------------------------------
void ofApp::drawPixelNumbersCircleNotes(){
    
    int _pixelSize = pixelCircleSize;
    float _ellipseSizeR = 0.7;
    
    ofPushMatrix();
    ofPushStyle();
    ofEnableAntiAliasing();
    
    ofSetColor( 0, 30 );
    
    if (whitePixels.size()>0) {
        
        int _noteLoopIndex = ((noteIndex) % (whitePixels.size()-1))+1;
        int _pixelNumbers = whitePixels[_noteLoopIndex].pixelN;
        int _idPixels = whitePixels[_noteLoopIndex].indexPos - _pixelNumbers;
        
        for (int i=0; i<_pixelNumbers; i++){
            
            float _xS = ((_idPixels+i) % changedCamSize) * pixelStepS * cameraScreenRatio;
            float _yS = (int)((_idPixels+i) / changedCamSize) * pixelStepS * cameraScreenRatio;
            
            //            ofFill();
            //            ofSetColor( 255, 20 );
            //            ofDrawCircle( _xS, _yS, _pixelSize * _ellipseSizeR );
            
            ofNoFill();
            
            ofSetColor( 0, 120 );
            
            ofDrawCircle( _xS, _yS, _pixelSize * _ellipseSizeR );
            
        }
        
    }
    
    ofPopStyle();
    ofPopMatrix();
    
}



//--------------------------------------------------------------
void ofApp::drawPlayingShapeNotes(){
    
    ofPushMatrix();
    ofPushStyle();
    ofEnableAntiAliasing();
    
    ofSetColor( 0, 120 );
    
    if (whitePixels.size()>0) {
        
        int _noteLoopIndex = ((noteIndex) % (whitePixels.size()-1))+1;
        int _pixelNumbers = whitePixels[ _noteLoopIndex ].pixelN;
        int _idPixels = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
        
        float _x = ((_idPixels) % changedCamSize) * pixelStepS * cameraScreenRatio;
        float _y = (int)((_idPixels) / changedCamSize) * pixelStepS * cameraScreenRatio;
        ofPoint _p = ofPoint( _x, _y );
        
        drawShape( _p, baseSelection, _pixelNumbers);
        
    }
    
    ofPopStyle();
    ofPopMatrix();
    
}



//--------------------------------------------------------------
void ofApp::drawPlayingShapeNote( vector<int> _vNote, int _scoreCh ){
    
    ofPushMatrix();
    ofPushStyle();
    
    float _h = ofMap( _scoreCh, 1, 7, 0, 255 );
    ofColor _c = ofColor::fromHsb( _h, 180, 255, 255 );
    
    if (whitePixels.size()>0) {
        
        int _noteLoopIndex = ((noteIndex) % (whitePixels.size()-1))+1;
        int _pixelNumbers = whitePixels[ _noteLoopIndex ].pixelN;
        int _idPixels = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
        
        float _x = ((_idPixels) % changedCamSize) * pixelStepS * cameraScreenRatio;
        float _y = (int)((_idPixels) / changedCamSize) * pixelStepS * cameraScreenRatio;
        ofPoint _p = ofPoint( _x, _y );
        
        int _idLoopLineOld = ((1 + noteIndex) % (whitePixels.size()-1)) + 1;
        
        int _note = _vNote[_noteLoopIndex];
        int _noteOld = _vNote[_idLoopLineOld];
        
        int _scaledNote = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
        int _scaledNoteOld = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteOld);
        
        if ( abs(_scaledNoteOld-_scaledNote) >= intervalDist ) {
            if (_note>0) {
                //                drawShapeCeterLine( _p, baseSelection, _pixelNumbers);
                
                float _size = _scaledNote * pixeShapeSize;
                drawShapeCeterLineColorRotation( _p, baseSelection, _size, _c );
            }
        }
        
    }
    
    ofPopStyle();
    ofPopMatrix();
    
}



//--------------------------------------------------------------
void ofApp::drawLineScore(){
    
    ofPushMatrix();
    ofTranslate((ofGetWidth() - 600) * 0.5, 0);
    
    if (bCameraCapturePlay) {
        
        int _xNumber = lineScoreNumber;
        int _stepX = lineScoreStepX;
        int _stepY = lineScoreStepY;
        int _defaultNote = 56;
        int _xDefaultPos = _stepX * (_xNumber-1);
        
        
        ofPushMatrix();
        ofTranslate( ctrlPnW * 0.5 - _xDefaultPos * 0.5, ctrlPnY + 127 * _stepY - _defaultNote );
        
        ofPushStyle();
        
        ofSetColor( 0, 120 );
        
        
        vector< vector<int> > _list = scoreMakeOutput();
        for (int i=0; i<_list.size(); i++) {
            drawScoreCircleLine( _list[i], i );
        }

//        for (int i=0; i<noteLists.size(); i++) {
//            drawScoreCircleLine( noteLists[i].noteArray, i );
//        }
        
    }
    
    ofPopStyle();
    ofPopMatrix();
    
    ofPopMatrix();
    
    
}



//--------------------------------------------------------------
void ofApp::drawScoreCircleLine( vector<int> _scoreNote, int _scoreCh ){
    
    
    int _xNumber = lineScoreNumber;
    int _middle = _xNumber * 0.5;
    int _stepX = lineScoreStepX;
    int _stepY = lineScoreStepY;
    int _defaultNote = 56;
    int _size = 3;
    int _xDefaultPos = _stepX * (_xNumber-1);
    
    float _h = ofMap( _scoreCh, 1, 7, 0, 255 );
    ofColor _c = ofColor::fromHsb( _h, 180, 255, 180 );
    
    
    if (_scoreNote.size()>0) {
        
        ofPushStyle();
        
        for (int i=0; i<_xNumber; i++){
            
            int _idLoopLine = ((i + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            int _idLoopLineOld = ((i + 1 + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            
            int _note = _scoreNote[_idLoopLine];
            int _noteOld = _scoreNote[_idLoopLineOld];
            
            int _scaledNote = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
            int _scaledNoteOld = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteOld);
            
            float _x1 = _xDefaultPos - i * _stepX;
            float _y1 = _defaultNote - _scaledNote * _stepY;
            
            if ( abs(_scaledNoteOld-_scaledNote) >= intervalDist ) {
                ofColor _c;
                if (i==11) {
                    _c = ofColor::fromHsb( _h, 255, 255, 255 );
                } else {
                    _c = ofColor::fromHsb( _h, 180, 255, 120 );
                }
                if (_note>0) {
                    ofSetColor( _c );
                    ofDrawCircle( _x1, _y1, _size );
                }
            }
            
        }
        
        ofPopStyle();
        
        ofPushStyle();
        ofSetColor( _c, 60 );
        
        
        for (int i=0; i<_xNumber-1; i++){
            
            int _idLoopLineS = ((i + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            int _idLoopLineE = ((i + 1 + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            
            int _idLoopLineEOld = ((i + 2 + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            
            int _noteS = _scoreNote[_idLoopLineS];
            int _noteE = _scoreNote[_idLoopLineE];
            int _noteEOld = _scoreNote[_idLoopLineEOld];
            
            int _noteSScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteS);
            int _noteEScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteE);
            int _noteEOldScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteEOld);
            
            float _x1 = _xDefaultPos - i * _stepX;
            float _y1 = _defaultNote - _noteSScaled * _stepY;
            float _x2 = _xDefaultPos - (i + 1) * _stepX;
            float _y2 = _defaultNote - _noteEScaled * _stepY;
            
            
            if ( (abs(_noteEScaled-_noteSScaled) >= intervalDist) && abs(_noteEOldScaled-_noteEScaled) >= intervalDist ) {
                if ( _noteS > 0 && _noteE > 0 ) {
                    ofDrawLine( _x1, _y1, _x2, _y2 );
                }
            }
        }
        
        ofPopStyle();
        
    }
    
}




//--------------------------------------------------------------
void ofApp::drawBaseInterface(){
    
    ofPushMatrix();
    ofPushStyle();
    
    ofColor _c[6];
    
    for (int i=0; i<6; i++) {
        if ( baseSelection == (4 + i) ) {
            _c[i] = ofColor::fromHsb( i*40, 255, 255 );
        } else {
            _c[i] = ofColor(0, 0, 0);
        }
    }
    
    drawShapeCeterLine( base4Pos, 4, baseSize, _c[0] );
    drawShapeCeterLine( base5Pos, 5, baseSize, _c[1] );
    drawShapeCeterLine( base6Pos, 6, baseSize, _c[2] );
    drawShapeCeterLine( base7Pos, 7, baseSize, _c[3] );
    drawShapeCeterLine( base8Pos, 8, baseSize, _c[4] );
    drawShapeCeterLine( base9Pos, 9, baseSize, _c[5] );
    
    ofPopMatrix();
    ofPopStyle();
    
}



//--------------------------------------------------------------
void ofApp::drawShapeCeterLine(ofPoint _pos, int _base, int _size, ofColor _c){
    
    vector<ofPoint> _posLine;
    
    for (int i=0; i<_base; i++) {
        float _sizeDegree = i * 360 / _base + 180.0;
        float _x = sin(ofDegToRad( _sizeDegree )) * _size;
        float _y = cos(ofDegToRad( _sizeDegree )) * _size;
        
        ofPoint _p = ofPoint( _x, _y );
        _posLine.push_back( _p );
    }
    
    
    ofPushMatrix();
    ofPushStyle();
    
    ofTranslate( _pos );
    
    ofSetColor( _c, 60 );
    
    for (int i=0; i<_posLine.size(); i++){
        ofDrawLine( 0, 0, _posLine[i].x, _posLine[i].y );
    }
    
    ofSetColor( _c, 180 );
    
    for (int i=0; i<_posLine.size()-1; i++){
        ofDrawLine( _posLine[i].x, _posLine[i].y, _posLine[i+1].x, _posLine[i+1].y );
    }
    ofDrawLine( _posLine[0].x, _posLine[0].y, _posLine[_posLine.size()-1].x, _posLine[_posLine.size()-1].y );
    
    ofPopMatrix();
    ofPopStyle();
    
}



//--------------------------------------------------------------
void ofApp::drawShapeCeterLineColorRotation(ofPoint _p, int _base, int _size, ofColor _c){
    
    vector<ofPoint> _posLine;
    
    for (int i=0; i<_base; i++) {
        float _sizeDegree = i * 360 / _base + 180.0;
        float _x = sin(ofDegToRad( _sizeDegree )) * _size;
        float _y = cos(ofDegToRad( _sizeDegree )) * _size;
        
        ofPoint _p = ofPoint( _x, _y );
        _posLine.push_back( _p );
    }
    
    
    ofPushMatrix();
    ofPushStyle();
    
    
    ofTranslate( _p );
    ofRotateZDeg( 45 );
    
    ofSetLineWidth( 3 );
    
    ofSetColor( _c.r, _c.g, _c.b, _c.a * 0.2 );
    for (int i=0; i<_posLine.size(); i++){
        ofDrawLine( 0, 0, _posLine[i].x, _posLine[i].y );
    }
    
    ofSetColor( _c );
    for (int i=0; i<_posLine.size()-1; i++){
        ofDrawLine( _posLine[i].x, _posLine[i].y, _posLine[i+1].x, _posLine[i+1].y );
    }
    ofDrawLine( _posLine[0].x, _posLine[0].y, _posLine[_posLine.size()-1].x, _posLine[_posLine.size()-1].y );
    
    ofPopMatrix();
    ofPopStyle();
    
}




//--------------------------------------------------------------
void ofApp::drawShape(ofPoint _pos, int _base, int _size){
    
    
    vector<ofPoint> _posLine;
    
    for (int i=0; i<_base; i++) {
        float _sizeDegree = i * 360 / _base + 180.0;
        float _x = sin(ofDegToRad( _sizeDegree )) * _size;
        float _y = cos(ofDegToRad( _sizeDegree )) * _size;
        
        ofPoint _p = ofPoint( _x, _y );
        _posLine.push_back( _p );
    }
    
    
    ofPushMatrix();
    
    ofTranslate( _pos );
    
    for (int i=0; i<_posLine.size()-1; i++){
        ofDrawLine( _posLine[i].x, _posLine[i].y, _posLine[i+1].x, _posLine[i+1].y );
    }
    ofDrawLine( _posLine[0].x, _posLine[0].y, _posLine[_posLine.size()-1].x, _posLine[_posLine.size()-1].y );
    
    ofPopMatrix();
    
}




//--------------------------------------------------------------
void ofApp::debugControlPDraw(){
    
    ofPushMatrix();
    ofPushStyle();
    
    ofSetColor( 0 );
    
    for (int i=0; i<15; i++){
        float _x1 = i * guideWidthStep + guideWidthStep;
        ofDrawLine( _x1, ctrlPnY, _x1, screenH );
    }
    
    for (int j=0; j<7; j++){
        float _y1 = j * guideHeightStep + guideHeightStep;
        ofDrawLine( 0, _y1 + ctrlPnY, screenW, _y1 + ctrlPnY );
    }
    
    ofPopStyle();
    ofPopMatrix();
    
    ofPushStyle();
    ofSetColor(0);
    ofDrawBitmapString( ofToString(ofGetFrameRate(),2), 10, screenH-10 );
    ofPopStyle();
    
}




//--------------------------------------------------------------
void ofApp::audioRequested (float * output, int bufferSize, int nChannels){
    
    synthMain.fillBufferOfFloats(output, bufferSize, nChannels);

}



//--------------------------------------------------------------
void ofApp::mainCaptureOnOff(){
    
    //    allPlayOnOff = !allPlayOnOff;
    
    
    if ((whitePixels.size()>2)) {
        bCameraCapturePlay = !bCameraCapturePlay;
        //            blur(edge, 3);
        bufferImg = edge;
        
        if ( !bCameraCapturePlay ) {
            index = -1;
            ofRemoveListener(*metroOut, this, &ofApp::triggerReceive);
            
            vector< vector<int> > _list = scoreMakeOutput();
            for (int i=0; i<_list.size(); i++) {
                _list[i].clear();
            }
            
            vector< vector<int> > _melody = melodyOutput();
            for (int i=0; i<_melody.size(); i++) {
                _melody[i].clear();
            }
            
        } else {
            scoreMake();
//            printScoreMake();
            //                noteIndex = index;
            ofAddListener(*metroOut, this, &ofApp::triggerReceive);
        }
        
        grayThresholdTouch = 120;
        touchDownDefault = 0;
    }
    
}


//--------------------------------------------------------------
void ofApp::mainCaptureOff(){
    
//    vector<int> oldScoreNote;
    
    if ((whitePixels.size()>2)) {
        bCameraCapturePlay = false;
        //            blur(edge, 3);
        bufferImg = edge;
        
        if ( !bCameraCapturePlay ) {
            index = -1;
            ofRemoveListener(*metroOut, this, &ofApp::triggerReceive);

            vector< vector<int> > _list = scoreMakeOutput();
            for (int i=0; i<_list.size(); i++) {
                _list[i].clear();
            }
            
            vector< vector<int> > _melody = melodyOutput();
            for (int i=0; i<_melody.size(); i++) {
                _melody[i].clear();
            }
            
        }
    }
    
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}



//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if (key == ' ') {
        
        mainCaptureOnOff();
        
    } else if (key == 'g') {
        
        debugView = !debugView;
        
    }  else if (key == 's') {
        
        printScore();
        
    } else if (key == 'e') {
        
        printer.close();
//        printer.open("/dev/cu.usbserial");
        printer.open("/dev/cu.usbmodem1421");

    } else if (key == 'i') {
        
        printCamView();
        
    } else if (key == 'h') {
        
        printHeader();
        
    } else if (key == 'f') {
        
        printFooter();
        
    } else if (key == 'r') {
        
        printer.close();
        printer.reset();
//        printer.open("/dev/cu.usbserial");
        printer.open("/dev/cu.usbmodem1421");
        string _date = ofGetTimestampString();
        printer.println("@RESET");
        printer.println("------------------------");
        printer.println(_date);
        printer.println("------------------------");
        printer.println(" ");

    }
    
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}



//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    ofPoint _adjustTouchPos = ofPoint(x, y);
    
    if (bSpeedCtrl) {
        float _minY = speedLineYMin;
        float _maxY = speedLineYMax;
        
        speedCPos.y = min(max(_adjustTouchPos.y, _minY), _maxY);
        float _tempo = ofMap( speedCPos.y, _minY, _maxY, maxSpeed, minSpeed );
        synthMain.setParameter("tempo", _tempo);
        
        if ( (_adjustTouchPos.y > _minY) && (_adjustTouchPos.y < _maxY) &&
            (_adjustTouchPos.x > controlPos.x - 30) && (_adjustTouchPos.x < controlPos.x + 30) ) {
        }
    }
    
    //        if (bthresholdCtrl) {
    //            float _minY = ctrlPnY + speedCSize * 0.75;
    //            float _maxY = screenH - speedCSize * 0.75;
    //
    //            if ((_changedTouch.y>_minY)&&(_changedTouch.y<_maxY)) {
    //                thresholdCPos.y = _changedTouch.y;
    //                float _threshold = ofMap(thresholdCPos.y, _minY, _maxY, 255, 0);
    //                grayThresholdTouch = _threshold;
    //            }
    //        }
    
    
    if (bInterval) {
        float _minY = intervalLineYMin;
        float _maxY = intervalLineYMax;
        
        intervalPos.y = min(max(_adjustTouchPos.y, _minY), _maxY);
        float _interval = ofMap(intervalPos.y, _minY, _maxY, 0, 4);
        intervalDist = _interval;
        
        if ((_adjustTouchPos.y>_minY)&&(_adjustTouchPos.y<_maxY) &&
            _adjustTouchPos.x<intervalPos.x * 2 ) {
        }
    }
    
    
    //    if ( touch.id==0 ) {
    //        if (bSpeedCtrl) {
    //            float _minY = ctrlPnY + speedCSize * 0.75;
    //            float _maxY = screenH - speedCSize * 0.75;
    //
    //            if ((_changedTouch.y>_minY)&&(_changedTouch.y<_maxY)) {
    //                speedCPos.y = _changedTouch.y;
    //                float _tempo = ofMap( speedCPos.y, _minY, _maxY, maxSpeed, minSpeed );
    //                synthMain.setParameter("tempo", _tempo);
    //            }
    //
    //        }
    //
    ////        if (bthresholdCtrl) {
    ////            float _minY = ctrlPnY + speedCSize * 0.75;
    ////            float _maxY = screenH - speedCSize * 0.75;
    ////
    ////            if ((_changedTouch.y>_minY)&&(_changedTouch.y<_maxY)) {
    ////                thresholdCPos.y = _changedTouch.y;
    ////                float _threshold = ofMap(thresholdCPos.y, _minY, _maxY, 255, 0);
    ////                grayThresholdTouch = _threshold;
    ////            }
    ////        }
    //
    //
    //        if (bInterval) {
    //            float _minY = ctrlPnY + speedCSize * 0.75;
    //            float _maxY = screenH - speedCSize * 0.75;
    //            if ((_changedTouch.y>_minY)&&(_changedTouch.y<_maxY)) {
    //                intervalPos.y = _changedTouch.y;
    //                float _interval = ofMap(intervalPos.y, _minY, _maxY, 0, 20);
    //                intervalDist = _interval;
    //            }
    //        }
    //    }
    
    
    if ( (_adjustTouchPos.x>screenPos.x)&&(_adjustTouchPos.x<(screenPos.x+screenW)) &&
        (_adjustTouchPos.y<screenH)&&(_adjustTouchPos.y>0) ) {
        grayThresholdTouch = 120 + (_adjustTouchPos.y - touchDownDefault);

        ctrl12OnOff = false;
    
        mainCaptureOff();
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
    float _tolerance = 3;
    
    ofPoint _adjustTouchPos = ofPoint(x, y);
    
    
    float _distSpeed = ofDist( speedCPos.x, speedCPos.y, _adjustTouchPos.x, _adjustTouchPos.y );
    if ( (_distSpeed < thresholdCSize * _tolerance) && bSpeedCtrl == false) {
        bSpeedCtrl = true;
    }
    
    
    float _distInterval = ofDist( intervalPos.x, intervalPos.y, _adjustTouchPos.x, _adjustTouchPos.y );
    if ( (_distInterval < intervalSize * _tolerance) && bInterval == false) {
        bInterval = true;
    }
    
    //    if ( touch.id==0 ) {
    
    //        float _distS = ofDist( speedCPos.x, speedCPos.y , _changedTouch.x, _changedTouch.y );
    //
    //        if (_distS < thresholdCSize * _tolerance) {
    //            bSpeedCtrl = true;
    //        } else {
    //            bSpeedCtrl = false;
    //        }
    
    //        float _distT = ofDist( thresholdCPos.x, thresholdCPos.y , _changedTouch.x, _changedTouch.y );
    
    //        if (_distT < thresholdCSize * _tolerance) {
    //            bthresholdCtrl = true;
    //        } else {
    //            bthresholdCtrl = false;
    //        }
    
    //        float _distI = ofDist( intervalPos.x, intervalPos.y , _changedTouch.x, _changedTouch.y );
    //
    //        if (_distI < intervalSize * _tolerance) {
    //            bInterval = true;
    //        } else {
    //            bInterval = false;
    //        }
    
    
    //    }
    
    if ( (_adjustTouchPos.x>screenPos.x)&&(_adjustTouchPos.x<(screenPos.x+screenW)) &&
        (_adjustTouchPos.y<screenH)&&(_adjustTouchPos.y>0) ) {
        
        mainCaptureOff();
        ctrl12OnOff = false;
        grayThresholdTouch = 120;
        touchDownDefault = _adjustTouchPos.y;
    }
    
    
    
    touchOnOffCheck = true;
    
}




//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
    ofPoint _adjustTouchPos = ofPoint(x, y);
    
    if ( (_adjustTouchPos.x>screenPos.x)&&(_adjustTouchPos.x<(screenPos.x+screenW)) &&
        (_adjustTouchPos.y<screenH)&&(_adjustTouchPos.y>0) && touchOnOffCheck) {
        mainCaptureOnOff();
        touchOnOffCheck = false;
    }
    
    
    float _tolerance = 2;
    
    float _distS = ofDist( speedCPos.x, speedCPos.y , _adjustTouchPos.x, _adjustTouchPos.y );
    if (_distS < (thresholdCSize * _tolerance) && bSpeedCtrl==true) {
        bSpeedCtrl = false;
    }
    
    float _distI = ofDist( intervalPos.x, intervalPos.y , _adjustTouchPos.x, _adjustTouchPos.y );
    if (_distI < (intervalSize * _tolerance) && bInterval == true) {
        bInterval = false;
        scoreMake();
//        printScoreMake();
    }
    
    
    if (printButton.inside( _adjustTouchPos )) {
//        printScore();
        printAll = true;
    }
    
    
    if (playStopArea.inside( _adjustTouchPos )) {
        mainCaptureOnOff();
    }

    
    
    
    float _4BaseDist = ofDist( _adjustTouchPos.x, _adjustTouchPos.y, base4Pos.x, base4Pos.y );
    if ( _4BaseDist < baseSize ) {
        baseSelection = 4;
        bChangedBaseNum = true;
    }
    
    float _5BaseDist = ofDist( _adjustTouchPos.x, _adjustTouchPos.y, base5Pos.x, base5Pos.y );
    if ( _5BaseDist < baseSize ) {
        baseSelection = 5;
        bChangedBaseNum = true;
    }
    
    float _6BaseDist = ofDist( _adjustTouchPos.x, _adjustTouchPos.y, base6Pos.x, base6Pos.y );
    if ( _6BaseDist < baseSize ) {
        baseSelection = 6;
        bChangedBaseNum = true;
    }
    
    float _7BaseDist = ofDist( _adjustTouchPos.x, _adjustTouchPos.y, base7Pos.x, base7Pos.y );
    if ( _7BaseDist < baseSize ) {
        baseSelection = 7;
        bChangedBaseNum = true;
    }
    
    float _8BaseDist = ofDist( _adjustTouchPos.x, _adjustTouchPos.y, base8Pos.x, base8Pos.y );
    if ( _8BaseDist < baseSize ) {
        baseSelection = 8;
        bChangedBaseNum = true;
    }
    
    float _9BaseDist = ofDist( _adjustTouchPos.x, _adjustTouchPos.y, base9Pos.x, base9Pos.y );
    if ( _9BaseDist < baseSize ) {
        baseSelection = 9;
        bChangedBaseNum = true;
    }
    
    
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



//--------------------------------------------------------------
void ofApp::printAllElement(){
    
//    if (printAll) {
//        string _date = ofGetTimestampString();
//        printer.println("------------------------");
//        printer.println(_date);
//        printer.println("------------------------");
//        printer.println(" ");
//        printCamView();
//        printScore();
//        printAll = false;
//        printHeaderOnOff = true;
//        printFooterOnOff = true;
//    }
//    
//    if (printer.isThreadRunning()) {
//    }
//    
//    if (!printer.isThreadRunning() && printHeaderOnOff) {
//        printer.println("");
//        printer.printBarcode("12345678", EAN8);
//        printHeaderOnOff = false;
//    }
//    
//    if (!printer.isThreadRunning() && printFooterOnOff) {
//        printFooter();
//        printFooterOnOff = false;
//        printImgOnOff = true;
//    }
    
    
}




//--------------------------------------------------------------
void ofApp::printScore(){
    
    printer.println("------------------------");
    printer.println(" ");
    ofPixels _p;
    printScoreFbo.readToPixels(_p);
//    ofImage _image;
//    float _w = printScoreFbo.getWidth();
//    float _h = printScoreFbo.getHeight();
//    _image.setFromPixels(_p.getData(), _w, _h, OF_IMAGE_COLOR_ALPHA);
    printer.print(_p, 250);
    
}



//--------------------------------------------------------------
void ofApp::printCamView(){
    
    printCam = bufferImg;
    printCam.resize(384, 384);
    
    rotate(printCam, printCam, -90);
    printer.print(printCam, 10);
    
}



//--------------------------------------------------------------
void ofApp::printHeader(){
    
    string _date = ofGetTimestampString();
    printer.println("------------------------");
    printer.println(_date);
    printer.println("------------------------");
    printer.println(" ");
    
}



//--------------------------------------------------------------
void ofApp::printFooter(){
    
    string _date = ofGetTimestampString();
    printer.println("------------------------");
    printer.println(" ");
    printer.println("Thanks !!!");
    printer.println(" ");
    printer.println(" ");
    printer.println(" ");
    printer.println("------------------------");
    printer.println(" ");
    printer.println(" ");
    printer.println(" ");
    printer.println(" ");
    
}



//--------------------------------------------------------------
void ofApp::synthSetting(){
    
    float _volume = 0.125;
    
    ControlParameter carrierPitch1 = synth1.addParameter("carrierPitch1");
    float amountMod1 = 1;
    ControlGenerator rCarrierFreq1 = ControlMidiToFreq().input(carrierPitch1);
    ControlGenerator rModFreq1 = rCarrierFreq1 * 2.5;
    Generator modulationTone1 = SineWave().freq( rModFreq1 ) * rModFreq1 * amountMod1;
    Generator tone1 = SineWave().freq(rCarrierFreq1 + modulationTone1);
    ControlGenerator envelopTrigger1 = synth1.addParameter("trigger1");
    Generator env1 = ADSR().attack(0.001).decay(0.3).sustain(0).release(0).trigger(envelopTrigger1).legato(false);
    synth1.setOutputGen( tone1 * env1 * _volume );
    
    ControlParameter carrierPitch2 = synth2.addParameter("carrierPitch2");
    float amountMod2 = 1;
    ControlGenerator rCarrierFreq2 = ControlMidiToFreq().input(carrierPitch2);
    ControlGenerator rModFreq2 = rCarrierFreq2 * 3.489;
    Generator modulationTone2 = SineWave().freq( rModFreq2 ) * rModFreq2 * amountMod2;
    Generator tone2 = SineWave().freq(rCarrierFreq2 + modulationTone2);
    ControlGenerator envelopTrigger2 = synth2.addParameter("trigger2");
    Generator env2 = ADSR().attack(0.001).decay(0.1).sustain(0).release(0).trigger(envelopTrigger2).legato(false);
    synth2.setOutputGen( tone2 * env2 * _volume );
    
    ControlParameter carrierPitch3 = synth3.addParameter("carrierPitch3");
    float amountMod3 = 12;
    ControlGenerator rCarrierFreq3 = ControlMidiToFreq().input(carrierPitch3);
    ControlGenerator rModFreq3 = rCarrierFreq3 * 1.2;
    Generator modulationTone3 = SineWave().freq( rModFreq3 ) * rModFreq3 * amountMod3;
    Generator tone3 = SineWave().freq(rCarrierFreq3 + modulationTone3);
    ControlGenerator envelopTrigger3 = synth3.addParameter("trigger3");
    Generator env3 = ADSR().attack(0.001).decay(0.1).sustain(0).release(0).trigger(envelopTrigger3).legato(true);
    synth3.setOutputGen( tone3 * env3 * _volume );
    
    ControlParameter carrierPitch4 = synth4.addParameter("carrierPitch4");
    float amountMod4 = 18;
    ControlGenerator rCarrierFreq4 = ControlMidiToFreq().input(carrierPitch4);
    ControlGenerator rModFreq4 = rCarrierFreq4 * 1.1;
    Generator modulationTone4 = SineWave().freq( rModFreq4 ) * rModFreq4 * amountMod4;
    Generator tone4 = SineWave().freq(rCarrierFreq4 + modulationTone4);
    ControlGenerator envelopTrigger4 = synth4.addParameter("trigger4");
    Generator env4 = ADSR().attack(0.001).decay(0.1).sustain(0).release(0).trigger(envelopTrigger4).legato(false);
    synth4.setOutputGen( tone4 * env4 * _volume );
    
    ControlParameter carrierPitch5 = synth5.addParameter("carrierPitch5");
    float amountMod5 = 6;
    ControlGenerator rCarrierFreq5 = ControlMidiToFreq().input(carrierPitch5);
    ControlGenerator rModFreq5 = rCarrierFreq5 * 1.489;
    Generator modulationTone5 = SineWave().freq( rModFreq5 ) * rModFreq5 * amountMod5;
    Generator tone5 = SineWave().freq(rCarrierFreq5 + modulationTone5);
    ControlGenerator envelopTrigger5 = synth5.addParameter("trigger5");
    Generator env5 = ADSR().attack(0.001).decay(0.1).sustain(0).release(0).trigger(envelopTrigger5).legato(false);
    synth5.setOutputGen( tone5 * env5 * _volume );
    
    ControlParameter carrierPitch6 = synth6.addParameter("carrierPitch6");
    float amountMod6 = 2;
    ControlGenerator rCarrierFreq6 = ControlMidiToFreq().input(carrierPitch6);
    ControlGenerator rModFreq6 = rCarrierFreq6 * 1.109;
    Generator modulationTone6 = SineWave().freq( rModFreq6 ) * rModFreq6 * amountMod6;
    Generator tone6 = SineWave().freq(rCarrierFreq6 + modulationTone6);
    ControlGenerator envelopTrigger6 = synth6.addParameter("trigger6");
    Generator env6 = ADSR().attack(0.001).decay(0.1).sustain(0).release(0).trigger(envelopTrigger6).legato(false);
    synth6.setOutputGen( tone6 * env6 * _volume );
    
    ControlParameter carrierPitch7 = synth7.addParameter("carrierPitch7");
    float amountMod7 = 4;
    ControlGenerator rCarrierFreq7 = ControlMidiToFreq().input(carrierPitch7);
    ControlGenerator rModFreq7 = rCarrierFreq7 * 3.109;
    Generator modulationTone7 = SineWave().freq( rModFreq7 ) * rModFreq7 * amountMod7;
    Generator tone7 = SineWave().freq(rCarrierFreq7 + modulationTone7);
    ControlGenerator envelopTrigger7 = synth7.addParameter("trigger7");
    Generator env7 = ADSR().attack(0.001).decay(0.2).sustain(0).release(0).trigger(envelopTrigger7).legato(false);
    synth7.setOutputGen( tone7 * env7 * _volume );
    
}



//--------------------------------------------------------------
void ofApp::scoreMake(){
    
    
//    for (int i=0; i<noteLists.size(); i++) {
//        noteLists[i].noteArray.clear();
//    }
//    
//    int _intervalDist = intervalDist;
//    
//    for (int i=0; i<whitePixels.size(); i++) {
//        
//        vector<int> _bitNumber;
//        _bitNumber.resize(7);
//        
//        int _idLoop = ((i) % (whitePixels.size()-1))+1;
//        int _pixelNrs = whitePixels[ _idLoop ].pixelN;
//        _bitNumber = convertDecimalToNBase( _pixelNrs, baseSelection, (int)_bitNumber.size());
//        
//        for (int j=0; j<noteLists.size(); j++) {
//            
//            if ( abs(_bitNumber[j] - oldScoreNote[j]) >= _intervalDist ) {
//                noteLists[j].noteArray.push_back(_bitNumber[j]);
//            } else {
//                noteLists[j].noteArray.push_back(-1);
//            }
//            oldScoreNote[j] = _bitNumber[j];
//            
//        }
//        
//    }
    
}



//--------------------------------------------------------------
vector< vector<int> > ofApp::scoreMakeOutput(){
    
    vector< vector<int> > _noteList;
    _noteList.resize(NOTE_SIZE);

    
    vector<int> _bufferScoreNote;
    _bufferScoreNote.resize(NOTE_SIZE);
    for (int i=0; i<NOTE_SIZE; i++){
        _bufferScoreNote[i] = 0;
    }
    
//    vector<int> oldScoreNote;
//    oldScoreNote.resize(NOTE_SIZE);
    
    int _intervalDist = intervalDist;
    
    vector<int> _oldNote = oldNoteOutput();

    
    if (whitePixels.size()>1) {
        for (int i=0; i<whitePixels.size(); i++) {
            
            vector<int> _bitNumber;
            _bitNumber.resize(NOTE_SIZE);
            
            int _idLoop = ((i) % (whitePixels.size()-1))+1;
            int _pixelNrs = whitePixels[ _idLoop ].pixelN;
            _bitNumber = convertDecimalToNBase( _pixelNrs, baseSelection, (int)_bitNumber.size());
            
            for (int j=0; j<NOTE_SIZE; j++) {
                
                if ( abs(_bitNumber[j] - _bufferScoreNote[j]) >= _intervalDist ) {
                    _noteList[j].push_back(_bitNumber[j]);
                } else {
                    _noteList[j].push_back(-1);
                }
                _bufferScoreNote[j] = _bitNumber[j];
                
            }
            
        }
    }
    
    return _noteList;
    
}


//--------------------------------------------------------------
vector<int> ofApp::oldNoteOutput(){
    
    vector<int> _oldNote;
    _oldNote.resize(NOTE_SIZE);
    
    int _intervalDist = intervalDist;
    
    if (whitePixels.size()>1) {
        for (int i=0; i<whitePixels.size(); i++) {
            
            vector<int> _bitNumber;
            _bitNumber.resize(NOTE_SIZE);
            
            int _idLoop = ((i) % (whitePixels.size()-1))+1;
            int _pixelNrs = whitePixels[ _idLoop ].pixelN;
            _bitNumber = convertDecimalToNBase( _pixelNrs, baseSelection, (int)_bitNumber.size());
            
            for (int j=0; j<NOTE_SIZE; j++) {
                
                _oldNote[j] = _bitNumber[j];
                
            }
            
        }
    }
    
    return _oldNote;
    
}



//--------------------------------------------------------------
void ofApp::trigScoreNote( vector<int> _vNote, ofxTonicSynth _synthIn, int _scoreCh ){
    
    int _idLoop = ((noteIndex) % (whitePixels.size()-1))+1;
    int _idLoopOld = ((noteIndex + 1) % (whitePixels.size()-1))+1;
    
    vector<int> _scoreNote = _vNote;
    ofxTonicSynth _synth = _synthIn;
    
    int _note = _scoreNote[_idLoop];
    int _noteOld = _scoreNote[_idLoopOld];
    
    int _scaledNote = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
    int _scaledNoteOld = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteOld);
    
    string tName = "trigger" + ofToString(_scoreCh);
    string tPitch = "carrierPitch" + ofToString(_scoreCh);
    
    if ( abs(_scaledNoteOld - _scaledNote) >= intervalDist ) {
        if (_note>0) {
            int _scaledNoteE = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
            _synth.setParameter( tName, 1);
            _synth.setParameter( tPitch, _scaledNoteE);
        }
    }
    
}




//--------------------------------------------------------------
vector<int> ofApp::convertDecimalToNBase(int n, int base, int size) {
    
    int i=0, div, res;
    
    vector<int> a;
    a.clear();
    a.resize(size);
    
    div=n/base;
    res=n%base;
    a[i] = res;
    
    while(1){
        if(div==0) break;
        else {
            i++;
            res=div%base;
            div=div/base;
            a[i] = res;
        }
    }
    return a;
    
}



//--------------------------------------------------------------
void ofApp::guiSetting(){
    
    parameters.setName("Main");
    parameters.add(thresholdGui.set("Threshold", 120, 0, 255));
    parameters.add(mainVolume.set("Volume", 0.5, 0, 1));
    parameters.add(baseNum.set("Base Selection", 7, 4, 9));
    parameters.add(frameRate.set("fps/s", ""));
    parameters.add(noteNum.set("Notes", ""));
    parameters.add(faceNum.set("Face Num", ""));
    
    parametersMain.add(parameters);
    
    gui.setup(parametersMain);
    
}



//--------------------------------------------------------------
void ofApp::changedBaseNum(int & param){
    
    bChangedBaseNum = true;
    
}




//--------------------------------------------------------------
void ofApp::exit(){
    
    ofSoundStreamStop();
    printer.close();
    
}
