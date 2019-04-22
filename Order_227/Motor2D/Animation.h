#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#define MAX_FRAMES 50

class Animation
{

public:

	bool loop = true;
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];

private:

	float current_frame;
	int last_frame = 0;
	int loops = 0;

public:

	void PushBack(const SDL_Rect& rect)
	{
		frames[last_frame++] = rect;
	}

	void operator=(Animation* ref)
	{
		Reset();

		speed = ref->speed;
		last_frame = ref->last_frame;
		loop = ref->loop;

		for (int i = 0; i < last_frame; i++) {
			frames[i] = ref->frames[i];
		}
	}

	SDL_Rect& AdvanceAnimation(float dt)
	{
		current_frame += speed * dt;

		if (current_frame >= last_frame) {

			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}

		return frames[(int)current_frame];
	}
	
	SDL_Rect& GetTheActualCurrentFrame()	//TODO: Change name to Get_Current_Frame without the "_"
	{
		return frames[(int)current_frame];
	}

	SDL_Rect& GetCurrentFrame(float dt)	//TODO: Erase this unintuitive function and wherever its used put AdvanceAnimation
	{
		current_frame += speed;

		if (current_frame >= last_frame) {

			current_frame = (loop) ? 0.0f : last_frame - 1;
			loops++;
		}
		return frames[(int)current_frame];
	}

	const float GetFramePosition() const { return current_frame; }

	bool Finished() const { return loops > 0; }

	void Reset()
	{
		current_frame = 0;
		loops = 0;

	}
};

#endif
