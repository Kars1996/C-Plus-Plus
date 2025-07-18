#pragma once

#include <string>
#include <vector>

class ASCIIVideoPlayer {
public:
    ASCIIVideoPlayer();
    ~ASCIIVideoPlayer();
    
    bool loadVideo(const std::string& videoPath);
    void playVideo();
    void showMenu();
    
private:
    std::vector<std::string> asciiFrames;
    std::string audioPath;
    int totalFrames;
    double frameRate;
    
    void extractAudio(const std::string& videoPath);
    void generateASCIIFrames(const std::string& videoPath);
    void setupConsole();
    void restoreConsole();
};