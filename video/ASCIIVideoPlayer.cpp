#include "ASCIIVideoPlayer.h"
#include "ASCIIConverter.h"
#include "AudioPlayer.h"
#include "FrameTimer.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

ASCIIVideoPlayer::ASCIIVideoPlayer() : totalFrames(0), frameRate(30.75) {
    audioPath = "audio.wav";
}

ASCIIVideoPlayer::~ASCIIVideoPlayer() {
    restoreConsole();
}

bool ASCIIVideoPlayer::loadVideo(const std::string& videoPath) {
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file " << videoPath << std::endl;
        return false;
    }
    
    totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    frameRate = cap.get(cv::CAP_PROP_FPS);
    
    std::cout << "Extracting audio..." << std::endl;
    extractAudio(videoPath);
    
    std::cout << "Generating ASCII frames..." << std::endl;
    generateASCIIFrames(videoPath);
    
    cap.release();
    return true;
}

void ASCIIVideoPlayer::extractAudio(const std::string& videoPath) {
    std::string command = "ffmpeg -i \"" + videoPath + "\" -vn -acodec pcm_s16le -ar 44100 -ac 2 \"" + audioPath + "\" -y";
    
    #ifdef _WIN32
    system(command.c_str());
    #else
    system(command.c_str());
    #endif
}

void ASCIIVideoPlayer::generateASCIIFrames(const std::string& videoPath) {
    cv::VideoCapture cap(videoPath);
    ASCIIConverter converter(150);
    
    cv::Mat frame;
    int frameCount = 0;
    
    asciiFrames.clear();
    asciiFrames.reserve(totalFrames);
    
    while (cap.read(frame) && frameCount < totalFrames) {
        std::string asciiFrame = converter.convertFrameToASCII(frame);
        asciiFrames.push_back(asciiFrame);
        
        frameCount++;
        
        if (frameCount % 10 == 0) {
            converter.showProgress(frameCount, totalFrames);
        }
    }
    
    std::cout << std::endl << "ASCII generation completed!" << std::endl;
    cap.release();
}

void ASCIIVideoPlayer::setupConsole() {
    #ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    system("mode 150,50");
    
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    #else
    system("clear");
    std::cout << "\033[2J\033[1;1H"; // Clear screen and move cursor to top-left
    std::cout << "\033[40m\033[37m"; // Set black background, white text
    std::cout << "\033[?25l"; // Hide cursor
    #endif
}

void ASCIIVideoPlayer::restoreConsole() {
    #ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    #else
    std::cout << "\033[?25h"; // Show cursor
    std::cout << "\033[0m"; // Reset colors
    #endif
}

// function made bhy AI
void ASCIIVideoPlayer::playVideo() {
    if (asciiFrames.empty()) {
        std::cerr << "No ASCII frames loaded!" << std::endl;
        return;
    }
    
    setupConsole();
    
    AudioPlayer audioPlayer;
    if (!audioPlayer.initialize()) {
        std::cerr << "Failed to initialize audio player!" << std::endl;
        return;
    }
    
    if (!audioPlayer.loadAudio(audioPath)) {
        std::cerr << "Failed to load audio file!" << std::endl;
        return;
    }
    
    audioPlayer.playAudio();
    
    FrameTimer timer(frameRate);
    
    for (size_t i = 0; i < asciiFrames.size(); ++i) {
        #ifdef _WIN32
        COORD coord = {0, 0};
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        #else
        std::cout << "\033[1;1H"; // Move cursor to top-left
        #endif
        
        std::cout << asciiFrames[i] << std::flush;
        timer.sleep();
        
        #ifdef _WIN32
        if (_kbhit()) {
            char key = _getch();
            if (key == 'q' || key == 'Q' || key == 27) { // ESC key
                break;
            }
        }
        #endif
    }
    
    audioPlayer.stopAudio();
    restoreConsole();
}

void ASCIIVideoPlayer::showMenu() {
    std::string input;
    
    while (true) {
        std::cout << "==============================================================" << std::endl;
        std::cout << "ASCII Video Player (Dark Mode)" << std::endl;
        std::cout << "==============================================================" << std::endl;
        std::cout << "Select option:" << std::endl;
        std::cout << "1) Play video" << std::endl;
        std::cout << "2) Exit" << std::endl;
        std::cout << "==============================================================" << std::endl;
        std::cout << "Your option: ";
        
        std::getline(std::cin, input);
        
        if (input == "1") {
            std::cout << "Please enter the video file path: ";
            std::string videoPath;
            std::getline(std::cin, videoPath);
            
            if (loadVideo(videoPath)) {
                playVideo();
            } else {
                std::cerr << "Failed to load video: " << videoPath << std::endl;
            }
        } else if (input == "2") {
            break;
        } else {
            std::cout << "Unknown input!" << std::endl;
        }
    }
}