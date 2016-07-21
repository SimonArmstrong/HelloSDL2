#include "Actor.h"

Actor::Actor() {
	this->parent = nullptr;
	//this->localSpace = &WorldSpace;
	this->name = "";
}

Actor::Actor(const char * name) {
	this->parent = nullptr;
	//this->localSpace = &WorldSpace;
	this->name = name;
}

void Actor::Child(Actor* parent) {
	this->parent = parent;
	this->localSpace = parent->localSpace;
}
