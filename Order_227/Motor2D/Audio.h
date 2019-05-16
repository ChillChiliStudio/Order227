#ifndef AUDIO_H
#define AUDIO_H

#include "Module.h"

#define FACTION_NUM 2
#define VARIATION_PER_SOUND 3
#define SOV 0
#define CAP 1
#define MAX_INFANTRY_NUMBER 7

struct _Mix_Music;
struct Mix_Chunk;

enum class type_sounds
{
	SPAWN,
	MOVING,
	COMFIRMATION,
	HURT,
	SHOT,
	ATTACK,
	MAX

};

enum class song_type {

	NONE = -1,
	MAIN_MENU,
	IN_GAME,
	WIN,
	LOSE
};

struct Music_Track {

	std::string trackName = "NULL";
	char* path = nullptr;
	int num_loops = 0;

	char* nextTrack_id = "NULL";
	Music_Track* nextTrack = nullptr;
};

struct Music_Song {

	std::string songName = "NULL";
	song_type songType = song_type::NONE;
	std::list<Music_Track*> songtracks_list;

	char* nextSong_id = "NULL";
	Music_Song* nextSong = nullptr;
};


class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	bool Start() override;

	// Called before quitting
	bool CleanUp() override;

	// Play a music file
	bool PlayMusic(const char* path, int loops = -1, float fade_time = 0.0f);

	//Check if something is being played
	bool MusicPlaying();

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0, int i = -1);

	//Volume Control
	void ControlVolume(int vol);
	void ControlMUSVolume(int vol);
	void ControlSFXVolume(int vol);

	void FillArrayFX();
	void LoadIntoArray();


	//TODO: The hardoced 4 should be entity/unit type::Max
	uint SoundFX_Array[MAX_INFANTRY_NUMBER][FACTION_NUM][(int)type_sounds::MAX][VARIATION_PER_SOUND];

	pugi::xml_document SFX_XML;

private:

	_Mix_Music *music = NULL;
	std::list<Mix_Chunk*>	fx;
};

#endif // AUDIO_H