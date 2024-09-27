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

string logfilename, logmessage;
bool logging = false;

string customToLowerCase(string str)
{
    // Convert to lowercase
    transform(str.begin(), str.end(), str.begin(), ::tolower);

    // Remove trailing spaces
    size_t end = str.find_last_not_of(" \n\r\t");
    if (end != string::npos)
    {
        str.erase(end + 1); // Erase from end+1 to the end of the string
    }
    else
    {
        str.clear(); // If the string is all spaces, clear it
    }

    return str;
}

bool containsOnlyNumbers(const string &parameter)
{
    if (parameter.empty())
        return false; // Return false for an empty string

    for (char c : parameter)
    {
        if (c < '0' || c > '9')
            return false;
    }
    return true;
}

bool stringExists(string arr[], int size, string target)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == target)
        {
            return true; // Found the string
        }
    }
    return false; // Not found
}

int checkVideoFileAccess(const string &readFilePath)
{
    // Check for reading the video file
    VideoCapture cap(readFilePath);
    if (!cap.isOpened())
    {
        return 1; // Return -1 for reading failure
    }

    // Get video properties from the input video
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    int fps = static_cast<int>(cap.get(CAP_PROP_FPS));
    int fourcc = static_cast<int>(cap.get(CAP_PROP_FOURCC)); // Get codec from the input video

    cap.release(); // Close the file after checking

    return 0; // Return 0 for success (both reading and writing)
}

