#pragma once
#include <Matrix3.h>
#include <Vector3.h>
#include <SDL.h>
#include <iostream>
#include <fstream>
#include <SDL_image.h>
#include <vector>
#include "AI.h"

Matrix3 world;
const int scaleFactor = 3;

/*struct Object {
	Matrix3 translation;
	Matrix3 matRotation;
	Matrix3 matScale;

	Matrix3 localSpace = matScale * matRotation * translation;

	Vector3 position;
	Vector3 rotation;
	Vector3 scale;

	SDL_Texture* tex;

	SDL_Rect aabb;

	SDL_Point verts[4];

	float speed;

	Object(Vector3 p, Vector3 r, Vector3 s) {
		this->position = p;
		this->rotation = r;
		this->scale = s;
	}

	void Draw(SDL_Renderer* renderer) {
		//matRotation.setRotateZ(rotation.z);
		localSpace = matScale * matRotation * translation;
		localSpace.setRotateZ(rotation.z);

		scale = localSpace * scale;
		rotation = localSpace * rotation;
		position = localSpace * position;

		verts[0].x = position.x;
		verts[0].y = position.y;
		verts[3].x = position.x + scale.x;
		verts[3].y = position.y;

		verts[2].x = position.x + scale.x;
		verts[2].y = position.y + scale.y;
		verts[1].x = position.x;
		verts[1].y = position.y + scale.y;

		for (int i = position.x; i < scale.x + position.x; i++) {
			for (int j = position.y; j < scale.y + position.y; j++) {
				//SDL_SetRenderDrawColor(renderer, 1, j, -j, 255);
				SDL_SetRenderDrawColor(renderer, i * rand() % 255, j * rand() % 255, -i * rand() % 255, 55);
				//SDL_SetRenderDrawColor(renderer, 0, j * rand() % 255, 0, 0);
				SDL_RenderDrawLine(renderer, i, j, i, j);
			}
		}

	}
};

Object box(Vector3(0, 0, 1), Vector3(0, 0, 1), Vector3(200, 200, 1));
*/
/*struct Actor {
	Matrix3 translation;
	Matrix3 rotation;
	Matrix3 trans;
	Matrix3 scl;

	Vector3 position;
	Vector3 scaleV;

	Actor(Vector3 p, Vector3 s) { this->position = p; this->scaleV = s; }

	void Draw(SDL_Renderer* renderer) {
		static float rot = 0;
		rot = 0.00005f;

		translation = scl * trans * rotation;
		translation.setRotateZ(rot);
		position = translation * position;



		SDL_RenderDrawLine(renderer, position.x, position.y, position.x + scaleV.x, position.y);
		SDL_RenderDrawLine(renderer, position.x, position.y, position.x, translation.m12);
	}
};
*/

typedef Uint32 Layer;

SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
	IMG_Init(IMG_INIT_PNG);
	SDL_Texture* rTex = nullptr;
	SDL_Surface* loadedSurface = IMG_Load(path);

	if (loadedSurface == nullptr) {
		//printf("Error loading image: \"", path, "\"");
	}
	else {
		rTex = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		SDL_FreeSurface(loadedSurface);
	}

	return rTex;
}

class GameObject {
private:
	const char* tex_path;

	Matrix3 globalMatrix;
	Matrix3 localMatrix;
	Matrix3 Rotation;
	Matrix3 Scale;
	
	Vector3 imageScale;

	SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
		IMG_Init(IMG_INIT_PNG);
		SDL_Texture* rTex = nullptr;
		SDL_Surface* loadedSurface = IMG_Load(path);

		if (loadedSurface == nullptr) {
			//printf("Error loading image: \"", path, "\"");
		}
		else {
			rTex = SDL_CreateTextureFromSurface(renderer, loadedSurface);

			scale.x = (float)loadedSurface->w * scaleFactor;
			scale.y = (float)loadedSurface->h * scaleFactor;

			SDL_FreeSurface(loadedSurface);
		}

		return rTex;
	}


	Layer layer;
