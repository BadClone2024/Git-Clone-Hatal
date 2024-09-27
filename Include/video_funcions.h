#ifndef VIDEO_FUNCTIONS_H
#define VIDEO_FUNCTIONS_H

#include "../Include/video_funcions.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>

#include <fstream>
#include <chrono> // For timing
#include <ctime>  // For date and time

using namespace std;
using namespace cv;

string customToLowerCase(string str);

int checkVideoFileAccess(const string &readFilePath);

bool containsOnlyNumbers(const string &parameter);

string commandLineHandle(int argc, char *argv[]);

string getCurrentDateTime();

void LogUpdate(string FileName, string LogLine, bool logging);

void offerHelp();

string countSeconds(chrono::high_resolution_clock::time_point start, chrono::high_resolution_clock::time_point end);

int help();

int playVideoInMediaPlayer(const string &videoFile);

int get_codec_from_extension(const string &outputPath);

int rotateVideo(const string &inputPath, const string &outputPath, double angle);

int resizeVideo(const string &inputPath, const string &outputPath, int newWidth, int newHeight);

int cutVideo(const string &inputPath, const string &outputPath, double startTime, double endTime);

int convert_video(const string &inputPath, const string &outputPath);

int blurVideo(const string &inputPath, const string &outputPath, int blurStrength);

int add_text_to_video(const string &inputPath, const string &outputPath, int x, int y, const string &text);

string getCommandLine(int argc, char *argv[]);

#endif // VIDEO_FUNCTIONS_H