string commandLineHandle(int argc, char *argv[])
{
    string example = "";

    if (string(argv[1]) != "/?" && argc < 4)
        return "Error: Not enough arguments";

    // show exanmple
    if (string(argv[1]) == "/?")
    {

        help();
        return "";
    }

    // checking if the requested operation is valid
    string operation;
    operation = customToLowerCase(string(argv[1]));
    string operations[7] = {"convert", "cut", "resize", "rotate", "watermark", "blur", "/?"};
    stringExists(operations, 7, operation);
    if (!stringExists(operations, 7, operation))
        return "Invalid operation: " + string("\"" + operation + "\"");

    // end checking the operation

    // checking if inputed file names are valid
    string infile = customToLowerCase(string(argv[2]));
    string outfile = customToLowerCase(string(argv[3]));
    logfilename = string(argv[3]).erase(string(argv[3]).length() - 4) + "_log.txt";
    if (infile.length() < 5)

        return "Source file name " + string("\"" + infile + "\"" + " is invalid");

    if (outfile.length() < 5)

        return "Target file name " + string("\"" + outfile + "\"" + " is invalid");

    // checking input file
    string fileFormats[4] = {".avi", ".mp4", ".mov", ".mkv"};

    if (!stringExists(fileFormats, 4, infile.substr(infile.length() - 4)))

        return "Invalid extention for the source file: " + string("\"" + infile + "\"\n" + "Valid extension are: .MP4, .AVI, .MOV and .MKV.");

    if (!stringExists(fileFormats, 4, outfile.substr(outfile.length() - 4)))

        return "Invalid extention for the target file: " + string("\"" + outfile + "\"\n" + "Valid extension are: .MP4, .AVI, .MOV and .MKV.");

    // end checking files

    // check if the paramaters are correct

    if (operation == "convert")
    {
        example = "\nProper syntax: VC3 convert MyVideo.MP4 MyVideo.AVI /Log /Play" + example;
        if (argc > 6)
            return "Too many paramaters" + example;
    }
    if (operation == "cut")
    {
        example = "\nProper syntax: VC3 cut MyVideo.MP4 MyVideo.MP4 /Log /Play" + example;
        if (argc < 5)
            return "There must be at least two paramaters in order to cut the video." + example;

        if (!containsOnlyNumbers(string(argv[4]))) // argv[4] is the first paramater that must contain the first second you want to start cutting from.
            return "Cutting paramaters, instead of " + string("\"" + string(argv[4]) + "\"") + string(", must be numbers only" + example);

        if (argc < 6)
            return "Missing last parameter" + example;

        if (!containsOnlyNumbers(string(argv[5]))) // argv[5] is the last paramater that must contain the last second you want to finish the video at.
            return "Cutting paramaters, instead of " + string("\"" + string(argv[5]) + "\"") + string(", must be numbers only" + example);

        if (stod(argv[4]) >= stod(argv[5]))
            return "Starting second must be before the ending second" + example;

        if (argc > 8)
            return "Too many paramaters" + example;
    }

    if (operation == "resize")
    {
        example = "\nProper syntax: VC3 resize MyVideo.MP4 MyVideo.MP4 500 250 /Log /Play";
        if (argc < 5)
            return "At least two paramaters are required: width and height" + example;

        if (!containsOnlyNumbers(string(argv[4]))) // argv[4] is the first paramater that must contain the width
            return "Resizing paramaters, instead of " + string("\"" + string(argv[4]) + "\"") + string(", must be numbers only" + example);

        if (argc < 6)
            return "Missing one parameter" + example;

        if (!containsOnlyNumbers(string(argv[5]))) // argv[5] is the last paramater that must contain the height
            return "Resizing paramaters, instead of " + string("\"" + string(argv[5]) + "\"") + string(", must be numbers only" + example);

        if (argc > 8)

            return "Too many paramaters" + example;
    }
    if (operation == "rotate")
    {
        example = "\nProper syntax: VC3 rotate MyVideo.MP4 MyVideo.MP4 45 /Log /Play";
        if (argc < 5)
            return "Missing roitation angle parameter" + example;

        if (!containsOnlyNumbers(string(argv[4]))) // argv[4] is the paramater that would rotate the video.
            return "Rotation paramater, instead of " + string("\"" + string(argv[4]) + "\"") + string(", must be a number") + example;

        if (argc > 7)

            return "Too many paramaters" + example;
    }
    if (operation == "watermark")
    {
        example = "\nProper syntax: VC3 MyVideo.MP4 MyVideo.MP4 100 50 \"This video was marked by VC3\" /Log /Play";
        if (argc < 7)
            return "Missing parameters" + example;

        if (!containsOnlyNumbers(string(argv[4]))) // argv[4] is the paramater that must contain the X value.
            return "X location paramater, instead of " + string("\"" + string(argv[4]) + "\"") + string(", must be a number" + example);

        if (!containsOnlyNumbers(string(argv[5]))) // argv[5] is the paramater that must contain the Y value.
            return "Y location paramater, instead of " + string("\"" + string(argv[5]) + "\"") + string(", must be a number" + example);

        if (argc > 7 && getCommandLine(argc, argv).find("/log") == string::npos && getCommandLine(argc, argv).find("/play") == string::npos)
            return "Too many paramaters.\nNote: for texts that contain spaces, pelase add quotations as such: \"Your text with spaces\"." + example;
    }

    if (operation == "blur")
    {
        example = "\nProper syntax: VC3 blur MyVideo.MP4 MyVideo.MP4 50 /Log /Play";
        if (argc < 5)
            return "Not enough parameters" + example;

        if (!containsOnlyNumbers(string(argv[4]))) // argv[4] is the first paramater that must contain the first resolution.
            return "The paramater, instead of  " + string("\"" + string(argv[4]) + "\"") + string(", must be a number" + example);

        if (argc > 7)
            return "Too many paramaters" + example;
    }

    int accessResult = checkVideoFileAccess(argv[2]); // Call the function once and store the result

    if (accessResult == 1)

        return "Can't get access to the source file: " + string(argv[2]) + "\nMake sure that the path to the video file is correct and that the file exist.";

    return "success";
}

string getCommandLine(int argc, char *argv[])
{
    string theCommand = "";
    for (int i = 0; i < argc; i++)
    {
        theCommand += argv[i] + string(" ");
    }
    return customToLowerCase(theCommand);
}

string getCurrentDateTime()
{
    // Get the current time
    time_t now = time(0);

    // Convert it to local time
    tm *localTime = localtime(&now);

    // Format the date and time into a string
    string dateTime = to_string(localTime->tm_year + 1900) + "-" +
                      to_string(localTime->tm_mon + 1) + "-" +
                      to_string(localTime->tm_mday) + " " +
                      to_string(localTime->tm_hour) + ":" +
                      to_string(localTime->tm_min) + ":" +
                      to_string(localTime->tm_sec);

    return dateTime;
}

