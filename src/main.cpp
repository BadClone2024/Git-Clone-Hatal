#include "../Include/video_funcions.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

int main(int argc, char *argv[])
{
    // Suppress all OpenCV logging
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);
    bool logging = false, playTheVideo = false;
    string logfilename, whatToDo, operation, infilemain, outfilemain;

    if (argc < 2)
    {
        cout << "\nVideo Conventor 3 By Moshe Nahshon Kisev\n";
        cout << "Example:\nVC3 {operation} [source file] [target file] [paramaters] /Log /Play\n";
        cout << "For more info: VC3 /?\n";
        return 1;
    }
    auto start = chrono::high_resolution_clock::now();

    // int succedOrNot = setOperation(argc, argv);
    if (customToLowerCase(string(argv[argc - 1])) == "/log" || customToLowerCase(string(argv[argc - 2])) == "/log")
        logging = true;

    if (customToLowerCase(string(argv[argc - 1])) == "/play" || customToLowerCase(string(argv[argc - 2])) == "/play")
        playTheVideo = true;

    whatToDo = commandLineHandle(argc, argv);

    if (whatToDo != "success")
    {
        cout << whatToDo;
        logfilename = string(argv[3]).erase(string(argv[3]).length() - 4) + "_log.txt";
        LogUpdate(logfilename, whatToDo, logging);
    }
    else
    {
        operation = customToLowerCase(string(argv[1]));
        infilemain = customToLowerCase(string(argv[2]));
        outfilemain = customToLowerCase(string(argv[3]));
        logfilename = string(argv[3]).erase(string(argv[3]).length() - 4) + "_log.txt";

        if (operation == "convert")
        {
            LogUpdate(logfilename, "Converting a video", logging);
            convert_video(argv[2], argv[3]);
            LogUpdate(logfilename, string("The video: " + infilemain + " was successfully converted and transformoed to: " + outfilemain), logging);
        }
        if (operation == "resize")
        {
            LogUpdate(logfilename, "Resizing a video", logging);
            resizeVideo(argv[2], argv[3], stoi(argv[4]), stoi(argv[5]));
            LogUpdate(logfilename, string("The video: " + infilemain + " was successfully resized and transformoed to: " + outfilemain), logging);
        }
        if (operation == "cut")
        {
            LogUpdate(logfilename, "Cutting a video", logging);
            cutVideo(argv[2], argv[3], stod(argv[4]), stod(argv[5]));
            LogUpdate(logfilename, string("The video: " + infilemain + " was successfully cutted and transformoed to: " + outfilemain), logging);
        }
        if (operation == "blur")
        {
            LogUpdate(logfilename, "Blurring a video", logging);
            blurVideo(argv[2], argv[3], stoi(argv[4]));
            LogUpdate(logfilename, string("The video: " + infilemain + " was successfully blurred and transformoed to: " + outfilemain), logging);
        }
        if (operation == "rotate")
        {
            LogUpdate(logfilename, "Rotating a video", logging);
            rotateVideo(argv[2], argv[3], stoi(argv[4]));
            LogUpdate(logfilename, string("The video: " + infilemain + " was successfully rotated and transformoed to: " + outfilemain), logging);
        }
        if (operation == "watermark")
        {
            LogUpdate(logfilename, "Watermarking a video", logging);
            add_text_to_video(argv[2], argv[3], stoi(argv[4]), stoi(argv[5]), argv[6]);
            LogUpdate(logfilename, string("The video: " + infilemain + " was successfully watermarked and transformoed to: " + outfilemain), logging);
        }
        if (operation == "/?")
            help();
        if (playTheVideo)
            playVideo(argv[3], 1280, 720);

        auto end = chrono::high_resolution_clock::now();
        LogUpdate(logfilename, ("The operation took: " + string(countSeconds(start, end))), logging);
        cout << "The operation took: " << countSeconds(start, end);
    }

    return 0;
}
