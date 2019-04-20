//TODO: Vehicle Class

////ENTITIES TEST
//if (myApp->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
//{
//
//	iPoint position;
//	myApp->input->GetMousePosition(position.x, position.y);
//	position = myApp->render->ScreenToWorld(position.x, position.y);
//
//	fPoint position2;
//	position2.x = position.x;
//	position2.y = position.y;
//	myApp->entities->ActivateInfantry(position2, infantry_type::BASIC, entity_faction::COMMUNIST);
//}
//
//if (myApp->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
//{
//
//	iPoint position;
//	myApp->input->GetMousePosition(position.x, position.y);
//	position = myApp->render->ScreenToWorld(position.x, position.y);
//
//	fPoint position2;
//	position2.x = position.x;
//	position2.y = position.y;
//	myApp->entities->ActivateInfantry(position2, infantry_type::BASIC, entity_faction::CAPITALIST);
//}
//
//if (myApp->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
//{
//	iPoint mousePos;
//	myApp->input->GetMousePosition(mousePos.x, mousePos.y);
//	mousePos = myApp->render->ScreenToWorld(mousePos.x, mousePos.y);
//
//	for (int i = 0; i < INFANTRY_ARRAY_SIZE; ++i)
//	{
//		if (myApp->entities->CommunistInfantryArray[i]->active == true && myApp->entities->CommunistInfantryArray[i]->IsDead() == false)
//		{
//			myApp->entities->CommunistInfantryArray[i]->StartMoveAndAttack(mousePos);
//		}
//	}
//}