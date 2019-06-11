#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Fonts.h"
#include "Render.h"
#include "Window.h"
#include "Map.h"
#include "Audio.h"
#include "PathFinding.h"
#include "Entity_Manager.h"
#include "UserInterface.h"
#include "Horde_Manager.h"
#include "ParamBox.h"
#include "Scene.h"
#include "GroupManager.h"
#include "Player.h"

#include "Brofiler/Brofiler.h"

Scene::Scene() : Module()
{
	name.assign("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	return true;
}

// Called before the first frame
bool Scene::Start()
{

	srand((uint)time(NULL));
	if(myApp->map->Load("Map1_0.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(myApp->map->CreateWalkabilityMap(w, h, &data))
			myApp->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}


	//MUSIC
	LoadGameMusic();
	SwitchMusic(Screen_Type::SCREEN_MAINMENU);
	
	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	BROFILER_CATEGORY("Scene Post-Update", Profiler::Color::DarkGreen);

	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene Post-Update", Profiler::Color::Green);

	if (myApp->audio->MusicPlaying() == false)
		ManageMusic();

	if (myApp->hordes->roundNumber == myApp->hordes->maxHordes && myApp->hordes->HordesDead() && ActivateGameOverMusic == true) {

		myApp->gui->WinIcon->Activate();
		myApp->scene->SwitchMusic(Screen_Type::SCREEN_WIN);
		ActivateGameOverMusic = false;
		myApp->player->playerIncome = 0;
		myApp->gui->OnPause = true;
		myApp->player->UpdateText();
	}

	if(myApp->entities->mainBase != nullptr && myApp->entities->mainBase->health <= 0 && ActivateGameOverMusic == true) {

		myApp->gui->LoseIcon->Activate();
		myApp->scene->SwitchMusic(Screen_Type::SCREEN_LOSE);
		ActivateGameOverMusic = false;
		myApp->player->playerIncome = 0;
		myApp->gui->OnPause = true;
		myApp->player->UpdateText();

	}

	myApp->map->Draw();
	return true;
}


// Called each loop iteration
bool Scene::PostUpdate()
{
	BROFILER_CATEGORY("Scene Post-Update", Profiler::Color::DarkSeaGreen);

	bool ret = true;

	//if(myApp->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	//	ret = false;

	myApp->gui->Draw();

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	std::list<Music_Song*>::iterator item = gameSongslist.begin();
	for (; item != gameSongslist.end(); item = next(item)) {

		std::list<Music_Track*>::iterator item2 = (*item)->songtracks_list.begin();
		for (; item2 != (*item)->songtracks_list.end(); item2 = next(item2)) {

			if(*item2 != nullptr)
				RELEASE(*item2);
		}

		(*item)->songtracks_list.clear();

		if((*item) != nullptr)
			RELEASE(*item);
	}

	gameSongslist.clear();
	return true;
}


void Scene::ManageMusic() {

	myApp->audio->PlayMusic(current_track->path, current_track->num_loops, 0.0f);

	if (current_track->nextTrack == nullptr) {

		current_song = current_song->nextSong;
		current_track = (*current_song->songtracks_list.begin());
	}


	current_track = current_track->nextTrack;
}


bool Scene::SwitchMusic(Screen_Type ScreenToSwitch) {

	bool ret = false;
	//Screen_Type ScreenToSwitch = getCurrentScreen();
	song_type SongType_Switch;

	switch (ScreenToSwitch) {

	case Screen_Type::SCREEN_INGAME:
		SongType_Switch = song_type::IN_GAME;
		ret = true;
		break;
	case Screen_Type::SCREEN_MAINMENU:
		SongType_Switch = song_type::MAIN_MENU;
		ret = true;
		break;
	case Screen_Type::SCREEN_LOSE:
		SongType_Switch = song_type::LOSE;
		ret = true;
		break;
	case Screen_Type::SCREEN_WIN:
		SongType_Switch = song_type::WIN;
		ret = true;
		break;
	default:
		SongType_Switch = song_type::NONE;
		break;
	}

	if (ret == true) {

		std::list<Music_Song*>::iterator item = gameSongslist.begin();
		for (; item != gameSongslist.end(); item = next(item)) {

			if ((*item)->songType == SongType_Switch) {

				current_song = (*item);
				current_track = (*current_song->songtracks_list.begin());
				break;
			}
		}

		ManageMusic();
	}

	return ret;
}


bool Scene::LoadGameMusic() {

	bool ret = false;

	pugi::xml_parse_result result = Music_Doc.load_file("audio/musicDoc.xml");

	if (result == NULL)
		LOG("Couldn't Load music Document! ERROR");
	else {

		pugi::xml_node Data = Music_Doc.child("Music_Document").child("song");
		for (; Data; Data = Data.next_sibling("song")) {

			Music_Song *newSong = new Music_Song();

			newSong->songName = Data.attribute("name").as_string("NULL");
			newSong->songType = (song_type)Data.attribute("type").as_int(-1);
			newSong->nextSong_id = (char*)Data.attribute("next_song").as_string("NULL");

			pugi::xml_node track_node = Data.child("track");
			for (; track_node; track_node = track_node.next_sibling("track")) {

				Music_Track *newTrack = new Music_Track();
				newTrack->num_loops = track_node.attribute("loops").as_int(0);
				newTrack->trackName = track_node.attribute("trackname").as_string("NULL");
				newTrack->path = (char*)track_node.attribute("path").as_string("NULL");
				newTrack->nextTrack_id = (char*)track_node.attribute("next_track").as_string("NULL");

				newSong->songtracks_list.push_back(newTrack);
			}

			std::list<Music_Track*>::iterator trackItem = newSong->songtracks_list.begin();
			for (; trackItem != newSong->songtracks_list.end(); trackItem = next(trackItem)) {

				std::list<Music_Track*>::iterator trackItem2 = newSong->songtracks_list.begin();
				for (; trackItem2 != newSong->songtracks_list.end(); trackItem2 = next(trackItem2)) {

					if ((*trackItem)->nextTrack_id == (*trackItem2)->trackName) {

						(*trackItem)->nextTrack = (*trackItem2);
						break;
					}
				}
			}

			gameSongslist.push_back(newSong);
		}


		std::list<Music_Song*>::iterator songItem = gameSongslist.begin();
		for (; songItem != gameSongslist.end(); songItem = next(songItem)) {

			std::list<Music_Song*>::iterator songItem2 = gameSongslist.begin();
			for (; songItem2 != gameSongslist.end(); songItem2 = next(songItem2)) {

				if ((*songItem)->nextSong_id == (*songItem2)->songName) {

					(*songItem)->nextSong = (*songItem2);
					break;
				}
			}
		}
		/*std::list<Music_Track*>::iterator trackItem = newSong.songtracks_list.begin();
		for (; (*trackItem); trackItem = next(trackItem)) {

			std::list<Music_Track*>::iterator trackItem2 = newSong.songtracks_list.begin();
			for (; (*trackItem2); trackItem2 = next(trackItem2)) {

				if ((*trackItem)->nextTrack->trackName == (*trackItem2)->trackName) {

					(*trackItem)->nextTrack = (*trackItem2);
					break;
				}

			}
		}*/


		ret = true;

		/*std::list<Music_Song*>::iterator item = gameSongslist.begin();
		for (; *item; item = next(item)) {

			if ((*item)->songType == song_type::IN_GAME) {

				current_song = (*item);
				current_track = *((*item)->songtracks_list.begin());
				break;
			}
		}*/
	}

	return ret;
}
