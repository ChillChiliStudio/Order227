#ifndef POOL_H
#define POOL_H

#include <vector>
#include <array>
#include "Defs.h"

template <class TYPE>
class Pool {

	Pool() : activeMembers(0) {}
	Pool(uint startSize) : activeMembers(0)
	{
		elementVector.resize(startSize);
		activeVector.resize(startSize);

		for (int i = 0; i < activeVector.size(); i++) {
			activeVector[i] = false;
		}
	}

	TYPE operator[](uint pos)	//Get element at pos
	{
		return elementVector[pos];
	}

	void resize(int newSize)	//Resize Pool
	{
		if (newSize < elementVector.size()) {
			for (int i = newSize; i < activeVector.size(); i++) {
				if (activeVector[i] == true) {
					activeMembers--;
				}
			}

			elementVector.resize(newSize);
			activeVector.resize(newSize);
		}
		else if (newSize > elementVector.size()) {
			int prevSize = activeVector.size();

			elementVector.resize(newSize);
			activeVector.resize(newSize);

			for (int i = prevSize; i < activeVector.size(); i++) {
				activeVector[i] = false;
			}
		}
	}

	int find(TYPE& elemPtr)	//Find element pos
	{
		int ret = -1;

		for (int i = 0; i < elementVector.size(); i++) {
			if (&elementVector[i] == &elemPtr) {
				ret = i;
				break;
			}
		}

		return ret;
	}

	TYPE at(uint pos)	//Find element at pos
	{
		TYPE ret;

		if (pos < elementVector.size()) {
			ret = elementVector.at(pos);
		}

		return ret;
	}
	
	void fill(TYPE elem)	//Fill Pool with copies of a reference element
	{
		for (int i = 0; i < elementVector.size(); i++) {
			elementVector[i] = elem;
		}
	}

	int activate(TYPE elem)	//Copy parameter data to one of the pool elements and mark it as active
	{
		int ret = -1;

		for (int i = 0; i < activeVector.size(); i++) {
			if (activeVector[i] == false) {
				elementVector[i] = elem;
				activeVector[i] = true;
				activeMembers++;
				ret = i;
				break;
			}
		}

		return ret;
	}

	TYPE* deactivate(uint pos)	//Deactivate element at given position
	{
		TYPE* ret = nullptr;

		if (pos < activeVector.size()) {
			activeVector[pos] = false;
			ret = &elementVector[pos];
			activeMembers--;
		}

		return ret;
	}

	int deactivate(TYPE& elemPtr)	//Find element and deactivate it
	{
		int ret = find(elemPtr);

		if (ret >= 0) {
			activeVector[ret] = false;
			activeMembers--;
		}

		return ret;
	}

	TYPE* reactivate(uint pos)	//Mark the element in the given position as active
	{
		TYPE* ret = nullptr;

		if (pos < activeVector.size()) {
			activeVector[pos] = true;
			ret = &elementVector[pos];
			activeMembers++;
		}

		return ret;
	}

	int reactivate(TYPE& elemPtr)	//Find element and mark it as active
	{
		int ret = find(elemPtr);

		if (ret >= 0) {
			activeVector[ret] = true;
			activeMembers++;
		}

		return ret;
	}

	int FindAvailableSpot()	//Return position of inactive element
	{
		int ret = -1;

		for (int i = 0; i < activeVector.size(); i++) {
			if (activeVector[i] == false) {
				ret = i;
				break;
			}
		}

		return ret;
	}

	TYPE* FindAvailableElem()	//Return address of inactive element
	{
		TYPE* ret = nullptr;

		for (int i = 0; i < activeVector.size(); i++) {
			if (activeVector[i] == false) {
				ret = &elementVector[i];
				break;
			}
		}

		return ret;
	}

	bool IsActive(uint pos)	//Check if element at given position is active
	{
		bool ret = false;

		if (pos < activeVector.size()) {
			ret = activeVector[pos];
		}
		
		return ret;
	}

	bool IsActive(TYPE& elemPtr)	//Check if given element is active, return false if not found in the pool
	{
		bool ret = false;

		int i = find(elemPtr);
		
		if (i >= 0) {
			ret = activeVector[i];
		}

		return ret;
	}

	std::vector<TYPE> elementVector;
	std::vector<bool> activeVector;
	uint activeMembers;
};

#endif	//POOL_H