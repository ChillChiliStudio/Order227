#ifndef AUDIO_H
#define AUDIO_H

#include "Module.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define FACTION_NUM 2
#define VARIATION_PER_SOUND 3
#define SOV 1
#define CAP 2

struct _Mix_Music;
struct Mix_Chunk;

struct SFX {

	std::string path;
	uint id;

};

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

enum class Entity_type_Sounds {

	BASIC,
	DESOLATOR,
	MAX

};


class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before quitting
	bool CleanUp() override;

	// Play a music file
	bool PlayMusic(const char* path, int loops = -1, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	//Check if something is being played
	bool MusicPlaying();

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	//Volume Control
	void ControlVolume(int vol);
	void ControlMUSVolume(int vol);
	void ControlSFXVolume(int vol);

	void FillArrayFX();
	void LoadIntoArray();

	uint SoundFX_Array[(int)Entity_type_Sounds::MAX][FACTION_NUM][(int)type_sounds::MAX][VARIATION_PER_SOUND];

private:

	_Mix_Music *music = NULL;
	std::list<Mix_Chunk*>	fx;
};

#endif // AUDIO_H