/**
Copyright 2017 Rafael Muñoz Salinas. All rights reserved.

Redistribution and use in source and bin_idary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in bin_idary form must reproduce the above copyright notice, this list
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
#include "rack.h"
#include "argc_argv.hpp"

using namespace cv;
using namespace std;
using namespace aruco;

vector<Marker> Markers;
cv::Mat InImage;
aruco::CameraParameters CamParam; //-c
MarkerDetector MDetector;
VideoCapture vreader;
ArgcArgvInput args;  


//params for how long to wait for cv image window to open
struct wait_for_image {
    int timeout_ms = 1500;
    int step_ms = 10;
    int waited_ms = 0;
} wfi;

const cv::String window_name = "close this window to quit.";

cv::Mat __resize(const cv::Mat& in, int width)
{
    if (in.size().width <= width)
        return in;
    float yf = float(width) / float(in.size().width);
    cv::Mat im2;
    cv::resize(in, im2, cv::Size(width, static_cast<int>(in.size().height * yf)));
    return im2;
}

bool findBin(vector<Marker> Markers, int bin_id)
{
    Rack rack(Markers);
    return rack.findBinRow(bin_id) && rack.findBinColumn(bin_id);
}


bool openInputImage() //uses image_filename
{
    bool success = false;

    if (args.image_filename.empty())
    {
        cerr << "No input image provided." << endl;
    }
    else
    {
        vreader.open(args.image_filename);
    
        if (!vreader.isOpened())
        {
            cerr << "Could not open the input file: " << args.image_filename << endl;
        }
        else
        {

            vreader >> InImage;
            if (InImage.empty())
            {
                cerr << "Could not read the image from: " << args.image_filename << endl;
            }
            else
            {
                success = true;
                cout << "Input image opened successfully: " << args.image_filename << endl;
            }   
        }
    }

    return success;
}


bool detect(){
    try
    {
        Markers = MDetector.detect(InImage, CamParam, args.marker_size);
    }
    catch (const std::exception& e)
    {
        cerr << "Exception caught during marker detection: " << e.what() << endl;
        return false;
    }
    catch (...)
    {
        cerr << "Unknown exception caught during marker detection." << endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    if (!get_args(argc, argv, args)){
        cerr << "Sad face: no args were parsed" << endl;
        return -1;
    }

    if (!openInputImage()) {
        cerr << "Failed to open input image or video." << endl;
        return -1;
    }

    if (!detect()){
        cerr << "Marker detection failed." << endl;
        return -1; 
    }

    // for each marker, draw info and its boundaries in the image
    for (unsigned int i = 0; i < Markers.size(); i++)
    {
        if (Markers[i].id == args.bin_id)
        {
            cout << Markers[i] << endl;
            // show input with augmented information
            cv::namedWindow(window_name, 1); 
            Markers[i].draw(InImage, Scalar(127,127, 0), 5, true);
            cv::imshow(window_name, __resize(InImage,1280));
            break;
        } 
        //if the bin_id is not found or user didn't care, mark bin_id as false
        else if (i == Markers.size()-1) args.bin_id = 0;
    }

    cout << "Num markers detected: " << Markers.size() << endl;

    if (args.bin_id) findBin(Markers,args.bin_id);

    //wait for the window to close then.. continue
    while (cv::getWindowProperty(window_name, 1) >= 0) cv::waitKey(50);
}

    

