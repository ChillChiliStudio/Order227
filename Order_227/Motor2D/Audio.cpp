#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Audio.h"
#include "Unit.h"
#include "Building.h"
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

	//Setup radius audio
	uPoint center, size;
	myApp->win->GetWindowSize(size.x, size.y);
	myApp->win->GetWindowCenter(center.x, center.y);

	sfxAudioRadius = size.x;	//Effective radius of each ear
	earOffset = 0; /*size.x / 8;*/	//Used to move slighlty the ear positions towars (+) or against (-) the center

	leftEar = { (uint)(center.x - size.x / 4 + earOffset), center.y };	//Left Ear screen pos
	rightEar = { (uint)(center.x + size.x / 4 - earOffset), center.y };	//Right Ear screen pos

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

	int channel;

	if (id > 0 && id <= fx.size())
	{
		if (spatial) {	//Spatial audio checks the distance between a sound and the players "ears" positions in World and changes the volume accordingly
			iPoint worldLeft;
			iPoint worldRight;

			worldLeft = myApp->render->ScreenToWorld(leftEar.x, leftEar.y);
			worldRight = myApp->render->ScreenToWorld(rightEar.x, rightEar.y);

			float leftEarDistance = GetDistance({ (float)worldLeft.x, (float)worldLeft.y }, pos);
			float rightEarDistance = GetDistance({ (float)worldRight.x, (float)worldRight.y }, pos);

			if (leftEarDistance < sfxAudioRadius || leftEarDistance < sfxAudioRadius) {

				std::list<Mix_Chunk*>::iterator it = fx.begin();
				it = next(fx.begin(), id - 1);
				channel = Mix_PlayChannel(i, *it, repeat);

				if (channel > -1) {
					float leftVol = 0.0f;
					float rightVol = 0.0f;

					//Formula: Ear % Volume + Global % Sfx Volume + 0 to 255 ratio
					if (leftEarDistance < sfxAudioRadius) {
						leftVol = (1.0f - leftEarDistance / sfxAudioRadius) * sfxVolume / 100.0f * 255.0f;
					}
					if (rightEarDistance < sfxAudioRadius) {
						rightVol = (1.0f - rightEarDistance / sfxAudioRadius) * sfxVolume / 100.0f * 255.0f;
					}

					if (!Mix_SetPanning(channel, leftVol, rightVol)) {
						LOG("Mix_SetPanning: %s\n", Mix_GetError());
					}
				}
			}
		}
		else {
			std::list<Mix_Chunk*>::iterator it = fx.begin();
			it = next(fx.begin(), id - 1);
			channel = Mix_PlayChannel(i, *it, repeat);

			if (channel > -1) {
				SetChannelVolume(channel);
			}
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
	uint ret;

	if (channel < 0) {
		sfxVolume = vol;
		ret = SetChannelVolume(channel);
	}
	else {
		ret = Mix_Volume(channel, masterVolume * (vol * MIX_MAX_VOLUME / 100) / 100);
	}

	return ret;
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
//  ControlMUSVolume(vol);
//	ControlSFXVolume(vol);
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
		
			for (int k = 0; k < (int)TroopType_Sounds::MAX; ++k) {
				for (int l = 0; l < VARIATION_PER_SOUND; ++l)
					SoundTroops_Array[i][k][l]= -1;

			}
		
	}

	for (int i = 0; i < (int)building_type::BUILDING_MAX; ++i) {

		for (int k = 0; k < (int)BuildingsType_Sounds::MAX; ++k) {
			for (int l = 0; l < VARIATION_PER_SOUND; ++l)
				SoundTroops_Array[i][k][l] = -1;

		}

	}

}

void Audio::LoadIntoArray() {


	pugi::xml_parse_result result = SFX_XML.load_file("SoundFX.xml");

	//SOV units Sound	

		for (pugi::xml_node DataUnit = SFX_XML.child("FX").child("Troops").child("Unit"); DataUnit != NULL; DataUnit = DataUnit.next_sibling("Unit")) {

			int id = DataUnit.attribute("id").as_int();
			
		
			for (pugi::xml_node SoundType = DataUnit.child("Sound"); SoundType != NULL; SoundType = SoundType.next_sibling("Sound")) {

				int TypeSound = SoundType.attribute("id").as_int();
				VarsXsound[id][TypeSound]=SoundType.attribute("NumVariation").as_int();

				for (pugi::xml_node DataSound = SoundType.child("Var"); DataSound != NULL; DataSound = DataSound.next_sibling("Var")) {

					int Variation = DataSound.attribute("Variation").as_int();

					if (Variation < VARIATION_PER_SOUND)
						SoundTroops_Array[id][TypeSound][Variation] = LoadFx(DataSound.attribute("path").as_string());


				}
			}
		}
	


		for (pugi::xml_node DataBuild = SFX_XML.child("FX").child("Building").child("Estructure"); DataBuild != NULL; DataBuild = DataBuild.next_sibling("Estructure")) {
		
			int id = DataBuild.attribute("id").as_int();

			for (pugi::xml_node SoundType = DataBuild.child("Sound"); SoundType != NULL; SoundType = SoundType.next_sibling("Sound")) {
				
				int TypeSound = SoundType.attribute("id").as_int();
				 VarsXsound_Buildings[id][TypeSound] = SoundType.attribute("NumVariation").as_int();

				for (pugi::xml_node DataSound = SoundType.child("Var"); DataSound != NULL; DataSound = DataSound.next_sibling("Var")) {

					int Variation = DataSound.attribute("Variation").as_int();

					if (Variation < VARIATION_PER_SOUND)
					 SoundBuilding_Array[id][TypeSound][Variation]=LoadFx(DataSound.attribute("path").as_string());;
				
				
				}

			}
		}



}