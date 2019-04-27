#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Scene.h"
#include "Input.h"
#include "Building.h"
#include <cmath>
#include <sstream>
#include "Horde_Manager.h"

class Spawning_Point;


Map::Map() : Module(), map_loaded(false)
{
	name.assign("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder = config.child("folder").child_value();

	return ret;
}

bool Map::Start()
{
	bool ret = true;

	//Default Debug Tile Texture
	debug_tex = myApp->tex->Load("maps/path2.png");

	return ret;
}

void Map::Draw()
{
	if(map_loaded == false)
		return;

	std::list<MapLayer*>::iterator item = data.layers.begin();

	for(; item != data.layers.end(); item = next(item))
	{
		MapLayer* layer = *item;

		if (layer->properties.Get("Nodraw") != 1 || mapDebugDraw) {
			layer->tile_tree->DrawMap();
		}

		if (mapDebugDraw) {
			layer->tile_tree->DrawQuadtree();

			iPoint mousePos;
			myApp->input->GetMousePosition(mousePos.x, mousePos.y);
			mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);
			mousePos = myApp->map->WorldToMap(mousePos.x, mousePos.y);
			mousePos = myApp->map->MapToWorld(mousePos.x, mousePos.y);

			myApp->render->Blit(debug_tex, mousePos.x, mousePos.y);
		}
	}
}

int Properties::Get(const char* value, int default_value) const
{
	std::list<Property*>::const_iterator item = list.begin();

	for(; item != list.end(); item = next(item))
		if((*item)->name == value)
			return (*item)->value;

	return default_value;
}

TileSet* Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>::const_iterator item = data.tilesets.begin();
	TileSet* set = *item;

	while(item != data.tilesets.end())
	{
		if(id < (*item)->firstgid)
		{
			set = *item.operator--();
			break;
		}
		set = *item;
		item = next(item);
	}

	return set;
}

iPoint Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (int)((x - y) * (data.tile_width * 0.5f));
		ret.y = (int)((x + y) * (data.tile_height * 0.5f));
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::MapToWorld(iPoint position) const
{
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = position.x * data.tile_width;
		ret.y = position.y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (int)((position.x - position.y) * (data.tile_width * 0.5f));
		ret.y = (int)((position.x + position.y) * (data.tile_height * 0.5f));
	}
	else
	{
		LOG("Unknown map type");
		ret.x = position.x; ret.y = position.y;
	}

	return ret;
}

