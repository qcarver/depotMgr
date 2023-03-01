/**
Copyright 2017 Rafael Muñoz Salinas. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Rafael Muñoz Salinas ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Rafael Muñoz Salinas OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of Rafael Muñoz Salinas.
*/

#include <aruco.h>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <stdexcept>
#include "bin.h"
#include "group.h"

using namespace cv;
using namespace std;
using namespace aruco;

const cv::String window_name = "close this window to quit.";
// class for parsing command line
// operator [](string cmd) return  whether cmd is present //string operator ()(string cmd) return the value as a string:
// -cmd value
class CmdLineParser{int argc;char** argv;public:CmdLineParser(int _argc, char** _argv): argc(_argc), argv(_argv){}   bool operator[](string param)    {int idx = -1;  for (int i = 0; i < argc && idx == -1; i++)if (string(argv[i]) == param)idx = i;return (idx != -1);}    string operator()(string param, string defvalue = "-1")    {int idx = -1;for (int i = 0; i < argc && idx == -1; i++)if (string(argv[i]) == param)idx = i;if (idx == -1)return defvalue;else return (argv[idx + 1]);}};

cv::Mat __resize(const cv::Mat& in, int width)
{
    if (in.size().width <= width)
        return in;
    float yf = float(width) / float(in.size().width);
    cv::Mat im2;
    cv::resize(in, im2, cv::Size(width, static_cast<int>(in.size().height * yf)));
    return im2;
}

bool findBin(vector<Marker> Markers, int bin)
{
    bool found = false;
    
    //Max vectors of columns and rows and their avg
    vector<Column *> vpColumns;
    vector<Row *> vpRows;

    // for each marker, consider its handle to be it's position in the vector of detections ..construction
    for (Marker & marker : Markers)
    {
        Bin * pBin = new Bin(marker);
        bool added_to_existing = false;
        
        for (Column * column : vpColumns)
        {
            if (*pBin < *column)  continue;

            else if (*pBin == *column){
                added_to_existing = column->addBin(pBin);
                break;
            }
        }
        if (!added_to_existing)
        {
            Column * column = new Column;
            column->addBin(pBin);
            vpColumns.emplace_back(column);
        }

        added_to_existing = false;

        for (Row * row : vpRows)
        {
            if (*pBin < *row)  continue;

            else if (*pBin == *row){
                added_to_existing = row->addBin(pBin);
                break;
            }
        }
        if (!added_to_existing)
        {
            Row * row = new Row;
            row->addBin(pBin);
            vpRows.emplace_back(row);
        }
    }

    //this will go in a search function
    cout << "looking for bin " << bin << endl;
    for (Column * pColumn : vpColumns)
    {
        if (pColumn->containsBin(bin))
        cout << "Bin found in column which has X coord avg " << pColumn->getAvg() << endl;
    }
    
    for (Row * pRow : vpRows)
    {
        if (pRow->containsBin(bin))
        cout << "Bin found in row which has Y coord avg " << pRow->getAvg() << endl;
    }


    //this will go in a destructor
    for (Column * pColumn : vpColumns)
    {
        //cout << "Avg: " << pColumn->getAvg() <<  *pColumn << endl;
        delete pColumn;
    }

    for (Row * pRow : vpRows)
    {
        //cout << "Avg: " << pRow->getAvg() <<  *pRow << endl;
        delete pRow;
    }
    return found;
}

int main(int argc, char** argv)
{
    vector<Marker> Markers;
    int bin = 0;

    try
    {
        CmdLineParser cml(argc, argv);
        if (argc == 1 || cml["-h"])
        {
            cerr << "Usage: (in_image|video.avi) [-c cameraParams.yml] [-s markerSize] [-d <dicionary>:ALL_DICTS default] [-f arucoConfig.yml] "   << endl;
            cerr << "\tDictionaries: ";
            for (auto dict : aruco::Dictionary::getDicTypes())
                cerr << dict << " ";
            cerr << endl;
            cerr << "\t Instead of these, you can directly indicate the path to a file with your own generated "
                    "dictionary"
                 << endl;
            cout << "Example to work with apriltags dictionary : video.avi -d TAG36h11" << endl << endl;
            return 0;
        }

        aruco::CameraParameters CamParam;
        // read the input image
        cv::Mat InImage;
        // Open input and read image
        VideoCapture vreader(argv[1]);
        if (vreader.isOpened()) vreader >> InImage;
        else throw std::runtime_error("Could not open input");
    while (cv::getWindowProperty(window_name, 1) >= 0) cv::waitKey(50);
        // read camera parameters if specifedsimple copyult value -1)
        float MarkerSize = std::stof(cml("-s", "-1"));
        // Create the detector
        MarkerDetector MDetector;
        if(cml["-f"]){//uses a configuration file. YOu can create it from aruco_test application
                MDetector.loadParamsFromFile(cml("-f"));
        }
        else{
            // Set the dictionary you want to work with, if you included option -d in command line
           //By default, all valid dictionaries are examined
            if (cml["-d"])
                MDetector.setDictionary(cml("-d"), 0.f);
        }

        // Ok, let's detect
        Markers = MDetector.detect(InImage, CamParam, MarkerSize);

        // What is the bin of interest?
        if(cml["-b"]){
            bin = stoi(cml("-b"));
            cout << "Query for bin: " << bin << endl;
        }

                // for each marker, draw info and its boundaries in the image
        for (unsigned int i = 0; i < Markers.size(); i++)
        {
            if (Markers[i].id == bin)
            {
                cout << Markers[i] << endl;
                Markers[i].draw(InImage, Scalar(127,127, 0), 0, true);
                break;
            } 
            //if the bin is not found or user didn't care, mark bin as false
            else if (i == Markers.size()-1) bin = 0;
        }

        if (bin)
        {
            // show input with augmented information
            cv::namedWindow(window_name, 1); 
            cv::imshow(window_name, __resize(InImage,1280));
        }

    }
    catch (std::exception& ex)
    {
        cout << "Exception :" << ex.what() << endl;
    }

    cout << "Num markers detected: " << Markers.size() << endl;

    if (bin) findBin(Markers,bin);

    //wait for the window to close then.. continue
    while (cv::getWindowProperty(window_name, 1) >= 0) cv::waitKey(50);

    cout << "num columns before exit: " << Group::count << ", num bins before exit: " << Bin::count << endl;
    cout << "num row before exit: " << Group::count << ", num bins before exit: " << Bin::count << endl;

}

    

