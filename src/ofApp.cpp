// http://www.translatorscafe.com/cafe/units-converter/numbers/calculator/octal-to-decimal/


#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

int octaveScaleFactor[7] = {60, 86, 36, 48, 72, 36, 36};


//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetFrameRate(60);

    ofEnableAntiAliasing();
    
    // !!! Only Full Screen !!!
    ofSetFullscreen(true);
    
    ofSetCircleResolution(16);
    
    printer.open("/dev/cu.usbserial-A900adIr");

    gclef.load("GClef.png");
    fclef.load("FClef.png");
    
    baseSelection = 7;
    
    ofBackground(255);
    
    ofSetFrameRate( 60 );
    ofEnableAlphaBlending();
    
    guiSetting();
    
    
    cam.setDeviceID(0);
    
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
    pixelCircleSize     = 10 / _widthDefault * _sizeF;
    ctrlRectS           = 80 / _widthDefault * _sizeF;
    guideWidthStepSize  = 96 / _widthDefault * _sizeF;
    guideHeightStepSize = 64 / _widthDefault * _sizeF;
    lineScoreStepX      = 51 / _widthDefault * _sizeF;
    lineScoreStepY      = 5 / _widthDefault * _sizeF;
    stepBasePos         = 105 / _widthDefault * _sizeF;
    pixeShapeSize       = 1 / _widthDefault * _sizeF;
    

    synthSetting();
    maxSpeed    = 200;
    minSpeed    = 30;
    bpm         = synthMain.addParameter("tempo", 100).min(minSpeed).max(maxSpeed);
    metro       = ControlMetro().bpm(4 * bpm);
    metroOut    = synthMain.createOFEvent(metro);

    synthMain.setOutputGen(synth1 + synth2 + synth3 + synth4 + synth5 + synth6 + synth7);
    

    
    pixelStepS      = 5;
    camSize         = screenW;
    changedCamSize  = camSize / pixelStepS;  // 90
    thresholdValue  = 80;
    
    cameraScreenRatio = 1;

    
    int _x = (cam.getWidth() - screenW) * 0.5;
    
    
    index       = -1;
    noteIndex   = -1;
    
    
    
    speedCSize = ctrlRectS;
    speedCPos = ofPoint( 15 * guideWidthStepSize, ctrlPnY + ctrlPnH * 0.5 );
    bSpeedCtrl = false;
    
    thresholdCSize = ctrlRectS * 0.5;
    thresholdCPos = ofPoint( 1 * guideWidthStepSize, ctrlPnY + ctrlPnH * 0.5 );
    bthresholdCtrl = false;
    
    intervalSize = ctrlRectS * 0.5;
    intervalPos = ofPoint( 1 * guideWidthStepSize, ctrlPnY + ctrlPnH * 0.5 );
    bthresholdCtrl = false;
    intervalDist = 1;
    
    cannyThreshold1 = 120;
    cannyThreshold2 = 120;
    grayThreshold = 120;
    
    
    float _posIndexRight = 13.5;
    float _posIndexLeft = 2.5;
    base4Pos = ofPoint( guideWidthStepSize * _posIndexLeft, ctrlPnY + stepBasePos * 1 );
    base5Pos = ofPoint( guideWidthStepSize * _posIndexLeft, ctrlPnY + stepBasePos * 2.5 );
    base6Pos = ofPoint( guideWidthStepSize * _posIndexLeft, ctrlPnY + stepBasePos * 4 );
    base7Pos = ofPoint( guideWidthStepSize * _posIndexRight, ctrlPnY + stepBasePos * 1 );
    base8Pos = ofPoint( guideWidthStepSize * _posIndexRight, ctrlPnY + stepBasePos * 2.5 );
    base9Pos = ofPoint( guideWidthStepSize * _posIndexRight, ctrlPnY + stepBasePos * 4 );
    baseSize = ctrlRectS * 0.55;
    
    bPlayNote = false;
    bCameraCapturePlay = false;
    
    scaleSetting.setup();
    
    lineScoreNumber = 23;
    

    allPlayOnOff = false;

    
    melodies.resize(7);
    noteLists.resize(7);
    oldScoreNote.resize(7);

    for (int i=0; i<oldScoreNote.size(); i++) {
        oldScoreNote[i] = 0;
        noteLists[i].noteArray.push_back(0);
        melodies[i].melodyLine.push_back(0);
    }
    
    
    faceFind.setup("haarcascade_frontalface_default.xml");
    faceFind.setPreset(ObjectFinder::Fast);
    faceFind.setFindBiggestObject(true);

    debugView = true;
 
    ofSoundStreamSetup(2, 0, this, 44100, 256, 4);

    printScoreFbo.allocate(384, ofGetWidth() * 2);
    

    
    baseNum.addListener(this, &ofApp::changedBaseNum);
    bChangedBaseNum = false;

    
}