void LogUpdate(string FileName, string LogLine, bool logging)
{
    LogLine = getCurrentDateTime() + ": " + LogLine;
    // Attempt to write to the log file
    if (logging)
        try
        {
            ofstream logFile(FileName, ios::app);

            if (!logFile.is_open())
            {
                throw runtime_error("Could not open log file for writing.");
            }

            logFile << LogLine << endl; // Write the message to the log file
            logFile.close();            // Close the file after writing
        }
        catch (const exception &e)
        {
            // If there's an error opening the file, create it
            ofstream logFile(FileName); // This will create the file
            if (logFile.is_open())
            {
                logFile << LogLine << endl; // Write the message to the new log file
                logFile.close();            // Close the file after writing
            }
            else
            {
                cerr << "Error creating log file." << endl;
            }
        }
}

string countSeconds(chrono::high_resolution_clock::time_point start, chrono::high_resolution_clock::time_point end)
{
    auto duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    int minutes = duration / 60; // Calculate total minutes
    int seconds = duration % 60; // Calculate remaining seconds

    // Create a string stream to format the output
    stringstream ss;
    ss << minutes << ":" << (seconds < 10 ? "0" : "") << seconds; // Format as MM:SS

    return ss.str(); // Return the formatted elapsed time as a string
}

int help()
{
    cout << "        options:\n"
            "    /Log to log the operation into log.txt\n"
            "    /Play to play the target file after conversion.\n\n"
            "    To convert:\n"
            "    VC3 {convert} [source file] [target file] /option1 /option2\n"
            "    Example: VC3 convert MyVideo.MP4 MyVideo.AVI /Log /Play\n\n"
            "    To resize:\n"
            "    VC3 {resize} [source file] [target file] [width] [height] /option1 /Option2\n"
            "    Example: VC3 resize MyVideo.MP4 MyVideo.MP4 500 250 /Log /Play\n\n"
            "    To rotate:\n"
            "    VC3 {rotate} [source file] [target file] [rotation angle] /option1 /Option2\n"
            "    Example: VC3 rotate MyVideo.MP4 MyVideo.MP4 45 /Log /Play\n\n"
            "    To cut:\n"
            "    VC3 {cut} [source file] [target file] [start second] [end second] /option1 /option2\n"
            "    Example: VC3 cut MyVideo.MP4 MyVideo.MP4 /Log /Play\n\n"
            "    To blur:\n"
            "    VC3 {blur} [source file] [target file] [blur strength 1..100] /option1 /option2\n"
            "    Example: VC3 blur MyVideo.MP4 MyVideo.MP4 50 /Log /Play\n\n"
            "    To watermark:\n"
            "    VC3 {watermark} [source file] [target file] [your text] [x] [y] /option1 /option2\n"
            "    Example: VC3 MyVideo.MP4 MyVideo.MP4 \"This video was marked by VC3\" 100 50 /Log /Play\n";
    return 1;
}

void playVideo(const string &videoPath, int screenWidth, int screenHeight)
{
    // Open the video file
    VideoCapture cap(videoPath);

    // Check if the video file was opened successfully
    if (!cap.isOpened())
    {
        return;
    }

    // Get the video frame rate
    double fps = cap.get(CAP_PROP_FPS);
    int delay = 1000 / fps; // Delay between frames in milliseconds

    Mat frame;
    while (true)
    {
        // Read the next frame
        bool isSuccess = cap.read(frame);

        if (!isSuccess)
        {
            break;
        }

        // Resize the frame to fit the screen size
        Mat resizedFrame;
        resize(frame, resizedFrame, Size(screenWidth, screenHeight));

        // Display the resized frame
        imshow("Video Playback", resizedFrame);

        // Wait for 'ESC' key press for 1ms
        if (waitKey(delay) >= 0)
        { // '27' is the ASCII code for the 'ESC' key
            cout << "Key was pressed. Exiting..." << endl;
            break;
        }
    }

    // Release the video file and close the window
    cap.release();
    destroyAllWindows();
}

int get_codec_from_extension(const string &outputPath)
{
    string extension = outputPath.substr(outputPath.find_last_of(".") + 1);

    if (extension == "avi")
    {
        return VideoWriter::fourcc('M', 'J', 'P', 'G'); // MJPEG codec for AVI
    }
    else if (extension == "mp4")
    {
        return VideoWriter::fourcc('a', 'v', 'c', '1'); // H.264 codec for MP4
    }
    else if (extension == "mov")
    {
        return VideoWriter::fourcc('m', 'p', '4', 'v'); // MPEG-4 codec for MOV
    }
    else if (extension == "mkv")
    {
        return VideoWriter::fourcc('X', '2', '6', '4'); // H.264 codec for MKV
    }
    else
    {
        // cerr << "Error: Unsupported output file format: " << extension << endl;
        return -1;
    }
}

