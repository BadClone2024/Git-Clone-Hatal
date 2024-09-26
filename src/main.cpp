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
        cout << "Git commit";
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
    {
        playTheVideo = true;
        cout << "Video";
    }

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
            LogUpdate(logfilename, "Converting the video", logging);
            convert_video(argv[2], argv[3]);
            LogUpdate(logfilename, string("The video: " + string(argv[2]) + " was successfully converted and transformoed to: " + string(argv[3])), logging);
        }
        if (operation == "resize")
        {
            LogUpdate(logfilename, "Resizing the video", logging);
            resizeVideo(argv[2], argv[3], stoi(argv[4]), stoi(argv[5]));
            LogUpdate(logfilename, string("The video: " + string(argv[2]) + " was successfully resized and transformoed to: " + string(argv[3])), logging);
        }
        if (operation == "cut")
        {
            LogUpdate(logfilename, "Cutting the video", logging);
            cutVideo(argv[2], argv[3], stod(argv[4]), stod(argv[5]));
            LogUpdate(logfilename, string("The video: " + string(argv[2]) + " was successfully cutted and transformoed to: " + string(argv[3])), logging);
        }
        if (operation == "blur")
        {
            LogUpdate(logfilename, "Blurring the video", logging);
            blurVideo(argv[2], argv[3], stoi(argv[4]));
            LogUpdate(logfilename, string("The video: " + string(argv[2]) + " was successfully blurred and transformoed to: " + string(argv[3])), logging);
        }
        if (operation == "watermark")
        {
            LogUpdate(logfilename, "Watermarking the video", logging);
            add_text_to_video(argv[2], argv[3], stoi(argv[4]), stoi(argv[5]), argv[6]);
            LogUpdate(logfilename, string("The video: " + string(argv[2]) + " was successfully watermarked and transformoed to: " + string(argv[3])), logging);
        }
        if (operation == "/?")
            help();
        if (playTheVideo && whatToDo == "success")
            playVideo(argv[3]);

        auto end = chrono::high_resolution_clock::now();
        LogUpdate(logfilename, ("The operation took: " + string(countSeconds(start, end))), logging);
    }

    return 0;
}