//--------------------------------------------------------------
void ofApp::update(){
    
    baseSelection = baseNum;
    
    cam.update();
    
    if(cam.isFrameNew()) {
        
        camColorCV.setFromPixels(cam.getPixels().getData(), cam.getWidth(), cam.getHeight());
        camColorCV.setROI(100, 0, 600, 600);
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
        threshold(gray, gray, thresholdF);
        //                erode(gray);
        Canny(gray, edge, cannyThreshold1, cannyThreshold2, 3);
        
        thin(edge);
        invert(edge);
        
        edge.update();


        
        
        convertColor(centerCam, downGray, CV_RGB2GRAY);
        threshold(downGray, downGray, thresholdF);
        Canny(downGray, downScaleEdge, cannyThreshold1, cannyThreshold2, 3);
        downScaleEdge.resize(150, 150);
        invert(downScaleEdge);
        downScaleEdge.update();
        
        printCam = downScaleEdge;
        printCam.resize(384, 384);
        
//        printCam.setFromPixels(downScaleEdge.getPixels().getData(), 200, 200, OF_IMAGE_GRAYSCALE);
//        //        invert(printCam);
//        rotate(printCam, printCam, -90);
//        printCam.resize(348, 348);

        
        
        
        
        
        if ( bCameraCapturePlay ) {
            noteIndex = index;
        } else {
            
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
                    
                    if ( _bCounter==0 ) {
                        blackWhitePixels _bWP;
                        _bWP.indexPos = i;
                        _bWP.pixelN = _wCounter;
                        blackPixels.push_back(_bWP);
                    }
                    _bCounter++;
                    _wCounter = 0;
                    
                } else {
                    
                    if ( _wCounter==0 ) {
                        blackWhitePixels _bWP;
                        _bWP.indexPos = i;
                        _bWP.pixelN = _bCounter;
                        whitePixels.push_back(_bWP);

                    }
                    

                    _wCounter++;
                    _bCounter = 0;
                }
            }

            
            if (whitePixels.size()%8 !=0) {
                int _restNum = whitePixels.size() % 8;
                for (int j=0; j<=(8-_restNum); j++) {
                    blackWhitePixels _bWP;
                    _bWP.indexPos = j;
                    _bWP.pixelN = _bCounter;
                    whitePixels.push_back(_bWP);
                }
            }
            

            
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
        printScoreMake();
    }
    
    
}




//--------------------------------------------------------------
void ofApp::triggerReceive(float & metro){
    
    index++;
    noteIndex = index;
    
    trigScoreNote( noteLists[0].noteArray, synth1, 1 );
    trigScoreNote( noteLists[1].noteArray, synth2, 2 );
    trigScoreNote( noteLists[2].noteArray, synth3, 3 );
    trigScoreNote( noteLists[3].noteArray, synth4, 4 );
    trigScoreNote( noteLists[4].noteArray, synth5, 5 );
    trigScoreNote( noteLists[5].noteArray, synth6, 6 );
    trigScoreNote( noteLists[6].noteArray, synth7, 7 );
    
}




//--------------------------------------------------------------
void ofApp::draw(){
    
    
    ofPushMatrix();
    
    ofTranslate((ofGetWidth() - 600) * 0.5, 0);

    
    ofPushMatrix();
    
    ofPushStyle();
    if (!bCameraCapturePlay) {
        ofSetColor( 255, 255 );
        if (edge.isAllocated()) {
            edge.draw( 0, 0, screenW, screenH);
        }
    }
    ofPopStyle();
    
    
    ofPushStyle();
    if (bCameraCapturePlay) {
        ofSetColor( 255, 255 );
        ofDrawRectangle(0, 0, screenW, screenH);
        
        ofSetColor( 255, 180 );
        bufferImg.draw( 0, 0, screenW, screenH);
    }
    ofPopStyle();
    
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
        
        for (int i=0; i<noteLists.size(); i++) {
            drawPixelAllNoteShapes( noteLists[i].noteArray, i );
        }

        for (int i=0; i<noteLists.size(); i++) {
            drawPlayingShapeNote( noteLists[i].noteArray, i );
        }
        
    }
    
    
//    drawBaseInterface();

    
    
//    ofPushStyle();
//    ofRectMode(OF_RECTMODE_CENTER);
//    ofNoFill();
//    ofSetColor(0,255,0);
//    ofDrawRectangle(faceCenter.x, faceCenter.y, 30, 30);
//    ofPopStyle();
    
    
    
    ofPopMatrix();

    
    if (debugView ) {
        debugInformation();
        gui.draw();
    }
    
    
    
    ofPushMatrix();
    
    //FIXME: 12 ????
    ofTranslate(-ofGetWidth()*0.5 + 12, ofGetHeight());
    ofRotateZ(-90);
    
    printScoreFbo.draw(0, ofGetHeight() - 384 * 0.5, 384 * 0.5, ofGetWidth());
    
    ofPopMatrix();
    
    
    
    
//    ofPushMatrix();
//    ofTranslate((ofGetWidth() - 600) * 0.5, 0);
//    drawControlElement();
//    if (bCameraCapturePlay) {
//        drawLineScore();
//    }
//    ofPopMatrix();

    
    
}