int rotateVideo(const string &inputPath, const string &outputPath, double angle)
{

    // Open the input video file
    VideoCapture cap(inputPath);
    if (!cap.isOpened())
    {
        return 1; // Return an error code
    }

    // Get original video properties
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(CAP_PROP_FPS);
    int fourcc = static_cast<int>(cap.get(CAP_PROP_FOURCC));

    // Calculate the center point and rotation matrix
    Point2f center(frameWidth / 2.0, frameHeight / 2.0);
    Mat rotationMatrix = getRotationMatrix2D(center, angle, 1.0);

    // Create the output video writer
    VideoWriter writer(outputPath, fourcc, fps, Size(frameWidth, frameHeight));
    if (!writer.isOpened())
    {
        return 2; // Return an error code
    }

    system("cls");

    cout << "Rotating the video..." << endl;

    // Read frames, rotate, and write to output
    Mat frame, rotatedFrame;
    while (true)
    {
        cap >> frame; // Capture a new frame
        if (frame.empty())
            break; // Break when no more frames

        // Rotate the frame using the rotation matrix
        warpAffine(frame, rotatedFrame, rotationMatrix, Size(frameWidth, frameHeight));

        // Write the rotated frame to the output video
        writer.write(rotatedFrame);
    }

    // Release the VideoCapture and VideoWriter
    cap.release();
    writer.release();

    cout << "Video rotation completed and saved to " << outputPath << endl;
    return 0; // Return 0 to indicate success
}

// Function to resize a video
int resizeVideo(const string &inputPath, const string &outputPath, int newWidth, int newHeight)
{
    // Open the input video file
    VideoCapture cap(inputPath);
    if (!cap.isOpened())
    {
        return 1; // Return an error code
    }

    // Get video properties
    int originalWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int originalHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(CAP_PROP_FPS);
    int codec = static_cast<int>(cap.get(CAP_PROP_FOURCC));

    // Output video writer
    VideoWriter writer(outputPath, codec, fps, Size(newWidth, newHeight));
    if (!writer.isOpened())
    {
        return 2; // Return an error code
    }

    system("cls");

    cout << "Resizing the video..." << endl;

    // Loop through each frame, resize, and write to the new video
    Mat frame, resizedFrame;
    while (true)
    {
        cap >> frame; // Read the next frame
        if (frame.empty())
            break; // End of video

        // Resize the frame
        resize(frame, resizedFrame, Size(newWidth, newHeight));

        // Write the resized frame to the output video
        writer.write(resizedFrame);
    }

    // Release the video capture and writer
    cap.release();
    writer.release();

    cout << "Video has been resized and saved to " << outputPath << endl;
    return 0; // Return 0 to indicate success
}

int cutVideo(const string &inputPath, const string &outputPath, double startTime, double endTime)
{

    // Open the input video file
    VideoCapture cap(inputPath);
    if (!cap.isOpened())
    {
        return 1; // Return an error code
    }

    // Get original video properties
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(CAP_PROP_FPS);
    int fourcc = static_cast<int>(cap.get(CAP_PROP_FOURCC));
    double duration = cap.get(CAP_PROP_FRAME_COUNT) / fps;

    // Ensure that start and end times are within the duration of the video
    if (startTime < 0 || endTime > duration || startTime >= endTime)
    {
        return 2; // Return an error code
    }

    // Set the video to start at the specified start time
    cap.set(CAP_PROP_POS_MSEC, startTime * 1000);

    // Create the output video writer
    VideoWriter writer(outputPath, fourcc, fps, Size(frameWidth, frameHeight));
    if (!writer.isOpened())
    {
        return 3; // Return an error code
    }

    system("cls");

    cout << "Cutting the video..." << endl;
    // system("cls");

    // Cut video by reading frames from start time to end time
    Mat frame;

    double currentTime = startTime;
    while (currentTime < endTime)
    {
        cap >> frame; // Capture a new frame
        if (frame.empty())
            break; // Break if no more frames

        writer.write(frame); // Write the frame to the output video

        // Move to the next frame and update current time
        currentTime = cap.get(CAP_PROP_POS_MSEC) / 1000.0;
    }

    // Release the VideoCapture and VideoWriter
    cap.release();
    writer.release();

    cout << "Video cut completed from " << startTime << " to " << endTime << " seconds and saved to " << outputPath << endl;
    return 0; // Return 0 to indicate success
}

