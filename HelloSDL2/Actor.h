#pragma once


#include <Vector3.h>

//Matrix3 WorldSpace;

class Actor {
private:
	Actor* parent = nullptr;
	const char* name;

public:
	Actor();
	Actor(const char* name);
	Matrix3* localSpace;

	void Child(Actor* parent);
};