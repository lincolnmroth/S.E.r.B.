//imports
#include "core.hpp"
#include "imgcodecs.hpp"
#include "imgproc.hpp"
#include "highgui.hpp"
#include "aruco.hpp"
#include "calib3d.hpp"
#include "opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>

//name spaces
using namespace std;
using namespace cv;

//function headers
void detectAndDisplay(Mat frame);

//global variables for Aruco detection
const float calibrationSquareDimension = 0.0245f; //meters
const float arucoSquareDimension = 0.025f; //meters  0.025f for smaller square
const Size chessboardDimensions = Size(6,9);//why is it 6x9 and not 7x10
bool idDetected = false;
const int correctID = 1; //ID of aruco board
float midpoint; //x coord of aruco board
double midpointface; //midpoint between eyes
bool safetoride=false;

//global variables for facial detection
string face_cascade_name = "haarcascade_frontalface_alt.xml";
string eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

void createArucoMarkers(){ //creates 50 4x4 aruco boards
    Mat outputMarker;

    Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::DICT_4X4_50);

    for(int i=0;i<50;i++){
        aruco::drawMarker(markerDictionary,i,500,outputMarker,1);
        ostringstream convert;
        String imageName="4x4Marker_";
        convert<<imageName<<i<<".jpg";
        imwrite(convert.str(),outputMarker);
    }
}

void createKnownBoardPosition(Size boardSize, float squareEdgeLength, vector<Point3f>& corners){
    for(int i=0;i<boardSize.height;i++){
        for(int j=0;j<boardSize.width;j++){
            corners.push_back(Point3f(j*squareEdgeLength,i*squareEdgeLength,0.0f));
        }
    }
}

void getChessboardCorners(vector<Mat> images, vector<vector<Point2f> >& allFoundCorners, bool showResults=false){
    for(vector<Mat>::iterator iter=images.begin();iter!=images.end();iter++){
        vector<Point2f> pointBuf;
        bool found=findChessboardCorners(*iter,Size(6,9),pointBuf,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);

        if(found){
            allFoundCorners.push_back(pointBuf);
        }

        if(showResults){
            drawChessboardCorners(*iter,Size(6,9),pointBuf,found);
            imshow("Looking for corners",*iter);
            waitKey(0);
        }
    }
}

double startWebcamMonitor(const Mat& cameraMatrix, const Mat& distanceCoefficients, float arucoSquareDimension, Mat frame, VideoCapture vid){ //detects aruco board then location of face
    vector<int> markerIds;
    vector<vector<Point2f> > markerCorners, rejectedCandidates;
    aruco::DetectorParameters parameters;

    Ptr<aruco::Dictionary> markerDictionary = aruco::getPredefinedDictionary(aruco::DICT_4X4_50);

    if(!vid.isOpened()){
        cout << "vid not opened" << endl;
        return -1;
    }

    namedWindow("Webcam - startWebcamMonitor",CV_WINDOW_AUTOSIZE);

    vector<Vec3d> rotationVectors, translationVectors;

    while(true){
        if(!vid.read(frame)){
            cout << "can't read frame" << endl;
            break;
        }
        aruco::detectMarkers(frame, markerDictionary, markerCorners, markerIds);
        aruco::estimatePoseSingleMarkers(markerCorners, arucoSquareDimension, cameraMatrix, distanceCoefficients, rotationVectors, translationVectors);
        if(markerIds.size()>0){
            aruco::drawDetectedMarkers(frame, markerCorners, markerIds);
        }
        for(int i=0;i<markerIds.size();i++){
            if(markerIds[i]==correctID){
                idDetected = true;
            }
            aruco::drawAxis(frame, cameraMatrix,distanceCoefficients, rotationVectors[i], translationVectors[i], 0.1f);
        }
        imshow("Webcam - startWebcamMonitor", frame);
        waitKey(30);

        if(idDetected==true){
            cout << "Correct ID detected: " << correctID << endl;

            midpoint = ((markerCorners.back().at(0) + markerCorners.back().at(3))/2.0).x;

            //First, load the cascades
            if( !face_cascade.load( face_cascade_name ) ){ printf("Error Loading face_cascade_name\n"); return -1; };
            if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("Error Loading eyes_cascade_name\n"); return -1; };

            //Second, read the video stream
                while( true ){
                    if(!vid.read(frame)){
                        cout << "can't read frame" << endl;
                        break;
                    }

            //Third, apply the classifier to the frame
                    if( !frame.empty()){detectAndDisplay( frame );}
                    else{ printf("Frame not captured"); break; }

            //'c' exits program and 'd' re-runs program w/o having to exit
                    int c = waitKey(10);
                    int d = waitKey(10);
                    if( (char)c == 'c' ) { break; }
                    if( (char)d == 'd' ) { startWebcamMonitor(cameraMatrix, distanceCoefficients, arucoSquareDimension,frame,vid); }
                    if(safetoride==true){break;}
                }

            return ((markerCorners.back().at(0) + markerCorners.back().at(3))/2.0).x;
        }
        char key = waitKey(1000/30);
        if(key==27){
            cout << "Escape key detected...Exiting program" << endl;
            break;
        }
    }
    return 1;
}