//--------------------------------------------------------------
void ofApp::drawPrintScoreFBO(){
    
    
    
    ofPushMatrix();
    //FIXME:: 40 ????
    ofTranslate(ofGetHeight() * 0.5 - 40, 0);
    
    ofSetColor(0);
    ofDrawRectangle(0, 0, printScoreFbo.getWidth(), printScoreFbo.getHeight());

    ofSetColor(255);
    ofDrawRectangle(10, 10, printScoreFbo.getWidth()-20, printScoreFbo.getHeight()-20);

    ofRotateZ(90);
    
    
    ofRectMode(OF_RECT_CENTER);
    
    float _stepLine = 12;
    float _noteSize = 4;
    
    float _noteExtraLineSize = 7;
    
    float _downBaseLine = 320;
    float _upBaseLine = _downBaseLine - _stepLine * 7;
    float _xSizeFactor = _stepLine * 5;
    
    int _melodyNoteNum = melodies[0].melodyLine.size();
    
    float _scoreXStart = 0;
    float _scoreXEnd = ofGetWidth() * 2-_xSizeFactor - (ofGetWidth() * 2 - 20.0) / _melodyNoteNum * 0.5;
    
    ofPushMatrix();
    ofTranslate( (ofGetWidth() * 2 - (_scoreXEnd - _scoreXStart)) * 0.5, 0 );
    
    
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
        
        float _xStep = (ofGetWidth() * 2 - 20.0) / _melodyNoteNum;
        
        if (j % 8 == 0) {
            float _x1 = ofMap(j, 0, _melodyNoteNum, _xSizeFactor, ofGetWidth() * 2-_xSizeFactor);
            ofDrawLine(_x1 - _xStep * 0.5, 0, _x1 - _xStep * 0.5, -_stepLine * 11);
        }
        
    }
    ofPopMatrix();
    
    
    
    ofPopStyle();
    
    ofPopMatrix();
    
    
    
    
    ofPushMatrix();
    
    ofPushStyle();
    ofTranslate(0, _upBaseLine);
    
    ofSetColor(255, 0, 0, 255);
    if (_melodyNoteNum>0) {
        int _index = noteIndex % _melodyNoteNum;
        float _x1 = ofMap(_index, 0, _melodyNoteNum, _xSizeFactor, ofGetWidth() * 2-_xSizeFactor);
        float _y1 = 0;
        ofDrawLine(_x1, _y1 + 100, _x1, _y1 - 100);
    }
    ofPopStyle();
    
    
    ofPushStyle();
    
    ofSetColor(0, 255);
    
    float _stempLength = 27;
    float _teilWidth = 2;
    float _teilLength = 10;
    float _teilEndX = _teilLength * 0.75;
    
    for (int i=0; i<melodies.size(); i++) {
        
        for (int j=0; j<melodies[i].melodyLine.size(); j++) {
            
            float _x1 = ofMap(j, 0, melodies[i].melodyLine.size(), _xSizeFactor, ofGetWidth() * 2-_xSizeFactor);
            

            if (melodies[i].melodyLine[j]>0) {
                
                //TODO: Fix Score
                if (i%3==0 || i%3==1) {
                    int _note = melodies[i].melodyLine[j] % 12;
                    int _octaveFactor = octaveScaleFactor[i];
                    int _noteOctave = (melodies[i].melodyLine[j] - _octaveFactor) / 12;
                    
                    float _posY = notePosition(_note, _stepLine);
                    float _yOutput = _posY - _noteOctave * _stepLine * 3.5;
                    
                    if (_yOutput == _stepLine) {
                        ofDrawLine(_x1-_noteExtraLineSize, _yOutput, _x1+_noteExtraLineSize, _yOutput);
                    }
                    
                    ofDrawCircle(_x1, _yOutput, _noteSize);
                    
                    ofSetLineWidth(1);
                    ofDrawLine(_x1+2, _yOutput, _x1+2, _yOutput - _stempLength);
                    
                    ofSetLineWidth(_teilWidth);
                    ofDrawLine(_x1+2, _yOutput-_stempLength, _x1+_teilEndX, _yOutput - _teilLength);
                    
                } else {
                    
                    int _note = melodies[i].melodyLine[j] % 12;
                    int _octaveFactor = octaveScaleFactor[i];
                    int _noteOctave = (melodies[i].melodyLine[j] - _octaveFactor) / 12 - 2;
                    
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
    
    
    for (int i=0; i<melodies.size(); i++) {
        melodies[i].melodyLine.clear();
    }
    
    
    for (int j=0; j<noteLists.size(); j++) {

        for (int i=1; i<noteLists[j].noteArray.size(); i++) {
            
            int _note = noteLists[j].noteArray[i];
            int _noteOld = noteLists[j].noteArray[i-1];
            
            int _outputNote;
            if ( abs(_noteOld - _note) >= intervalDist ) {
                
                if (_note>0) {
                    _outputNote = scaleSetting.noteSelector(baseSelection, j+1, _note);
                    melodies[j].melodyLine.push_back(_outputNote);
                } else {
                    melodies[j].melodyLine.push_back(0);
                }
                
            } else {
                melodies[j].melodyLine.push_back(0);
            }
            
        }

    }

    
    
}




//--------------------------------------------------------------
void ofApp::debugInformation(){
    
    ofPushMatrix();
    
    ofPushStyle();

    ofSetColor(255);
    printCam.draw(ofGetWidth()-175, 10, 150, 150);

    ofSetColor(255, 20);
    centerCam.draw(ofGetWidth()-175, 10, 150, 150);

    ofPopStyle();
    
    ofPopMatrix();
    
}



//--------------------------------------------------------------
void ofApp::drawControlElement(){
    
    
//    ofPushStyle();
    //        ofSetColor( 255 );
//    ofDrawRectangle( 0, ctrlPnY, ctrlPnW, ctrlPnH );
    //        ofSetColor( 0, 10 );
//    ofPopStyle();
//    
//    ofPushMatrix();
//    ofPushStyle();
//    
    //        ofSetColor( 0, 80 );
//
//    ofDrawLine( _speedX, ctrlPnY + _yD, _speedX, screenH - _yD);
//    
//    float _thresholdX = guideWidthStepSize * 15;
//    ofDrawLine( _thresholdX, ctrlPnY + _yD, _thresholdX, screenH - _yD);
//    
//    //    float _intervalX = guideWidthStepSize * 2.5;
//    //    ofDrawLine( _intervalX, ctrlPnY + _yD, _intervalX, screenH - _yD);
//    
//    ofPopStyle();
//    ofPopMatrix();
//    
//    ofPushStyle();
//        ofSetColor( 0, _alpha );
//    ofSetCircleResolution(48);
//    float _sX = speedCPos.x;
//    float _sY = speedCPos.y;
//    ofNoFill();
//    ofDrawCircle( _sX, _sY, speedCSize * 0.5 );
//    ofPopStyle();
    
    //    ofPushStyle();
    //    ofSetColor( 255, _alpha );
    //    ofNoFill();
    //    float _sizeF = 1.1;
    //    float _x1 = thresholdCPos.x;
    //    float _y1 = thresholdCPos.y - thresholdCSize * _sizeF;
    //    float _x2 = thresholdCPos.x - cos(ofDegToRad(30)) * thresholdCSize * _sizeF;
    //    float _y2 = thresholdCPos.y + sin(ofDegToRad(30)) * thresholdCSize * _sizeF;
    //    float _x3 = thresholdCPos.x + cos(ofDegToRad(30)) * thresholdCSize * _sizeF;
    //    float _y3 = thresholdCPos.y + sin(ofDegToRad(30)) * thresholdCSize * _sizeF;
    //    ofDrawTriangle( _x1, _y1, _x2, _y2, _x3, _y3 );
    //    ofPopStyle();
    
    int _alpha = 180;
    float _speedX = guideWidthStepSize;
    float _yD = 20;

//    ofPushStyle();
    //        ofSetColor( 0, _alpha );
//    float _iX = intervalPos.x;
//    float _iY = intervalPos.y;
//    ofDrawLine( _iX - intervalSize, _iY, _iX, _iY + intervalSize );
//    ofDrawLine( _iX, _iY - intervalSize, _iX + intervalSize, _iY );
//    ofDrawLine( _iX + intervalSize, _iY, _iX, _iY + intervalSize );
//    ofDrawLine( _iX, _iY - intervalSize, _iX - intervalSize, _iY );
//    ofPopStyle();
    
    
    ofPushMatrix();
    ofPushStyle();

    ofSetColor( 0, 160 );
    
    int _xDefaultPos = lineScoreStepX * (lineScoreNumber-1);
    
    float _xL1 = ctrlPnW * 0.5 - _xDefaultPos * 0.5;
    ofDrawLine( _xL1, ctrlPnY + _yD, _xL1, ofGetHeight() - _yD);
    
    float _xL2 = ctrlPnW * 0.5 + _xDefaultPos * 0.5;
    ofDrawLine( _xL2, ctrlPnY + _yD, _xL2, ofGetHeight() - _yD);
    
    float _xM = ctrlPnW * 0.5;
    
    ofSetColor( 0, 40 );

    ofDrawLine( _xM, ctrlPnY + _yD, _xM, screenH - _yD);
    
    ofPopStyle();
    ofPopMatrix();
    
    
}




//--------------------------------------------------------------
void ofApp::drawTrianglePixel(){
    
    int _pixelSize = pixelCircleSize;  // 10
    float _ellipseSizeR = 1.7;
    
    ofPushMatrix();
    ofPushStyle();
    ofEnableAntiAliasing();
    
    if (whitePixels.size() > 1) {
        for (int i=0; i<whitePixels.size(); i++) {
            
            int _noteLoopIndex = ((i) % (whitePixels.size()-1))+1;
            int _pixelNumbers = whitePixels[ _noteLoopIndex ].pixelN;
            int _indexPixes = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
            
            float _x = ((_indexPixes) % changedCamSize) * pixelStepS * cameraScreenRatio - _pixelSize;
            float _y = (int)((_indexPixes) / changedCamSize) * pixelStepS * cameraScreenRatio;
            
            ofPoint _1P = ofPoint( _x, _y - _pixelSize * _ellipseSizeR * 0.75 );
            ofPoint _2P = ofPoint( _x - _pixelSize * _ellipseSizeR * 0.55, _y + _pixelSize * _ellipseSizeR * 0.25 );
            ofPoint _3P = ofPoint( _x + _pixelSize * _ellipseSizeR * 0.55, _y + _pixelSize * _ellipseSizeR * 0.25 );
            
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
        int _indexPixes = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
        
        float _x = ((_indexPixes) % changedCamSize) * pixelStepS * cameraScreenRatio;
        float _y = (int)((_indexPixes) / changedCamSize) * pixelStepS * cameraScreenRatio;
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
        int _indexPixes = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
        
        float _x = ((_indexPixes) % changedCamSize) * pixelStepS * cameraScreenRatio;
        float _y = (int)((_indexPixes) / changedCamSize) * pixelStepS * cameraScreenRatio;
        ofPoint _p = ofPoint( _x, _y );
        
        
        int _indexLoopLine = ((i) % (whitePixels.size()-1)) + 1;
        int _indexLoopLineOld = ((i + 1) % (whitePixels.size()-1)) + 1;
        
        int _note = _vNote[_indexLoopLine];
        int _noteOld = _vNote[_indexLoopLineOld];
        
        int _noteScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
        int _noteOldScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteOld);
        
        
        if ( abs(_noteOldScaled-_noteScaled) >= intervalDist ) {
            if (_note>0) {
                float _size = _noteScaled * pixeShapeSize;
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
        int _indexPixes = whitePixels[_noteLoopIndex].indexPos - _pixelNumbers;
        
        for (int i=0; i<_pixelNumbers; i++){
            
            float _xS = ((_indexPixes+i) % changedCamSize) * pixelStepS * cameraScreenRatio;
            float _yS = (int)((_indexPixes+i) / changedCamSize) * pixelStepS * cameraScreenRatio;
            
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
        int _indexPixes = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
        
        float _x = ((_indexPixes) % changedCamSize) * pixelStepS * cameraScreenRatio;
        float _y = (int)((_indexPixes) / changedCamSize) * pixelStepS * cameraScreenRatio;
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
    ofColor _c = ofColor::fromHsb( _h, 180, 255, 180 );
    
    if (whitePixels.size()>0) {
        
        int _noteLoopIndex = ((noteIndex) % (whitePixels.size()-1))+1;
        int _pixelNumbers = whitePixels[ _noteLoopIndex ].pixelN;
        int _indexPixes = whitePixels[ _noteLoopIndex ].indexPos - _pixelNumbers;
        
        float _x = ((_indexPixes) % changedCamSize) * pixelStepS * cameraScreenRatio;
        float _y = (int)((_indexPixes) / changedCamSize) * pixelStepS * cameraScreenRatio;
        ofPoint _p = ofPoint( _x, _y );
        
        int _indexLoopLineOld = ((1 + noteIndex) % (whitePixels.size()-1)) + 1;
        
        int _note = _vNote[_noteLoopIndex];
        int _noteOld = _vNote[_indexLoopLineOld];
        
        int _noteScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
        int _noteOldScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteOld);
        
        if ( abs(_noteOldScaled-_noteScaled) >= intervalDist ) {
            if (_note>0) {
                //                drawShapeCeterLine( _p, baseSelection, _pixelNumbers);
                
                float _size = _noteScaled * pixeShapeSize;
                drawShapeCeterLineColorRotation( _p, baseSelection, _size, _c );
            }
        }
        
    }
    
    ofPopStyle();
    ofPopMatrix();
    
}


//--------------------------------------------------------------
void ofApp::drawLineScore(){
    
    int _xNumber = lineScoreNumber;
    int _stepX = lineScoreStepX;
    int _stepY = lineScoreStepY;
    int _defaultNote = 56;
    int _xDefaultPos = _stepX * (_xNumber-1);
    
    
    ofPushMatrix();
    ofTranslate( ctrlPnW * 0.5 - _xDefaultPos * 0.5, ctrlPnY + 127 * _stepY - _defaultNote );
    
    ofPushStyle();

    ofSetColor( 0, 120 );

    
    for (int i=0; i<noteLists.size(); i++) {
        drawScoreCircleLine( noteLists[i].noteArray, i );
    }
    
    ofPopStyle();
    ofPopMatrix();
    
}


//--------------------------------------------------------------
void ofApp::drawScoreCircleLine( vector<int> _vNote, int _scoreCh ){
    
    
    int _xNumber = lineScoreNumber;
    int _middle = _xNumber * 0.5;
    int _stepX = lineScoreStepX;
    int _stepY = lineScoreStepY;
    int _defaultNote = 56;
    int _size = 3;
    int _xDefaultPos = _stepX * (_xNumber-1);
    
    vector<int> _scoreNote = _vNote;
    
    float _h = ofMap( _scoreCh, 1, 7, 0, 255 );
    ofColor _c = ofColor::fromHsb( _h, 180, 255, 180 );
    
    
    if (_scoreNote.size()>0) {
        
        ofPushStyle();
        
        for (int i=0; i<_xNumber; i++){
            
            int _indexLoopLine = ((i + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            int _indexLoopLineOld = ((i + 1 + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            
            int _note = _scoreNote[_indexLoopLine];
            int _noteOld = _scoreNote[_indexLoopLineOld];
            
            int _noteScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
            int _noteOldScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteOld);
            
            float _x1 = _xDefaultPos - i * _stepX;
            float _y1 = _defaultNote - _noteScaled * _stepY;
            
            if ( abs(_noteOldScaled-_noteScaled) >= intervalDist ) {
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
            
            int _indexLoopLineS = ((i + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            int _indexLoopLineE = ((i + 1 + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            
            int _indexLoopLineEOld = ((i + 2 + noteIndex - _middle) % (whitePixels.size()-1)) + 1;
            
            int _noteS = _scoreNote[_indexLoopLineS];
            int _noteE = _scoreNote[_indexLoopLineE];
            int _noteEOld = _scoreNote[_indexLoopLineEOld];
            
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
void ofApp::controlGuide(){
    
    
    
}



//--------------------------------------------------------------
void ofApp::drawBaseInterface(){
    
    ofPushMatrix();
    ofPushStyle();
    
    ofColor _c[6];
    
    for (int i=0; i<6; i++) {
        if ( baseSelection == (4 + i) ) {
            _c[i] = ofColor::fromHsb( i*40, 180, 180 );
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
void ofApp::drawShapeCeterLine(ofPoint pos, int base, int size, ofColor _c){
    
    ofPoint _pos = pos;
    
    vector<ofPoint> posLine;
    
    int _base = base;
    int _size = size;
    
    for (int i=0; i<_base; i++) {
        float _sizeDegree = i * 360 / _base + 180.0;
        float _x = sin(ofDegToRad( _sizeDegree )) * _size;
        float _y = cos(ofDegToRad( _sizeDegree )) * _size;
        
        ofPoint _p = ofPoint( _x, _y );
        posLine.push_back( _p );
    }
    
    
    ofPushMatrix();
    ofPushStyle();
    
    ofTranslate( _pos );
    
    ofSetColor( _c, 60 );

    for (int i=0; i<posLine.size(); i++){
        ofDrawLine( 0, 0, posLine[i].x, posLine[i].y );
    }
    
    ofSetColor( _c, 180 );

    for (int i=0; i<posLine.size()-1; i++){
        ofDrawLine( posLine[i].x, posLine[i].y, posLine[i+1].x, posLine[i+1].y );
    }
    ofDrawLine( posLine[0].x, posLine[0].y, posLine[posLine.size()-1].x, posLine[posLine.size()-1].y );
    
    ofPopMatrix();
    ofPopStyle();
    
}


//--------------------------------------------------------------
void ofApp::drawShapeCeterLineColorRotation(ofPoint pos, int base, int size, ofColor color){
    
    ofPoint _pos = pos;
    
    vector<ofPoint> posLine;
    
    int _base = base;
    int _size = size;
    
    for (int i=0; i<_base; i++) {
        float _sizeDegree = i * 360 / _base + 180.0;
        float _x = sin(ofDegToRad( _sizeDegree )) * _size;
        float _y = cos(ofDegToRad( _sizeDegree )) * _size;
        
        ofPoint _p = ofPoint( _x, _y );
        posLine.push_back( _p );
    }
    
    
    ofPushMatrix();
    ofPushStyle();
    
    
    ofTranslate( _pos );
    ofRotateZ( 45 );
    
    ofSetLineWidth( 3 );
    
    ofSetColor( color.r, color.g, color.b, color.a * 0.2 );
    for (int i=0; i<posLine.size(); i++){
        ofDrawLine( 0, 0, posLine[i].x, posLine[i].y );
    }
    
    ofSetColor( color );
    for (int i=0; i<posLine.size()-1; i++){
        ofDrawLine( posLine[i].x, posLine[i].y, posLine[i+1].x, posLine[i+1].y );
    }
    ofDrawLine( posLine[0].x, posLine[0].y, posLine[posLine.size()-1].x, posLine[posLine.size()-1].y );
    
    ofPopMatrix();
    ofPopStyle();
    
}




//--------------------------------------------------------------
void ofApp::drawShape(ofPoint pos, int base, int size){
    
    ofPoint _pos = pos;
    
    vector<ofPoint> posLine;
    
    int _base = base;
    int _size = size;
    
    for (int i=0; i<_base; i++) {
        float _sizeDegree = i * 360 / _base + 180.0;
        float _x = sin(ofDegToRad( _sizeDegree )) * _size;
        float _y = cos(ofDegToRad( _sizeDegree )) * _size;
        
        ofPoint _p = ofPoint( _x, _y );
        posLine.push_back( _p );
    }
    
    
    ofPushMatrix();
    
    ofTranslate( _pos );
    
    for (int i=0; i<posLine.size()-1; i++){
        ofDrawLine( posLine[i].x, posLine[i].y, posLine[i+1].x, posLine[i+1].y );
    }
    ofDrawLine( posLine[0].x, posLine[0].y, posLine[posLine.size()-1].x, posLine[posLine.size()-1].y );
    
    ofPopMatrix();
    
}



//--------------------------------------------------------------
void ofApp::debugControlPDraw(){
    
    ofPushMatrix();
    ofPushStyle();

    ofSetColor( 0 );

    for (int i=0; i<15; i++){
        float _x1 = i * guideWidthStepSize + guideWidthStepSize;
        ofDrawLine( _x1, ctrlPnY, _x1, screenH );
    }
    
    for (int j=0; j<7; j++){
        float _y1 = j * guideHeightStepSize + guideHeightStepSize;
        ofDrawLine( 0, _y1 + ctrlPnY, screenW, _y1 + ctrlPnY );
    }
    
    ofPopStyle();
    ofPopMatrix();
    
    ofPushStyle();
    ofSetColor(0);
    ofDrawBitmapString( ofToString(ofGetFrameRate(),2), 10, screenH-10 );
    ofPopStyle();
    
}




void ofApp::audioRequested (float * output, int bufferSize, int nChannels){
    
    synthMain.fillBufferOfFloats(output, bufferSize, nChannels);
    
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    
    if (key == ' ') {
        
        allPlayOnOff = !allPlayOnOff;
        
        if ((whitePixels.size()>2)) {
            bCameraCapturePlay = !bCameraCapturePlay;
            //            blur(edge, 3);
            bufferImg = edge;
            
            if ( !bCameraCapturePlay ) {
                index = -1;
                ofRemoveListener(* metroOut, this, &ofApp::triggerReceive);
            } else {
                scoreMake();
                printScoreMake();
                //                noteIndex = index;
                ofAddListener(* metroOut, this, &ofApp::triggerReceive);
                bPlayNote = true;
            }
            
            grayThreshold = 120;
            touchDownDefault = 0;
        }
        
        
    } else if (key == 'g') {
    
        debugView = !debugView;

    }  else if (key == 's') {

        printScore();

    } else if (key == 'e') {
    
        printer.close();
        printer.open("/dev/cu.usbserial-A900adIr");

    } else if (key == 'i') {
    
        printCamView();
        
    } else if (key == 'h') {
    
        printHeader();

    } else if (key == 'f') {
    
        printFooter();

    }
    

    
}





//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
//    ofPoint _changedTouch = ofPoint(x, y);
//    
//    if (bSpeedCtrl) {
//        float _minY = ctrlPnY + speedCSize * 0.75;
//        float _maxY = screenH - speedCSize * 0.75;
//        
//        if ( (_changedTouch.y>_minY) && (_changedTouch.y<_maxY) && _changedTouch.x>speedCPos.x - (ctrlPnW-speedCPos.x) ) {
//            speedCPos.y = _changedTouch.y;
//            float _tempo = ofMap( speedCPos.y, _minY, _maxY, maxSpeed, minSpeed );
//            synthMain.setParameter("tempo", _tempo);
//        }
//        
//    }
//    
//    //        if (bthresholdCtrl) {
//    //            float _minY = ctrlPnY + speedCSize * 0.75;
//    //            float _maxY = screenH - speedCSize * 0.75;
//    //
//    //            if ((_changedTouch.y>_minY)&&(_changedTouch.y<_maxY)) {
//    //                thresholdCPos.y = _changedTouch.y;
//    //                float _threshold = ofMap(thresholdCPos.y, _minY, _maxY, 255, 0);
//    //                grayThreshold = _threshold;
//    //            }
//    //        }
//    
//    
//    if (bInterval) {
//        float _minY = ctrlPnY + speedCSize * 0.75;
//        float _maxY = screenH - speedCSize * 0.75;
//        if ((_changedTouch.y>_minY)&&(_changedTouch.y<_maxY) && _changedTouch.x<intervalPos.x * 2 ) {
//            intervalPos.y = _changedTouch.y;
//            float _interval = ofMap(intervalPos.y, _minY, _maxY, 0, 20);
//            intervalDist = _interval;
//        }
//    }
//    
//    
//    
////    if ( touch.id==0 ) {
//    
//        //        if (bSpeedCtrl) {
//        //            float _minY = ctrlPnY + speedCSize * 0.75;
//        //            float _maxY = screenH - speedCSize * 0.75;
//        //
//        //            if ((_changedTouch.y>_minY)&&(_changedTouch.y<_maxY)) {
//        //                speedCPos.y = _changedTouch.y;
//        //                float _tempo = ofMap( speedCPos.y, _minY, _maxY, maxSpeed, minSpeed );
//        //                synthMain.setParameter("tempo", _tempo);
//        //            }
//        //
//        //        }
//        //
//        ////        if (bthresholdCtrl) {
//        ////            float _minY = ctrlPnY + speedCSize * 0.75;
//        ////            float _maxY = screenH - speedCSize * 0.75;
//        ////
//        ////            if ((_changedTouch.y>_minY)&&(_changedTouch.y<_maxY)) {
//        ////                thresholdCPos.y = _changedTouch.y;
//        ////                float _threshold = ofMap(thresholdCPos.y, _minY, _maxY, 255, 0);
//        ////                grayThreshold = _threshold;
//        ////            }
//        ////        }
//        //
//        //
//        //        if (bInterval) {
//        //            float _minY = ctrlPnY + speedCSize * 0.75;
//        //            float _maxY = screenH - speedCSize * 0.75;
//        //            if ((_changedTouch.y>_minY)&&(_changedTouch.y<_maxY)) {
//        //                intervalPos.y = _changedTouch.y;
//        //                float _interval = ofMap(intervalPos.y, _minY, _maxY, 0, 20);
//        //                intervalDist = _interval;
//        //            }
//        //        }
//        
//        
//        
////    }
//    
//    if ( (_changedTouch.x>0)&&(_changedTouch.x<ctrlPnW) && (_changedTouch.y<ctrlPnY)&&(_changedTouch.y>0) ) {
//        
//        grayThreshold = 120 + (_changedTouch.y - touchDownDefault);
//        
//    }
//    
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
//    float _tolerance = 3;
//    
//    ofPoint _changedTouch = ofPoint(x, y);
//    
//    
//    float _distS = ofDist( speedCPos.x, speedCPos.y , _changedTouch.x, _changedTouch.y );
//    
//    for (int i=0; i<2; i++) {
//        
//        float _distS = ofDist( speedCPos.x, speedCPos.y , _changedTouch.x, _changedTouch.y );
//        if ( (_distS < thresholdCSize * _tolerance) && bSpeedCtrl == false) {
//            bSpeedCtrl = true;
//        }
//        
//    }
//    
//    
//    float _distI = ofDist( intervalPos.x, intervalPos.y , _changedTouch.x, _changedTouch.y );
//    
//    for (int i=0; i<2; i++) {
//        float _distI = ofDist( intervalPos.x, intervalPos.y , _changedTouch.x, _changedTouch.y );
//        if ( (_distI < intervalSize * _tolerance) && bInterval == false) {
//            bInterval = true;
//        }
//        
//    }
//    
//    
//    
//    
////    if ( touch.id==0 ) {
//    
//        //        float _distS = ofDist( speedCPos.x, speedCPos.y , _changedTouch.x, _changedTouch.y );
//        //
//        //        if (_distS < thresholdCSize * _tolerance) {
//        //            bSpeedCtrl = true;
//        //        } else {
//        //            bSpeedCtrl = false;
//        //        }
//        
//        //        float _distT = ofDist( thresholdCPos.x, thresholdCPos.y , _changedTouch.x, _changedTouch.y );
//        
//        //        if (_distT < thresholdCSize * _tolerance) {
//        //            bthresholdCtrl = true;
//        //        } else {
//        //            bthresholdCtrl = false;
//        //        }
//        
//        //        float _distI = ofDist( intervalPos.x, intervalPos.y , _changedTouch.x, _changedTouch.y );
//        //
//        //        if (_distI < intervalSize * _tolerance) {
//        //            bInterval = true;
//        //        } else {
//        //            bInterval = false;
//        //        }
//    
//    
////    }
//    
//    if ( (_changedTouch.x>0)&&(_changedTouch.x<ctrlPnW) && (_changedTouch.y<ctrlPnY)&&(_changedTouch.y>0) ) {
//        
//        grayThreshold = 120;
//        touchDownDefault = _changedTouch.y;
//        
//    }
//
//    
//    float _4BaseDist = ofDist( _changedTouch.x, _changedTouch.y, base4Pos.x, base4Pos.y );
//    if ( _4BaseDist < baseSize ) {
//        //        index = 0;
//        baseSelection = 4;
//    }
//    
//    float _5BaseDist = ofDist( _changedTouch.x, _changedTouch.y, base5Pos.x, base5Pos.y );
//    if ( _5BaseDist < baseSize ) {
//        //        index = 0;
//        baseSelection = 5;
//    }
//    
//    float _6BaseDist = ofDist( _changedTouch.x, _changedTouch.y, base6Pos.x, base6Pos.y );
//    if ( _6BaseDist < baseSize ) {
//        //        index = 0;
//        baseSelection = 6;
//    }
//    
//    float _7BaseDist = ofDist( _changedTouch.x, _changedTouch.y, base7Pos.x, base7Pos.y );
//    if ( _7BaseDist < baseSize ) {
//        //        index = 0;
//        baseSelection = 7;
//    }
//    
//    float _8BaseDist = ofDist( _changedTouch.x, _changedTouch.y, base8Pos.x, base8Pos.y );
//    if ( _8BaseDist < baseSize ) {
//        //        index = 0;
//        baseSelection = 8;
//    }
//    
//    float _9BaseDist = ofDist( _changedTouch.x, _changedTouch.y, base9Pos.x, base9Pos.y );
//    if ( _9BaseDist < baseSize ) {
//        //        index = 0;
//        baseSelection = 9;
//    }
    
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
 
//    ofPoint _changedTouch = ofPoint(x, y);
//    
//    if ( (_changedTouch.x>0)&&(_changedTouch.x<ctrlPnW) && (_changedTouch.y<ctrlPnY)&&(_changedTouch.y>0) ) {
//        if ((whitePixels.size()>2)) {
//            bCameraCapturePlay = !bCameraCapturePlay;
//            //            blur(edge, 3);
//            bufferImg = edge;
//            
//            if ( !bCameraCapturePlay ) {
//                index = 0;
//                ofRemoveListener(* metroOut, this, &ofApp::triggerReceive);
//            } else {
//                scoreMake();
//                //                noteIndex = index;
//                ofAddListener(* metroOut, this, &ofApp::triggerReceive);
//                bPlayNote = true;
//            }
//            
//            grayThreshold = 120;
//            touchDownDefault = 0;
//        }
//        
//    }
//    
//    
//    if ( (_changedTouch.x<guideWidthStepSize * 11)&&(_changedTouch.x>guideWidthStepSize * 4) && (_changedTouch.y>ctrlPnY)&&(_changedTouch.y<screenH) && bCameraCapturePlay ) {
//        
//        bPlayNote = !bPlayNote;
//        
//        if ( !bPlayNote ) {
//            ofRemoveListener(* metroOut, this, &ofApp::triggerReceive);
//        } else {
//            ofAddListener(* metroOut, this, &ofApp::triggerReceive);
//        }
//        
//    }
//    
//    float _tolerance = 2;
//    
//    float _distS = ofDist( speedCPos.x, speedCPos.y , _changedTouch.x, _changedTouch.y );
//    if (_distS < (thresholdCSize * _tolerance) && bSpeedCtrl==true) {
//        bSpeedCtrl = false;
//    }
//    
//    float _distI = ofDist( intervalPos.x, intervalPos.y , _changedTouch.x, _changedTouch.y );
//    if (_distI < (intervalSize * _tolerance) && bInterval == true) {
//        bInterval = false;
//    }
    
    
    
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
void ofApp::printScore(){
    
    printer.println("------------------------");
    printer.println(" ");
    ofPixels _p;
    printScoreFbo.readToPixels(_p);
    ofImage _image;
    float _w = printScoreFbo.getWidth();
    float _h = printScoreFbo.getHeight();
    _image.setFromPixels(_p.getData(), _w, _h, OF_IMAGE_COLOR_ALPHA);
    printer.print(_image, 100);
    
}


//--------------------------------------------------------------
void ofApp::printCamView(){
    
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
    
    
    for (int i=0; i<noteLists.size(); i++) {
        noteLists[i].noteArray.clear();
    }
    
    int _intervalDist = 1;
    
    for (int i=0; i<whitePixels.size(); i++) {
        
        
        vector<int> _bitNumber;
        _bitNumber.resize(7);
        
        int _indexLoop = ((i) % (whitePixels.size()-1))+1;
        int _pixelNumbers = whitePixels[ _indexLoop ].pixelN;
        _bitNumber = convertDecimalToNBase( _pixelNumbers, baseSelection, (int)_bitNumber.size() );
        
        
        for (int j=0; j<noteLists.size(); j++) {
            
            if (abs(_bitNumber[j] - oldScoreNote[j]) >= _intervalDist) {
                noteLists[j].noteArray.push_back(_bitNumber[j]);
            } else {
                noteLists[j].noteArray.push_back(-1);
            }
            oldScoreNote[j] = _bitNumber[j];
            
        }
        
        
    }
    
    
    
}





//--------------------------------------------------------------
void ofApp::trigScoreNote( vector<int> _vNote, ofxTonicSynth _synthIn, int _scoreCh ){
    
    int _indexLoop = ((noteIndex) % (whitePixels.size()-1))+1;
    int _indexLoopOld = ((noteIndex + 1) % (whitePixels.size()-1))+1;
    
    
    vector<int> _scoreNote = _vNote;
    ofxTonicSynth _synth = _synthIn;
    
    int _note = _scoreNote[_indexLoop];
    int _noteOld = _scoreNote[_indexLoopOld];
    
    int _noteScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
    int _noteOldScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteOld);
    
    
    string tName = "trigger" + ofToString(_scoreCh);
    string tPitch = "carrierPitch" + ofToString(_scoreCh);
    
    
    
    if ( abs(_noteOldScaled - _noteScaled) >= intervalDist ) {
        if (_note>0) {
            
            int _noteScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
            
            _synth.setParameter( tName, 1);
            _synth.setParameter( tPitch, _noteScaled);
            
        }
    }
    

}



//--------------------------------------------------------------
void ofApp::checkSameNote( vector<int> _vNote, ofxTonicSynth _synthIn, int _scoreCh ){
    
    int _indexLoop = ((noteIndex) % (whitePixels.size()-1))+1;
    int _indexLoopOld = ((noteIndex + 1) % (whitePixels.size()-1))+1;
    
    
    vector<int> _scoreNote = _vNote;
    ofxTonicSynth _synth = _synthIn;
    
    int _note = _scoreNote[_indexLoop];
    int _noteOld = _scoreNote[_indexLoopOld];
    
    int _noteScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
    int _noteOldScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _noteOld);
    
    
    string tName = "trigger" + ofToString(_scoreCh);
    string tPitch = "carrierPitch" + ofToString(_scoreCh);
    
    
    
    if ( abs(_noteOldScaled - _noteScaled) >= intervalDist ) {
        if (_note>0) {
            
            int _noteScaled = scaleSetting.noteSelector(baseSelection, _scoreCh, _note);
            
            _synth.setParameter( tName, 1);
            _synth.setParameter( tPitch, _noteScaled);
            
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
    parameters.add(thresholdF.set("Threshold", 120, 0, 255));
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

