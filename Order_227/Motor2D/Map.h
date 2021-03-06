#ifndef MAP_H
#define MAP_H

#include "PugiXml/src/pugixml.hpp"
#include "Module.h"

#include "TileQuadtree.h"

struct Properties
{
	struct Property
	{
		std::string name;
		int value;
	};

	~Properties()
	{
		std::list<Property*>::iterator item = list.begin();

		while(item != list.end())
		{
			RELEASE(*item)
			item = next(item);
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	std::list<Property*>	list;
};

struct GameObjectGroup
{

	std::string nameGroup;


	struct Object
	{
		std::string name;
		float x, y, width, height;
		int id;
		Properties PropObj;

	};


	~GameObjectGroup()
	{
		std::list<Object*>::iterator itemP;
		itemP = Objectlist.begin();

		while (itemP != Objectlist.end())
		{
			RELEASE(*itemP);
			itemP = next(itemP);
		}

		Objectlist.clear();
	}

	std::list<Object*>	Objectlist;


};
// ----------------------------------------------------
struct MapLayer
{
	std::string	name;
	int			width;
	int			height;
	TileQuadtree*	tile_tree;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	std::string			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	//list
	std::list<TileSet*>	tilesets;
	std::list<MapLayer*>layers;
	std::list<GameObjectGroup *>gameObjects;
	
};

// ----------------------------------------------------
class Map : public Module
{
public:

	friend TileQuadtree;

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	bool Start();

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint MapToWorld(iPoint position) const;

	iPoint WorldToMap(int x, int y) const;
	iPoint WorldToMap(iPoint position) const;


	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;
	void PlaceGameObjects();
	iPoint PointToTile(int x,int y);

	TileSet* GetTilesetFromTileId(int id) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	bool LoadGameObjects(pugi::xml_node& node, GameObjectGroup*ObjGroup);

public:

	MapData data;
	
	bool mapDebugDraw = false;
	SDL_Texture* debug_tex = nullptr;

private:

	pugi::xml_document	map_file;
	std::string			folder;
	bool				map_loaded;
};

#endif // MAP_H