#ifndef POOL_H
#define POOL_H	//@Carles

#include <vector>

//Pool Item Struct
template <class TYPE>
struct pool_item {
	TYPE content;
	bool active = false;
};

// Pool Vector Class
template <class TYPE>
class Pool {

private:
	std::vector<pool_item<TYPE>> poolVec;
	unsigned int activeMembers;

public:
	Pool() : activeMembers(0) {}
	Pool(unsigned int startSize) : activeMembers(0)
	{
		poolVec.resize(startSize);

		for (int i = 0; i < poolVec.size(); i++) {
			poolVec[i].active = false;
		}
	}

	unsigned int GetActiveSize()
	{
		return activeMembers;
	}

	//Vector function mimics
	TYPE& operator[](unsigned int pos)	//Get element at pos
	{
		return poolVec[pos].content;
	}

	unsigned int size()	//Return pool size
	{
		return poolVec.size();
	}

	void clear()	//Clear pool
	{
		poolVec.clear();
		activeMembers = 0;
	}

	int find(TYPE& elemPtr)	//Find element pos
	{
		int ret = -1;

		for (int i = 0; i < poolVec.size(); i++) {
			if (&poolVec[i].content == &elemPtr) {
				ret = i;
				break;
			}
		}

		return ret;
	}

	TYPE at(unsigned int pos)	//Find element at pos
	{
		TYPE ret;

		if (pos < poolVec.size()) {
			ret = poolVec.at(pos).content;
		}

		return ret;
	}

	void resize(int newSize)	//Resize Pool
	{
		if (newSize < poolVec.size()) {
			for (int i = newSize; i < poolVec.size(); i++) {
				if (poolVec[i].active == true) {
					activeMembers--;
				}
			}

			poolVec.resize(newSize);
		}
		else if (newSize > poolVec.size()) {
			int prevSize = poolVec.size();

			poolVec.resize(newSize);

			for (int i = prevSize; i < poolVec.size(); i++) {
				poolVec[i].active = false;
			}
		}
	}
	
	//Pool Functions
	void fill(TYPE elem)	//Fill Pool with copies of a reference element
	{
		for (int i = 0; i < poolVec.size(); i++) {
			poolVec[i].content = elem;
		}
	}

	int activate(TYPE elem)	//Copy parameter data to one of the pool elements and mark it as active
	{
		int ret = -1;

		for (int i = 0; i < poolVec.size(); i++) {
			if (poolVec[i].active == false) {
				poolVec[i].content = elem;
				poolVec[i].active = true;
				activeMembers++;
				ret = i;
				break;
			}
		}

		return ret;
	}

	TYPE* deactivate(unsigned int pos)	//Deactivate element at given position
	{
		TYPE* ret = nullptr;

		if (pos < poolVec.size()) {
			poolVec[pos].active = false;
			ret = &poolVec[pos].content;
			activeMembers--;
		}

		return ret;
	}

	int deactivate(TYPE& elemPtr)	//Find element and deactivate it
	{
		int ret = find(elemPtr);

		if (ret >= 0) {
			poolVec[ret].active = false;
			activeMembers--;
		}

		return ret;
	}

	TYPE* reactivate(unsigned int pos)	//Mark the element in the given position as active
	{
		TYPE* ret = nullptr;

		if (pos < poolVec.size()) {
			poolVec[pos].active = true;
			ret = &poolVec[pos].content;
			activeMembers++;
		}

		return ret;
	}

	int reactivate(TYPE& elemPtr)	//Find element and mark it as active
	{
		int ret = find(elemPtr);

		if (ret >= 0) {
			poolVec[ret].active = true;
			activeMembers++;
		}

		return ret;
	}

	int FirstAvailableSpot()	//Return position of inactive element
	{
		int ret = -1;

		for (int i = 0; i < poolVec.size(); i++) {
			if (poolVec[i].active == false) {
				ret = i;
				break;
			}
		}

		return ret;
	}

	TYPE* FirstAvailableMember()	//Return address of inactive element
	{
		TYPE* ret = nullptr;

		for (int i = 0; i < poolVec.size(); i++) {
			if (poolVec[i].active == false) {
				ret = &poolVec[i].content;
				break;
			}
		}

		return ret;
	}

	bool IsActive(unsigned int pos)	//Check if element at given position is active
	{
		bool ret = false;

		if (pos < poolVec.size()) {
			ret = poolVec[pos].active;
		}
		
		return ret;
	}

	bool IsActive(TYPE& elemPtr)	//Check if given element is active, return false if not found in the pool
	{
		bool ret = false;

		int i = find(elemPtr);
		
		if (i >= 0) {
			ret = poolVec[i].active;
		}

		return ret;
	}
};

#endif	//POOL_H