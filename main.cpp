#include <iostream>                 //using OpenCV open source computer vision library for image processing - color detection
#include <opencv2/opencv.hpp>       
#include <opencv2/objdetect.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

bool HolKnt(Mat gray) {
    bool checkH = false;
    Mat frameThreshold;

    equalizeHist(gray, gray);
    equalizeHist(gray, gray);
    equalizeHist(gray, gray);
    equalizeHist(gray, gray);

    GaussianBlur(gray, gray, Size(7, 7), 0, 0, BORDER_DEFAULT);
    Scalar meanG = Scalar(mean(gray));
    inRange(gray, 0, meanG(0) - 115, frameThreshold); 
    
    Mat edges;
    Canny(frameThreshold, edges, 0, 255, 3);
    vector<vector<Point>> holCon;
    vector<Vec4i> hierarchy;
    findContours(edges, holCon, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    vector<RotatedRect> Ellipse(holCon.size());
    for( size_t i = 0; i < holCon.size(); i++ )
    {
        if( holCon[i].size() > 155)
        {
            Ellipse[i] = fitEllipse(holCon[i]);
        }
    }
    
    for( size_t i = 0; i< holCon.size(); i++ )
    {  
        double ratio = (double) Ellipse[i].size.height / Ellipse[i].size.width;
        if ((Ellipse[i].size.height > 20) && (Ellipse[i].size.width > 20) &&
            (ratio > 0.5) && (ratio < 2)){
            
            checkH = true;
        }
    }
    return checkH;
}

bool Cracks(Mat gray) {
    bool checkC = false;
    
    Mat frameThreshold;
    equalizeHist(gray, gray);

    GaussianBlur(gray, gray, Size(7, 7), 0, 0, BORDER_DEFAULT);
    inRange(gray, 0, 13, frameThreshold); 
    
    Mat edges;
    Canny(frameThreshold, edges, 0, 255, 3);
    vector<vector<Point>> crkCon;
    vector<Vec4i> hierarchy;
    findContours(edges, crkCon, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    vector<RotatedRect> Ellipse(crkCon.size());
    for( size_t i = 0; i < crkCon.size(); i++ )
    {
        if( crkCon[i].size() > 5 && crkCon[i].size() < 500)
        {
            Ellipse[i] = fitEllipse(crkCon[i]);
        }
    }
    
    for( size_t i = 0; i< crkCon.size(); i++ )
    {  
        double ratio = (double) Ellipse[i].size.height / Ellipse[i].size.width;
        if (((Ellipse[i].size.height > 150) || (Ellipse[i].size.width > 150)) &&
            ((ratio > 10) || (ratio < 0.1))){ 
            
            checkC = true;
        }
    }

    return checkC;
}

bool Knots(Mat frameGray) {
    bool checkK = false;
    Mat frameThreshold;

    Scalar meanOrig = Scalar(mean(frameGray));
    
    while(meanOrig(0) > 150) {
        frameGray = frameGray * 0.81;
        meanOrig = Scalar(mean(frameGray));
    }

    equalizeHist(frameGray, frameGray);
    equalizeHist(frameGray, frameGray);
    equalizeHist(frameGray, frameGray);
    equalizeHist(frameGray, frameGray);
    
    GaussianBlur(frameGray, frameGray, Size(7, 7), 0, 0, BORDER_DEFAULT);
    
    Scalar meanG = Scalar(mean(frameGray));
    inRange(frameGray, meanG(0)-19, 255, frameThreshold); 
    
    dilate(frameThreshold, frameThreshold, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
    dilate(frameThreshold, frameThreshold, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
    
    Mat edges;
    Canny(frameThreshold, edges, 30, 90, 3);
    vector<vector<Point>> knotCon;
    vector<Vec4i> hierarchy;
    findContours(edges, knotCon, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    vector<RotatedRect> Ellipse(knotCon.size());
    for( size_t i = 0; i < knotCon.size(); i++ )
    {
        if( knotCon[i].size() > 50 && knotCon[i].size() < 500)
        {
            Ellipse[i] = fitEllipse(knotCon[i]);
        }
    }
    
    for( size_t i = 0; i< knotCon.size(); i++ )
    {  
        double ratio = (double) Ellipse[i].size.height / Ellipse[i].size.width;
        if ((Ellipse[i].size.height > 50) && (Ellipse[i].size.height < 500) && 
            (Ellipse[i].size.width > 50) && (Ellipse[i].size.width < 500) &&
            (ratio > 0.63) && (ratio < 1.59)){ 
            
            checkK = true;
        }
    }
    
    return checkK;
}

int main()
{   
    Mat frameGray;
    Mat frameColor;

    frameColor = imread("C:/Users/mro12/.vscode/ObjID/src/Wood Images/0.jpg", IMREAD_COLOR);

    if (frameColor.empty()) {    //frame is empty
        printf("No image\n");
        return -1;
    }
    
    resize(frameColor, frameColor, Size(1020,574));
    cvtColor(frameColor, frameGray, COLOR_BGR2GRAY); 
    
    bool holknt = HolKnt(frameGray);
    bool crk = Cracks(frameGray);
    bool knot = Knots(frameGray);
    bool noDef = true;

    if (holknt == true) {
        cout << "HOLE OR DEEP KNOT PRESENT" << endl; 
        noDef = false;
    }
    if (crk == true) {
        cout << "CRACK PRESENT" << endl;
        noDef = false;
    }
    if (knot == true) {
        cout << "KNOT PRESENT" << endl;
        noDef = false;
    }
    if (noDef == true) {
        cout << "NO DEFECTS" << endl;
    }
    
    return 0;
}
