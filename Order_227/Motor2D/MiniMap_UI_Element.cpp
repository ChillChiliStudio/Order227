#include "MiniMap_UI_Element.h"
#include "Render.h"
#include "UserInterface.h"
#include "MiniMap.h"


MiniMap_UI::MiniMap_UI(ui_type type)
	:UI_Element(type, center)
{

}


bool MiniMap_UI::Update(float dt) {

	

	return true;
}


bool MiniMap_UI::PostUpdate() {

	return true;
}


bool MiniMap_UI::Draw() {
	if(myApp->gui->MainMenuTemp_Image->active==false)
		myApp->minimap->Draw();
	return true;
}

bool MiniMap_UI::DebugDraw() const
{
	return true;
}