public:	
	// [path]: The file location of the texture of this object
	SDL_Point center;
	SDL_Rect aabb;
	SDL_Texture* tex;

	Vector3 position;
	Vector3 scale;

	GameObject* parent;
	std::vector<GameObject*> child;
	
	GameObject() { this->tex = nullptr; }

	GameObject(const char* path) {
		tex_path = path;
		SDL_Surface* tempSurf = IMG_Load(path);
		
		if (tempSurf != nullptr) {
			this->scale.x = (float)tempSurf->w;
			this->scale.y = (float)tempSurf->h;
		}
	}

	~GameObject() {
		if(tex != nullptr)
			SDL_DestroyTexture(tex);
	}

	AI::Node* currentNode;

	float rotation = 0;
	float acceleration;
	float speed;
	//Saves information about this object
	void Save() {
		std::ofstream savefile;

		savefile.open("sav.bin", std::ios::out);
		if (savefile.good()) {
			savefile.write((char*)&this->localMatrix, sizeof(Matrix3));
			savefile.close();
		}
	}
	//Loads information about this object
	void Load() {
		std::ifstream loadfile;

		loadfile.open("sav.bin", std::ios::in);
		if (loadfile.good()) {
			loadfile.read((char*)&this->localMatrix, sizeof(Matrix3));
			loadfile.close();
		}
	}
	//Detaches object from parent
	void Unchild() {
		if (this->parent == nullptr) return;
		for (int i = 0; this->parent->child[i] == nullptr; i++) {
			if (this->parent->child[i] == this) {
				this->parent->child[i] = nullptr;
			}
		}
		this->localMatrix = parent->globalMatrix;
		this->parent = nullptr;
	}
	//Attaches object c as child
	void Child(GameObject* c) {
		c->parent = this;
		this->child.push_back(c);
	}

	//Draws the object to the renderer
	void draw(SDL_Renderer* renderer) {
		//NOTE: Consider Revising
		if (tex == nullptr) {			
			if(tex_path != nullptr)
				tex = loadTexture(tex_path, renderer);
		}

		localMatrix.vec[0][2] = position.x;
		localMatrix.vec[1][2] = position.y;

		localMatrix.setRotateZ(rotation);

		if (parent != nullptr) {
			//Does have parent
			globalMatrix = localMatrix * parent->globalMatrix;
			scale.x = scale.x + 1 * (parent->scale.x - scale.x);
			scale.y = scale.y + 1 * (parent->scale.y - scale.y);

			center.x = (int)(((this->scale.x * world.vec[0][0]) - (parent->scale.x * world.vec[0][0]) / 2) - (globalMatrix.vec[0][2] - parent->globalMatrix.vec[0][2])) / 2;
			center.y = (int)(((this->scale.y * world.vec[1][1]) - (parent->scale.x * world.vec[1][1]) / 2) - (globalMatrix.vec[1][2] - parent->globalMatrix.vec[1][2])) / 2;
		}
		else {
			//Doesn't have parent
			globalMatrix = localMatrix * world;

			center.x = (int)(this->scale.x * world.vec[0][0]) / 2;
			center.y = (int)(this->scale.y * world.vec[1][1]) / 2;
		} 

		SDL_Point drawPosition = { (int)globalMatrix.vec[0][2] - center.x, (int)globalMatrix.vec[1][2] - center.y };

		SDL_Rect rect = { (int)drawPosition.x,
						  (int)drawPosition.y,
						  scale.x * world.vec[0][0],
						  scale.y * world.vec[1][1] };
		aabb = rect;		


		SDL_RenderCopyEx(renderer, tex, NULL, &rect, -rotation * 57.2958f, &center, SDL_FLIP_NONE);
		//std::cout << rect.x << " (" << center.x << "), " << rect.y << " (" << center.y << ") "<< std::endl;
		//if (parent == nullptr) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
			//SDL_RenderDrawPoint(renderer, rect.x, rect.y);
		//}
	}
	//Returns true if object's rect is touching / inside of b
	bool intersecting(SDL_Rect b) {

		if (aabb.x + aabb.w > b.x && aabb.y + aabb.h > b.y && aabb.x < b.x + b.w && aabb.y < b.y + b.h) {
			return true;
		}

		return false;
	}
};


class Camera : GameObject {
private:
	Matrix3 local;
	Matrix3* global;

public:
	Vector3 postition;

	Camera() { this->global = &world; }
};