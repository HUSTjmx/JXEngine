#pragma once

template<class BeingCounted>
class Counted
{

public:

	class TooManyObjects{}; //用来抛出异常

	static int ObjectCount() { return numObjects; }

	int Index() { return index; }

protected:

	Counted();
	
	Counted(const Counted& rhs);

	~Counted() { --numObjects; }

private:

	static int numObjects;

	static const size_t maxObjects;

	int index;

	void init();
};


template<class BeingCounted>
int Counted<BeingCounted>::numObjects;

template<class BeingCounted>
inline Counted<BeingCounted>::Counted()
{
	init();
}

template<class BeingCounted>
inline Counted<BeingCounted>::Counted(const Counted& rhs)
{
	init();
}

template<class BeingCounted>
inline void Counted<BeingCounted>::init()
{
	if (numObjects >= maxObjects)throw TooManyObjects();
	index = numObjects;
	++numObjects;
}