void cameraCalibration(vector<Mat> calibrationImages, Size boardSize, float squareEdgeLength,Mat& cameraMatrix, Mat& distanceCoefficients){
    vector<vector<Point2f> > checkerboardImageSpacePoints;
    getChessboardCorners(calibrationImages, checkerboardImageSpacePoints,false);

    vector<vector<Point3f> > worldSpaceCornerPoints(1);

    createKnownBoardPosition(boardSize,squareEdgeLength,worldSpaceCornerPoints[0]);
    worldSpaceCornerPoints.resize(checkerboardImageSpacePoints.size(),worldSpaceCornerPoints[0]);

    vector<Mat> rVectors, tVectors;
    distanceCoefficients=Mat::zeros(8,1,CV_64F);

    calibrateCamera(worldSpaceCornerPoints, checkerboardImageSpacePoints,boardSize,cameraMatrix,distanceCoefficients,rVectors,tVectors);
}

bool saveCameraCalibration(string name, Mat cameraMatrix, Mat distanceCoefficients){
    ofstream outStream(name.c_str());
    if(outStream){
        uint16_t rows = cameraMatrix.rows;
        uint16_t columns = cameraMatrix.cols;

        outStream << rows <<endl;
        outStream << columns <<endl;

        for(int r=0;r<rows;r++){
            for(int c=0;c<columns;c++){
                double value=cameraMatrix.at<double>(r,c);
                outStream << value << endl;
            }
        }

        rows = distanceCoefficients.rows;
        columns = distanceCoefficients.cols;

        outStream << rows <<endl;
        outStream << columns <<endl;

        for(int r=0;r<rows;r++){
            for(int c=0;c<columns;c++){
                double value=distanceCoefficients.at<double>(r,c);
                outStream << value << endl;
            }
        }

        outStream.close();
        return true;

    }

    return false;
}

bool loadCameraCalibration(string name, Mat& cameraMatrix, Mat& distanceCoefficients){
    ifstream inStream(name.c_str());
    if(inStream){
        uint16_t rows;
        uint16_t columns;

        inStream >> rows;
        inStream >> columns;

        cameraMatrix = Mat(Size(rows, columns), CV_64F);

        for(int r=0;r<rows;r++){
            for(int c=0;c<columns;c++){
                double read=0.0;
                inStream >> read;
                cameraMatrix.at<double>(r,c)= read;
            }
        }

        //Distance Coefficients
        inStream >> rows;
        inStream >> columns;

        distanceCoefficients = Mat::zeros(rows,columns,CV_64F);

        for(int r=0;r<rows;r++){
            for(int c=0;c<columns;c++){
                double read = 0.0;
                inStream >> read;
                distanceCoefficients.at<double>(r,c)=read;
            }
        }

       inStream.close();
       return true;
    }
    return false;
}

void cameraCalibrationProcess(Mat& cameraMatrix, Mat& distanceCoefficients){
    Mat frame;
    Mat drawToFrame;

    vector<Mat> savedImages;

    vector<vector<Point2f> > markerCorners,rejectedCandidates;

    VideoCapture vid(0);

    if(!vid.isOpened()){
        cout<<"video not opened"<<endl;
    }

    int framesPerSecond=20;

    namedWindow("Webcam",CV_WINDOW_AUTOSIZE);
    int savedimagecounter=0;
    while(true){
        if(!vid.read(frame)){
            cout<<"no webcam found"<<endl;
            break;
        }
        vector<Vec2f> foundPoints;
        bool found=false;

        found=findChessboardCorners(frame,chessboardDimensions,foundPoints,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE);
        frame.copyTo(drawToFrame);
        drawChessboardCorners(drawToFrame,chessboardDimensions,foundPoints,found);
        if(found){
            imshow("Webcam",drawToFrame);
        }
        else{
            imshow("Webcam",frame);
        }
        char character = waitKey(1000/framesPerSecond);

        switch(character){
        case ' '://space key
            //saving image
            if(found){
                Mat temp;
                frame.copyTo(temp);
                savedImages.push_back(temp);
                cout <<"image saved"<<savedimagecounter<<endl;
                savedimagecounter++;
            }
            break;
        case 13://enter key
            //start calibration
            if(savedImages.size() > 15){
                cameraCalibration(savedImages,chessboardDimensions,calibrationSquareDimension,cameraMatrix,distanceCoefficients);
                saveCameraCalibration("CameraCalibrationFile", cameraMatrix,distanceCoefficients);
                cout<<"Calibrated"<<endl;
            }
            break;
        case 27://escape key
            //exit program
            break;
        }
    }
}

void detectAndDisplay( Mat frame ){ // facial detection
  std::vector<Rect> faces;
  Mat frame_gray;

  cvtColor( frame, frame_gray, CV_BGR2GRAY );
  equalizeHist( frame_gray, frame_gray );

  //Detect faces
  face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

  for( size_t i = 0; i < faces.size(); i++ ){
    Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
    ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

    Mat faceROI = frame_gray( faces[i] );
    std::vector<Rect> eyes;

    //In each face, detect eyes
    eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    midpointface=0.0;
    for( size_t j = 0; j < eyes.size(); j++ ){
       Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
       int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
       circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
       midpointface=midpointface+center.x;
     }
     midpointface = midpointface/eyes.size();
     if(abs(midpointface - midpoint)<40.0){safetoride=true;}
  }

  imshow( "Webcam - startWebcamMonitor", frame );
 }

int main(int argv, char** argc){
    Mat cameraMatrix = Mat::eye(3,3,CV_64F);

    Mat distanceCoefficients;

    Mat frame;

    VideoCapture capture(0);

    //Aruco detection
    loadCameraCalibration("CameraCalibrationFile", cameraMatrix, distanceCoefficients);
    midpoint=startWebcamMonitor(cameraMatrix, distanceCoefficients, arucoSquareDimension,frame,capture);
    cout << "safe to ride" << endl;

    return 0;
}
