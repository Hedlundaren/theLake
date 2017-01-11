#include "Sound.h"



Sound::Sound() : path("catmas.mp3")
{
	load();
}

Sound::Sound(std::string p) : path(p)
{
	load();
}

void Sound::load() {

	std::string m1 = "open \"";
	m1 += path;
	m1 += "\" type mpegvideo alias mp3";
	const char * tot = m1.c_str();

	mciSendString(tot, NULL, 0, NULL);
}

void Sound::setVolume(int vol) {

	std::string str = "setaudio mp3 volume to ";
	str += std::to_string(vol);
	const char * tot = str.c_str();
	mciSendString(tot, NULL, 0, NULL);

}

void Sound::play() {
	mciSendString("play mp3", NULL, 0, NULL);
}

void Sound::pause() {
	mciSendString("pause mp3", NULL, 0, NULL);
}

void Sound::resume() {
	mciSendString("resume mp3", NULL, 0, NULL);
}

void Sound::stop() {
	mciSendString("stop mp3", NULL, 0, NULL);
}

void Sound::close() {
	mciSendString("close mp3", NULL, 0, NULL);
}

Sound::~Sound()
{
}
