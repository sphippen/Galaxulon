// ObjectAllocator.h
//
// Authors: Spencer Phippen, Aric Parkinson
// Date created: March 30, 2012
// 
// -------
// Contains class interface for BulletContainer

#ifndef OBJECT_ALLOCATOR_H_
#define OBJECT_ALLOCATOR_H_

#include "tonc_types.h"
#include "assert2.h"

// Provides interface to contain a given number of bullets, allocating and deallocating them
// as the user requests. The ordering of Bullets in the container is undefined - do not expect
// any particular order
template <typename T, int SIZE>
class ObjectAllocator
{
public:
	class iterator;
	// Constructs a new ObjectAllocator object.
	ObjectAllocator();

	// Initializes each object's sprite OBJ_ATTR*
	void spriteInit(OBJ_ATTR* imgs);

	// Unitializes the sprite of each object in the allocator
	void spriteDestroy();

	// Returns an iterator that iterates over all objects.
	iterator begin();

	// Allocates a provided number of objects to the user. Returns an iterator to access these
	// objects.
	iterator alloc(int numToAlloc);

	// Iterates over a ObjectAllocator object. Allows iterating over or deleting objects.
	class iterator
	{
	public:
		// Returns this object, and moves to the next one. Returns NULL when the
		// end has been reached.
		T* next();
		// Deallocates the last returned object, and keeps the iterator in the same place.
		void dealloc();
		// Access the object stored at the current iteration position
		T* data();
	private:
		iterator(ObjectAllocator* _parent, int iterStart);

		ObjectAllocator* const parent;
		int curPos;

		friend class ObjectAllocator;
	};

private:
	T objects[SIZE];
	T* ptrs[SIZE];
	int top;
};

// IMPLEMENTATION //
template<typename T, int SIZE>
ObjectAllocator<T, SIZE>::ObjectAllocator() : top(0)
{
	for (int i = 0; i < SIZE; ++i)
	{
		ptrs[i] = &objects[i];
	}
}

template<typename T, int SIZE>
void ObjectAllocator<T, SIZE>::spriteInit(OBJ_ATTR* imgs)
{
	for (int i = 0; i < SIZE; ++i)
	{
		objects[i].useImg(imgs++);
	}
}

template<typename T, int SIZE>
void ObjectAllocator<T, SIZE>::spriteDestroy()
{
	for (int i = 0; i < SIZE; ++i)
	{
		objects[i].unuseImg();
	}
}

template<typename T, int SIZE>
typename ObjectAllocator<T, SIZE>::iterator ObjectAllocator<T, SIZE>::alloc(int numToAlloc)
{
	assert2(numToAlloc != 0, "cannot allocate 0 objects");

	// We don't have enough room to allocate more objects, so return
	// an allocator pointing past the end
	if (top + numToAlloc > SIZE)
	{
		return ObjectAllocator<T, SIZE>::iterator(this, top);
	}

	int iterBegin = top;
	top += numToAlloc;
	return ObjectAllocator<T, SIZE>::iterator(this, iterBegin);
}

template<typename T, int SIZE>
typename ObjectAllocator<T, SIZE>::iterator ObjectAllocator<T, SIZE>::begin()
{
	return ObjectAllocator<T, SIZE>::iterator(this, 0);
}

template<typename T, int SIZE>
ObjectAllocator<T, SIZE>::iterator::iterator(ObjectAllocator<T, SIZE>* _parent, int iterStart) 
	: parent(_parent), curPos(iterStart) { }

template<typename T, int SIZE>
T* ObjectAllocator<T, SIZE>::iterator::next()
{	
	// If we're at the end of the iterator, just return NULL
	if(curPos >= parent->top)
		return 0;

	T* toReturn = parent->ptrs[curPos];
	curPos++;
	return toReturn;
}

template<typename T, int SIZE>
void ObjectAllocator<T, SIZE>::iterator::dealloc()
{
	assert2((curPos-1) < parent->top, "attempted to deallocate element that was not allocated");
	assert2(curPos >= 1, "called dealloc without calling next");

	// Swap the pointer at top with the pointer at curPos
	T* tmp = parent->ptrs[curPos - 1];
	parent->ptrs[curPos - 1] = parent->ptrs[parent->top - 1];
	parent->ptrs[parent->top - 1] = tmp;

	// Decrement top and curpos, to still iterate over the object we just swapped down
	--(parent->top);
	--curPos;
}

template<typename T, int SIZE>
T* ObjectAllocator<T, SIZE>::iterator::data()
{
	if(curPos >= parent->top)
		return 0;
	
	return parent->ptrs[curPos];
}

#endif