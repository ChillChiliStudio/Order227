#ifndef AUDIO_H
#define AUDIO_H

#include "Module.h"
#include "Point.h"
#include "SDL_mixer\include\SDL_mixer.h"

#define FACTION_NUM 2
#define VARIATION_PER_SOUND 4
#define SOV 0
#define CAP 1
#define MAX_INFANTRY_NUMBER 8

struct _Mix_Music;
struct Mix_Chunk;

// Mixer Channels go from 0 to 7
enum class type_sounds
{
	SPAWN,			//Channel 0
	MOVING,			//Channel 1
	CONFIRMATION,	//Channel 1
	HURT,			//Channel 2
	SHOT,			//Channel 3,4,5,6,7
	ATTACK,			//Channel 1
	MAX
};

enum sound_channels	// 0 for Spawns, 1 for Orders, 2 for Hurt, 3 for Explosions, ANY for attacks/shooting
{
	CHANNEL_SPAWN = 0,
	CHANNEL_MOVING = 1,
	CHANNEL_CONFIRMATION = 1,
	CHANNEL_HURT = 2,
	CHANNEL_SHOT = -1,
	CHANNEL_ATTACK = 1
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
	bool PlayFx(unsigned int fx, int repeat = 0, int channel = -1, fPoint pos = { 0.0f, 0.0f }, bool spatial = false);

	//Volume Control (Paremeter input and variables use percentage 0 - 100 values, Mixer uses 0 - 128 values)
	//Master Volume
	void SetMasterVolume() const;
	void ChangeMasterVolume(uint vol);
  
	//Music Volume
	uint SetMusicVolume() const;
	uint ChangeMusicVolume(uint vol);
  
	//Sfx Volume
	uint SetChannelVolume(int channel = -1);
	uint ChangeChannelVolume(uint vol, int channel = -1);
  
	//Chunck Volume
	uint SetSfxChunkVolume(uint vol, int id = -1);
  
	//Get functions
	uint* GetMasterVolume() {
		return &masterVolume;
	}
	uint* GetSfxVolume() {
		return &sfxVolume;
	}
	uint* GetMusicVolume() {
		return &musicVolume;
	}

	//Legacy Lucho Methods
	/*void ControlVolume(int vol);
	void ControlMUSVolume(int vol);
	void ControlSFXVolume(int vol);*/

	void FillArrayFX();
	void LoadIntoArray();

public:
	// 0% - 100% Range
	uint masterVolume;	//General Volume
	uint musicVolume;	  //Music Volume
	uint sfxVolume;		  //Sfx Volume

	//Spatial Audio
	float sfxAudioRadius;
	uPoint leftEar;
	uPoint rightEar;
	int earOffset;

	uint SoundFX_Array[MAX_INFANTRY_NUMBER][(int)type_sounds::MAX][VARIATION_PER_SOUND];
	int VarsXsound[MAX_INFANTRY_NUMBER][(int)type_sounds::MAX];

	pugi::xml_document SFX_XML;

private:

	_Mix_Music *music = NULL;
	std::list<Mix_Chunk*>	fx;
};

#endif // AUDIO_H