iPoint Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if(data.type == MAPTYPE_ISOMETRIC)
	{
		
		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2) - 1;
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint Map::WorldToMap(iPoint position) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = position.x / data.tile_width;
		ret.y = position.y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((position.x / half_width + position.y / half_height) / 2) - 1;
		ret.y = int((position.y / half_height - (position.x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = position.x; ret.y = position.y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	std::list<TileSet*>::iterator item = data.tilesets.begin();

	while(item != data.tilesets.end())
	{
		myApp->tex->UnLoad((*item)->texture);
		RELEASE(*item);
		item = next(item);
	}

	data.tilesets.clear();

	// Remove all layers
	std::list<MapLayer*>::iterator item2 = data.layers.begin();

	while(item2 != data.layers.end())
	{
		RELEASE(*item2);
		item2 = next(item2);
	}
	data.layers.clear();

	// Unload Debug Tile Texture
	myApp->tex->UnLoad(debug_tex);

	// Clean up the pugui tree
	map_file.reset();
	return true;
}

// Load new map
bool Map::Load(const char* file_name)
{
	bool ret = true;

	std::stringstream stream_to_assign;
	stream_to_assign << folder.c_str() << file_name;
	std::string tmp = stream_to_assign.str();

	pugi::xml_parse_result result = map_file.load_file(tmp.data());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.layers.push_back(lay);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		std::list<TileSet*>::iterator item = data.tilesets.begin();
		while(item != data.tilesets.end())
		{
			TileSet* s = *item;

			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.data(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = next(item);
		}

		std::list<MapLayer*>::iterator item_layer = data.layers.begin();
		while(item_layer != data.layers.end())
		{
			MapLayer* l = *item_layer;
			LOG("Layer ----");
			LOG("name: %s", l->name.data());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = next(item_layer);
		}
	}

	//Load GameObjectGroup Info-------------------------
	pugi::xml_node objectGame;
	for (objectGame = map_file.child("map").child("objectgroup"); objectGame && ret; objectGame = objectGame.next_sibling("objectgroup"))
	{
		GameObjectGroup*objGroup = new GameObjectGroup();
		ret = LoadGameObjects(objectGame, objGroup);

		if (ret == true)
			data.gameObjects.push_back(objGroup);

	}



	//place all game objects needed such as SpawningPoint,bases,etc.
	PlaceGameObjects();


	map_loaded = ret;
	return ret;
}

// Load map general properties
bool Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.length() > 0)
		{
			std::string	 red, green, blue;
			bg_color.substr(1, 2);
			bg_color.substr(3, 4);
			bg_color.substr(5, 6);

			int v = 0;

			sscanf_s(red.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.assign(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = myApp->tex->Load(PATH(folder.data(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	iPoint layer_size;
	iPoint quadT_position(0, 0);
	switch (data.type)
	{
	case MAPTYPE_ORTHOGONAL:
		layer_size.x = layer->width*myApp->map->data.tile_width;
		layer_size.y = layer->height*myApp->map->data.tile_height;
		quadT_position.x = 0;
		break;
	case MAPTYPE_ISOMETRIC:
		layer_size.x = (int)((layer->width + layer->height)*(myApp->map->data.tile_width *0.5f));
		layer_size.y = (int)((layer->width + layer->height + 1) * (data.tile_height *0.5f));
		quadT_position.x = -layer_size.x + ((layer->width + 1)*myApp->map->data.tile_width / 2);
		break;
	}
	layer->tile_tree = new TileQuadtree(6, { quadT_position.x, 0, layer_size.x,layer_size.y }, layer->width*layer->height * 3);

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			iPoint tile_map_coordinates(myApp->map->MapToWorld((i - int(i / layer->width)*layer->width), int(i / layer->width)));
			TileData new_tile(tile.attribute("gid").as_int(0), tile_map_coordinates);
			if (new_tile.id!=0)
			layer->tile_tree->InsertTile(new_tile);

			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if(data != NULL)
	{
		pugi::xml_node prop;

		for(prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.push_back(p);
		}
	}

	return ret;
}

bool Map::LoadGameObjects(pugi::xml_node& node, GameObjectGroup*ObjGroup) {

	bool ret = true;

	ObjGroup->nameGroup = node.attribute("name").as_string();
	for (pugi::xml_node obj = node.child("object"); obj && ret; obj=obj.next_sibling("object"))
	{

		GameObjectGroup::Object*objectAux = new GameObjectGroup::Object();

		objectAux->id = obj.attribute("id").as_int();
		objectAux->name = obj.attribute("name").as_string();
		objectAux->x = obj.attribute("x").as_float();
		objectAux->y = obj.attribute("y").as_float();
		objectAux->width = obj.attribute("width").as_float();
		objectAux->height = obj.attribute("height").as_float();
		
		LoadProperties(obj, objectAux->PropObj);
	


		ObjGroup->Objectlist.push_back(objectAux);

	}


	
	

	return ret;
}

//Basically here we fullfill a map array with 0s and 1s (same than we did with the map[X][Y] but better
bool Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{

	bool ret = false;
	std::list<MapLayer*>::const_iterator item = data.layers.begin();

	for(; item != data.layers.end(); item = next(item))
	{
		MapLayer* layer = *item;

		if(layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);
		
		for(int y = 0; y < data.height; ++y)
		{
			for(int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL; //If tile id > 0, tileset = tileset from a tile id. Else, tileset = NULL
				
				if(tileset != NULL)
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1; //Here, if (tile id - tileset 1stGID) > 0, map[i] = 0. Else is 1
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

void Map::PlaceGameObjects() {

	std::list<GameObjectGroup*>::iterator item = data.gameObjects.begin();
	for (; item != data.gameObjects.end(); item = next(item)) {

		if ((*item)->nameGroup == "spawnPoint") {

			std::list<GameObjectGroup::Object*>::iterator item2 = (*item)->Objectlist.begin();
			for (; item2 != (*item)->Objectlist.end(); item2 = next(item2)) {

				if ((*item2)->name == "Spawn") {

					Spawning_Point* new_SP = new Spawning_Point(PointToTile((int)(*item2)->x, (int)(*item2)->y));
					myApp->hordes->SpawningPoints_Array.push_back(new_SP);
					Group* newHorde = new Group();
					myApp->hordes->hordes.push_back(newHorde);

				}
			}
		}

		 if ((*item)->nameGroup == "Buildings") {

			std::list<GameObjectGroup::Object*>::iterator item2 = (*item)->Objectlist.begin();
			for (; item2 != (*item)->Objectlist.end(); item2 = next(item2)) {

				if ((*item2)->name == "MainBase") {

					iPoint pos = PointToTile((int)(*item2)->x, (int)(*item2)->y);
					fPoint fPos = { (float)pos.x, (float)pos.y };

					Building* newBuilding = new Building(fPos, building_type::BUILDING_NONE, entity_faction::COMMUNIST);

					std::list <Properties::Property*> ::iterator itemProp = (*item2)->PropObj.list.begin();
					for (; itemProp != (*item2)->PropObj.list.end();  itemProp = next(itemProp)) {

						if ((*itemProp)->name == "income")
							newBuilding->income = (*itemProp)->value;
						if ((*itemProp)->name == "health")
							newBuilding->health = (*itemProp)->value;
						if ((*itemProp)->name == "type")
							newBuilding->buildingType == (building_type)(*itemProp)->value;
						
					}
					
					myApp->entities->BuildingsList.push_back(newBuilding);
				}
			}
		}

		 //TODO:Draw threes depending on the terrain tile

		 if ((*item)->nameGroup == "Trees") {

			 int i = 0;

			 std::list<GameObjectGroup::Object*>::iterator item3 = (*item)->Objectlist.begin();
			 for (; item3 != (*item)->Objectlist.end(); item3 = next(item3)) {

				 if ((*item3)->name == "Tree") {

					 iPoint Aux = PointToTile((int)(*item3)->x, (int)(*item3)->y);
					 fPoint fPos = fPoint(Aux.x, Aux.y);

					 Static_Object *newStaticObject = new Static_Object(fPos, object_type::TREE, entity_faction::NEUTRAL);

					 myApp->entities->ObjectsArray[i] = newStaticObject;
					 i++;

					 if (i >= OBJECTS_INITIAL_SIZE)
						 break;
				 }
			 }
		 }


	}
}

iPoint Map::PointToTile(int x, int y) {
	
	iPoint Aux;	
	
	//harcoded need to clean

		/*	int w=data.tile_width/2;
			int h=data.tile_height/2;

			int Hypot = sqrt(w * w + h * h);*/

			Aux = {x/30,y/30 };

		
	return MapToWorld(Aux.x, Aux.y);
}