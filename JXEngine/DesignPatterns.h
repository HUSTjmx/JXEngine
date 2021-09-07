#pragma once
#include <vector>

#define MAX_OBSERVERS 8

class Event
{
public:
	virtual ~Event() {}
};

//观察者模式

class Observer
{
public:
	virtual ~Observer() {}
	virtual void OnNotify(Event* event) = 0;
};

class Subject
{
public:
	Subject() : numObservers_(0) {}

	void addObserver(Observer* observer);

	void removeObserver(Observer* observer);

	void notify(Event* event);

private:
	std::vector<Observer*> observers_;
	int numObservers_;
};