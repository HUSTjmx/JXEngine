#include "DesignPatterns.h"
#include <algorithm>
#include <iostream>

void Subject::addObserver(Observer* observer)
{
	observers_.push_back(observer);
	numObservers_++;
}

void Subject::removeObserver(Observer* observer)
{
	observers_.erase(std::find(observers_.begin(), observers_.end(), observer));
}

void Subject::notify(Event* event)
{
	//std::cout << numObservers_ << std::endl;
	for (int i = 0; i < numObservers_; i++)
	{
		observers_[i]->OnNotify(event);
	}
}