// Function to determine codec based on the file extension

// Flexible video conversion function
int convert_video(const string &inputPath, const string &outputPath)
{
    // Open the input video file

    VideoCapture cap(inputPath);
    if (!cap.isOpened())
    {
        return 1;
    }

    // Get video properties
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(CAP_PROP_FPS);

    // Get the appropriate codec based on the output file extension
    int codec = get_codec_from_extension(outputPath);
    if (codec == -1)
    {
        return 2;
    }

    // Create the VideoWriter object for output video
    VideoWriter writer(outputPath, codec, fps, Size(frameWidth, frameHeight));
    if (!writer.isOpened())
    {
        return 3;
    }

    Mat frame;

    system("cls");

    cout << "Converting the video..." << endl;

    // Read and write each frame
    while (true)
    {
        cap >> frame;

        if (frame.empty())
        {
            break;
        }

        writer.write(frame);
    }

    // Release resources
    cap.release();
    writer.release();

    cout << "Video successfully converted and saved to: " << outputPath << endl;
    return 0;
}

int blurVideo(const string &inputPath, const string &outputPath, int blurStrength)
{

    // Ensure blurStrength is odd
    if (blurStrength % 2 == 0)
    {
        blurStrength++;
    }

    // Open the input video file
    VideoCapture cap(inputPath);
    if (!cap.isOpened())
    {
        return 1; // Return an error code
    }

    // Get video properties
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(CAP_PROP_FPS);
    int totalFrames = static_cast<int>(cap.get(CAP_PROP_FRAME_COUNT));
    int codec = static_cast<int>(cap.get(CAP_PROP_FOURCC));

    // Define the codec and create a VideoWriter object for the output video
    VideoWriter writer(outputPath, codec, fps, Size(frameWidth, frameHeight));
    if (!writer.isOpened())
    {
        return 2; // Return an error code
    }
    system("cls");

    cout << "Applying blur to the video..." << endl;

    // Process each frame
    Mat frame;
    int frameCounter = 0;

    while (frameCounter < totalFrames)
    {
        bool isSuccess = cap.read(frame); // Read the next frame
        if (!isSuccess || frame.empty())
        {
            break; // Exit loop when no more frames are available
        }

        // Apply Gaussian blur to the frame
        Mat blurredFrame;
        GaussianBlur(frame, blurredFrame, Size(blurStrength, blurStrength), 0);

        // Write the blurred frame to the output video
        writer.write(blurredFrame);

        // Increment the frame counter
        frameCounter++;
    }

    // Release the resources
    cap.release();
    writer.release();

    cout << "Video blurred successfully and saved to " << outputPath << endl;
    return 0; // Return 0 to indicate success
}

int add_text_to_video(const string &inputPath, const string &outputPath, int x, int y, const string &text)
{

    // Open the input video file
    VideoCapture cap(inputPath);
    if (!cap.isOpened())
    {
        return 1;
    }

    double fps = cap.get(CAP_PROP_FPS);
    int frameWidth = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    int codec = static_cast<int>(cap.get(CAP_PROP_FOURCC));

    // Create the VideoWriter object for output video
    VideoWriter writer(outputPath, codec, fps, Size(frameWidth, frameHeight));
    if (!writer.isOpened())
    {
        return 2;
    }

    system("cls");

    cout << "Applying watermark to the video..." << endl;

    Mat frame;

    while (true)
    {
        cap >> frame;

        // Break the loop if we reach the end of the video
        if (frame.empty())
        {
            break;
        }

        // Add text to the frame
        putText(frame, text, Point(x, y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, LINE_AA);

        // Write the frame with text to the output video
        writer.write(frame);
    }

    // Release resources
    cap.release();
    writer.release();

    cout << "Video successfully processed with text added and saved to: " << outputPath << endl;
    return 0;
}
