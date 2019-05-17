#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Audio.h"
#include "Unit.h"
#include "Window.h"
#include "Render.h"
#include "Geometry.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

Audio::Audio() : Module()
{
	music = NULL;
	name.assign("audio");
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if ((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	// Load volumes
	masterVolume = config.attribute("volume").as_uint();
	musicVolume = config.child("music").attribute("volume").as_uint();
	sfxVolume = config.child("sfx").attribute("volume").as_uint();

	// Set channel volume
	SetMasterVolume();

	return ret;
}

bool Audio::Start() {

	static_assert((int)infantry_type::INFANTRY_MAX == MAX_INFANTRY_NUMBER, "MAX_INFANTRY_NUMBER in Audio.h is not the same that the number of unit types (infantry_type::INFANTRY_MAX). Sounds won't be loaded! Modify the MAX_INFANTRY_NUMBER define value and set it as the same as infantry_type::INFANTRY_MAX");

	FillArrayFX();
	LoadIntoArray();
	return true;
}

// Called before quitting
bool Audio::CleanUp()
{
	if (!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if (music != nullptr)
	{
		Mix_FreeMusic(music);
	}

	if (fx.empty() == false) {

		std::list<Mix_Chunk*>::iterator item = fx.begin();
		for (; item != fx.end(); item = next(item))
			Mix_FreeChunk(*item);

		fx.clear();
	}

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool Audio::PlayMusic(const char* path, int loops, float fade_time)
{
	bool ret = true;

	if (!active)
		return false;

	if (music != NULL)
	{
		if (fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if (music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if (fade_time > 0.0f)
		{
			if (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if (Mix_PlayMusic(music, loops) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}


//Check if something is being played
bool Audio::MusicPlaying() {

	if (Mix_PlayingMusic() == 0)
		return false;

	return true;
}

// Load WAV
unsigned int Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if (!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if (chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

// Play WAV
bool Audio::PlayFx(unsigned int id, int repeat, fPoint pos, bool spatial, int i)
{
	bool ret = false;

	if (!active)
		return false;

	uint channel;

	if (id > 0 && id <= fx.size())
	{
		if (spatial) {
			iPoint screenMid;
			myApp->win->GetWindowCenter((uint&)screenMid.x, (uint&)screenMid.y);
			screenMid = myApp->render->ScreenToWorld(screenMid.x, screenMid.y);

			fVec2 vec = GetVector2({ (float)screenMid.x, (float)screenMid.y }, pos);
			float distance = vec.GetMagnitude();

			if (distance < sfxAudioRadius) {	//TODO: 100 is a hardcoded value, shouldn't be
				std::list<Mix_Chunk*>::iterator it = fx.begin();
				it = next(fx.begin(), id - 1);
				channel = Mix_PlayChannel(i, *it, repeat);
				
				float vol = 100.0f - (100.0f * distance / sfxAudioRadius);

				ChangeChannelVolume(vol, channel);
			}
		}
		else {
			std::list<Mix_Chunk*>::iterator it = fx.begin();
			it = next(fx.begin(), id - 1);
			channel = Mix_PlayChannel(i, *it, repeat);
			SetChannelVolume(channel);
		}
	}

	return ret;
}

//Volume Control
//Master
void Audio::SetMasterVolume() const
{
	Mix_VolumeMusic(masterVolume * (musicVolume * MIX_MAX_VOLUME / 100) / 100);
	Mix_Volume(-1, masterVolume * (sfxVolume * MIX_MAX_VOLUME / 100) / 100);
}

void Audio::ChangeMasterVolume(uint vol)
{
	masterVolume = vol;
	SetMasterVolume();
}

//Music
uint Audio::SetMusicVolume() const
{
	return Mix_VolumeMusic(masterVolume * (musicVolume * MIX_MAX_VOLUME / 100) / 100);
}

uint Audio::ChangeMusicVolume(uint vol)
{
	musicVolume = vol;
	return SetMusicVolume();
}

//Channels
uint Audio::SetChannelVolume(int channel)
{
	return Mix_Volume(channel, masterVolume * (sfxVolume * MIX_MAX_VOLUME / 100) / 100);
}

uint Audio::ChangeChannelVolume(uint vol, int channel)
{
	if (channel < 0) {
		sfxVolume = vol;
	}

	return SetChannelVolume(channel);
}

//Sfx Chunks
uint Audio::SetSfxChunkVolume(uint vol, int id)
{
	if (id > 0 && id <= fx.size())
	{
		std::list<Mix_Chunk*>::iterator it = fx.begin();
		it = next(fx.begin(), id - 1);

		return Mix_VolumeChunk(*it, vol * MIX_MAX_VOLUME / 100);
	}
	else {
		return 0;
	}
}

//Legacy Lucho Methods
//void Audio::ControlVolume(int vol) { //Range: 0-128
//
//	Mix_Volume(-1, vol);
//
//}
//
//void Audio::ControlMUSVolume(int vol) { //Range: 0-128
//
//	Mix_VolumeMusic(vol);
//
//}
//
//void Audio::ControlSFXVolume(int vol) { //Range: 0-128
//
//	std::list<Mix_Chunk*>::iterator it = fx.begin();
//
//	for(; *it != NULL; it = next(it))
//		Mix_VolumeChunk(*it, vol);
//
//}

void Audio::FillArrayFX() {
	
	for (int i = 0; i < (int)infantry_type::INFANTRY_MAX; ++i) {
		for (int j = 0; j < FACTION_NUM; ++j) {
			for (int k = 0; k < (int)type_sounds::MAX; ++k) {
				for (int l = 0; l < VARIATION_PER_SOUND; ++l)
					SoundFX_Array[i][j][k][l]= -1;

			}
		}
	}

}

void Audio::LoadIntoArray() {


	pugi::xml_parse_result result = SFX_XML.load_file("SoundFX.xml");

	//SOV units Sound

	pugi::xml_node Data = SFX_XML.child("FX").child("Troops").child("SOV").child("Conscript");

	//CONSCRIPT---------------------------------------------------------------------------------------------------------------------
	//Shot 1 & 2
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::SHOT][0] = LoadFx(Data.child("Shot").child("Var_one").attribute("path").as_string());
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::SHOT][1] = LoadFx(Data.child("Shot").child("Var_two").attribute("path").as_string());
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::SHOT][2] = LoadFx(Data.child("Shot").child("Var_three").attribute("path").as_string());

	//Attack
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::ATTACK][0] = LoadFx(Data.child("Attack").child("Var_one").attribute("path").as_string());

	//Spawn
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::SPAWN][0] = LoadFx(Data.child("Spawn").child("Var_one").attribute("path").as_string());

	//Moving
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::MOVING][0] = LoadFx(Data.child("Moving").child("Var_one").attribute("path").as_string());
	
	//Hurt
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::HURT][0] = LoadFx(Data.child("Hurt").child("Var_one").attribute("path").as_string());
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::HURT][1] = LoadFx(Data.child("Hurt").child("Var_two").attribute("path").as_string());

	//Comfirmation order
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::COMFIRMATION][0] = LoadFx(Data.child("Comfirmation").child("Var_one").attribute("path").as_string());
	SoundFX_Array[(int)infantry_type::CONSCRIPT][SOV][(int)type_sounds::COMFIRMATION][1] = LoadFx(Data.child("Comfirmation").child("Var_two").attribute("path").as_string());


	//BASIC
	//Shot 1 & 2
	SoundFX_Array[(int)infantry_type::BASIC][CAP][(int)type_sounds::SHOT][0] = LoadFx(Data.child("Shot").child("Var_one").attribute("path").as_string());
	SoundFX_Array[(int)infantry_type::BASIC][CAP][(int)type_sounds::SHOT][1] = LoadFx(Data.child("Shot").child("Var_two").attribute("path").as_string());
	SoundFX_Array[(int)infantry_type::BASIC][CAP][(int)type_sounds::SHOT][2] = LoadFx(Data.child("Shot").child("Var_three").attribute("path").as_string());

	//Hurt
	SoundFX_Array[(int)infantry_type::BASIC][CAP][(int)type_sounds::HURT][0] = LoadFx(Data.child("Hurt").child("Var_one").attribute("path").as_string());
	SoundFX_Array[(int)infantry_type::BASIC][CAP][(int)type_sounds::HURT][1] = LoadFx(Data.child("Hurt").child("Var_two").attribute("path").as_string());

//----------------------------------------------------------------------------------------------------------------------------------


}