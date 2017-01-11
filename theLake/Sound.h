#pragma once

#include <string>
#include <windows.h>
#include <Mmsystem.h>
#include <mciapi.h>

//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "Winmm.lib")

class Sound
{
public:
	Sound();
	Sound(std::string p);
	~Sound();

	// Methods
	void play();
	void pause();
	void resume();
	void stop();
	void close();
	void setVolume(int vol); // 1 - 1000

private:
	void load();
	std::string path;
};

