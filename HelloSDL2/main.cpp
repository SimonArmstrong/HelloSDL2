#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include "window.h"
#include <Vector2.h>
#include <Vector3.h>
#include "color.h"
#include <SDL_timer.h>
#include "objTest.h"
#include "AI.h"
#include <time.h>
#include "_time.h"

#include <vector>

const int SCREEN_SCALE = 100;
const int SCREEN_WIDTH = 16 * SCREEN_SCALE;
const int SCREEN_HEIGHT = 9 * SCREEN_SCALE;

int UNIT = 32;

SDL_Renderer* r = nullptr;

Color c = { 255, 255, 255, 255 };

SDL_Renderer* renderRef;

enum {
	KEYBOARD
};

Vector3 InputAxis(SDL_Event* e) {
	Vector3 axis;
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_UP]) axis.y = -1;
	if (keys[SDL_SCANCODE_DOWN]) axis.y = 1;
	if (keys[SDL_SCANCODE_LEFT]) axis.x = -1;
	if (keys[SDL_SCANCODE_RIGHT]) axis.x = 1;

	//axis.normalise()
	return axis;
}

GameObject skull("sprites/skull.png");
GameObject bone("sprites/bone.png");
GameObject spook("sprites/spooky skull.png");

Vector3 Camera;

GameObject tile_a("sprites/grass tile.png");
GameObject tile_b("sprites/grass_tile_0.png");
GameObject tile_c("sprites/grass_tile_1.png");

std::vector<GameObject> tile_types = { tile_a, tile_b, tile_c };

std::vector<std::string> grass_tile_paths;

std::vector<GameObject*> gobjs;

float Axis(SDL_Scancode positive, SDL_Scancode negative) {
	float axis = 0.f;
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[positive]) axis =  1;
	if (keys[negative]) axis = -1;

	return axis;
}

bool KeyDown(SDL_Scancode key) {
	bool axis = 0;
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[key]) axis = 1;

	return axis;
}

bool KeyUp(SDL_Scancode key) {
	bool axis = 0;
	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (!keys[key]) axis = 1;

	return axis;
}

std::vector<GameObject> tiles;
std::vector<std::vector<AI::Node>> node_pos;
std::vector<AI::Node> tile_nodes;

 //AI::Node grassNode;
//AI::Node wall;

bool mapBuilt = false;
void buildMap() {
	tiles.clear();
	int r_spr = rand() % 2 + 1;
	for (int x = 0; x < 17; x++) {
		for (int y = 0; y < 10; y++) {
			r_spr = rand() % 3;
			if (x != 5 && y != 1) {
				tiles.push_back(tile_types[r_spr]);	
				//tile_nodes.push_back(tiles[x + y].currentNode);
			}
			if (y == 2 && x == 2) {
				tiles.push_back(tile_types[r_spr]);
				//tile_nodes.push_back(tiles[x + y].currentNode);
			}
			if (y == 1 && x == 5) {
				tiles.push_back(tile_types[r_spr]);
				//tile_nodes.push_back(tiles[x + y].currentNode);
			}
			if (y == 3 && x == 5) {
				tiles.push_back(tile_types[r_spr]);
				//tile_nodes.push_back(tiles[x + y].currentNode);
			}
			//tiles[x + y].tex = loadTexture(grass_tile_paths[0].c_str(), window.screen->renderer);
			//tiles[x + y].position = Vector3(x * tile.scale.x, y * tile.scale.y, 0);
		}
		r_spr = rand() % 2 + 1;
	}
	mapBuilt = true;
}


bool moved = true;

AI::Node* getNodeAt(Vector3 pos) {
	for (int i = 0; i < tiles.size(); i++) {
		if (tiles[i].currentNode.x == pos.x && tiles[i].currentNode.y == pos.y) {
			return &tiles[i].currentNode;
		}
	}
	return nullptr;
}

std::vector<AI::Node*> generatePath(AI::Node* From, AI::Node* To) {
	//find cheapest path from From to To
	std::vector<AI::Node*> closedNodes;
	std::vector<AI::Node*> openedNodes;
	std::vector<AI::Node*> unvisitedNodes;
	std::vector<AI::Path> paths;
	std::vector<AI::Node*> record;
	openedNodes.push_back(From);
	openedNodes[0]->gCost = 0;
	openedNodes[0]->fCost = 0;	// 0 == no heuristic

	int nodeToCheck = 0;	// lowest fScore value of openedNodes

	for (int i = 0; i < tiles.size(); i++) {
		unvisitedNodes.push_back(&tiles[i].currentNode);
	}


	//while we still have nodes to check
	while (openedNodes.size() > 0) {
		AI::Node* currentNode = openedNodes[nodeToCheck];
		//Check if there is a wall node
		//process the node, add it to closedNodes and remove it from openedNodes
		//Stop and create the path if the current node we are checking is our goal node
		
		SDL_SetRenderDrawColor(renderRef, 255, 0, 0, 255);
		SDL_RenderDrawPoint(renderRef, currentNode->x, currentNode->y);

		currentNode->neighbours.push_back(getNodeAt(Vector3(currentNode->x + UNIT, currentNode->y, 0))); // left
		currentNode->neighbours.push_back(getNodeAt(Vector3(currentNode->x - UNIT, currentNode->y, 0))); // right
		currentNode->neighbours.push_back(getNodeAt(Vector3(currentNode->x, currentNode->y - UNIT, 0))); // up
		currentNode->neighbours.push_back(getNodeAt(Vector3(currentNode->x, currentNode->y + UNIT, 0))); // down

		if (AI::processNode(currentNode, To)) { //if we find the end of the path
			//check the score of the path, save the path
			break;
		}

		//check neighbors, if wall, don't add to record
		//else
		//add closest-to-To neighbor to record

		closedNodes.push_back(openedNodes[nodeToCheck]);
		openedNodes.erase(openedNodes.begin() + nodeToCheck);

		nodeToCheck++;
	}

	//make array of paths
	//path cost = sum of path nodes costs
	//return path with shortest cost

	return record;
}

void enemyMove() {
	//Calculate shortest path
	std::vector<AI::Node*> path = generatePath(&spook.currentNode, &skull.currentNode);
	
	//Move along path
	for (static int i = 0; i < path.size(); i++) {
		spook.position = Vector3(path[i]->x, path[i]->y, 1);
	}
}

void Input(SDL_Event *e) {
	float deltaTime = 0.001f;
	static float step = 0.f;
	Vector3 Movement = InputAxis(e);
	Movement.normalise();

	static float rot = 0;
	rot += 0.0005f;
	
	if (Axis(SDL_SCANCODE_C, SDL_SCANCODE_X) < 0) {
		bone.Unchild();
	}

	skull.scale.x += Axis(SDL_SCANCODE_E, SDL_SCANCODE_Q) / skull.aabb.h * skull.speed * 5 * deltaTime * skull.aabb.h;
	skull.scale.y += Axis(SDL_SCANCODE_E, SDL_SCANCODE_Q) / skull.aabb.w * skull.speed * 5 * deltaTime * skull.aabb.w;

	skull.rotation += Axis(SDL_SCANCODE_D, SDL_SCANCODE_A) * 0.0004f;

	world.m11 += Axis(SDL_SCANCODE_PAGEUP, SDL_SCANCODE_PAGEDOWN) * 0.00004f;
	world.m22 += Axis(SDL_SCANCODE_PAGEUP, SDL_SCANCODE_PAGEDOWN) * 0.00004f;

	Camera.x += Axis(SDL_SCANCODE_J, SDL_SCANCODE_L) * skull.speed * deltaTime * 2;
	Camera.y += Axis(SDL_SCANCODE_I, SDL_SCANCODE_K) * skull.speed * deltaTime * 2;

	//skull.localMatrix.setRotateZ(rot);

	//skull.position = skull.position + Movement * skull.speed * deltaTime * 2;
	if (KeyDown(SDL_SCANCODE_LEFT) && moved) {
		// Player
		if (moved) {
			step += 0.00001;
			skull.position.x -= UNIT * world.m11; // * deltaTime;
		}
		enemyMove();
		moved = false;
	}
	if (KeyDown(SDL_SCANCODE_RIGHT) && moved) {
		// Player
		if (moved) {
			step += 0.00001;
			skull.position.x += UNIT * world.m11; // * deltaTime;
		}
		enemyMove();
		moved = false;
	}
	if (KeyDown(SDL_SCANCODE_UP) && moved) {
		// Player
		if (moved) {
			step += 0.00001;
			skull.position.y -= UNIT * world.m11; // * deltaTime;
		}
		enemyMove();
		moved = false;
	}
	if (KeyDown(SDL_SCANCODE_DOWN) && moved) {
		// Playre
		if (moved) {
			step += 0.00001;
			skull.position.y += UNIT * world.m11; // * deltaTime;
		}
		enemyMove();
		moved = false;
	}

	if (!moved && (KeyUp(SDL_SCANCODE_UP) && KeyUp(SDL_SCANCODE_DOWN) && KeyUp(SDL_SCANCODE_LEFT) && KeyUp(SDL_SCANCODE_RIGHT))) {
		step = 0;
		moved = true;
	}

	if (KeyDown(SDL_SCANCODE_R)) { 

		buildMap();
	}
}

//Do all the drawing here
void Window::Draw(SDL_Renderer* renderer, SDL_Event *e) {
	
	Time::last = Time::now;
	Time::now = SDL_GetTicks();
		
	Time::deltaTime = (float)((Time::now - Time::last) * 1000 / SDL_GetTicks());

	UNIT = 32 * scaleFactor;
	Input(e);

	if (mapBuilt) {
		for (int x = 0; x < 17; x++) {
			for (int y = 0; y < 10; y++) {
				if (x != 5 && y != 2) {
					tiles[x + y].position = Vector3(x * tile_a.scale.x, y * tile_a.scale.y, 0);
					tiles[x + y].currentNode = AI::Node(tiles[x + y].position);
					tiles[x + y].draw(renderer);
				}
				if (y == 2 && x == 2) {
					tiles[x + y].position = Vector3(x * tile_a.scale.x, y * tile_a.scale.y, 0);
					tiles[x + y].currentNode = AI::Node(tiles[x + y].position);
					tiles[x + y].draw(renderer);
				}
				if (y == 1 && x == 5) {
					tiles[x + y].position = Vector3(x * tile_a.scale.x, y * tile_a.scale.y, 0);
					tiles[x + y].currentNode = AI::Node(tiles[x + y].position);
					tiles[x + y].draw(renderer);
				}
				if (y == 3 && x == 5) {
					tiles[x + y].position = Vector3(x * tile_a.scale.x, y * tile_a.scale.y, 0);
					tiles[x + y].currentNode = AI::Node(tiles[x + y].position);
					tiles[x + y].draw(renderer);
				}
			}
		}			
	}


	world.vec[0][2] = Camera.x + (SCREEN_WIDTH / 2);
	world.vec[1][2] = Camera.y + (SCREEN_HEIGHT / 2);

	if (mapBuilt) {
		skull.currentNode = AI::Node(skull.position);
		spook.currentNode = AI::Node(spook.position);
	}

	skull.draw(renderer);
	spook.draw(renderer);
}

int main(int argc, char* args[]) {	
	Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "Window");

	renderRef = window.screen->renderer;

	tile_a.scale = tile_a.scale * scaleFactor;

	buildMap();

	srand(time(NULL));

	skull.Load();

	Camera = Vector3(-428, -428, 1);

	skull.speed = 15;
	skull.position = Vector3(32 * scaleFactor + 96, 32 * scaleFactor, 0);

	spook.position = Vector3((32 * scaleFactor * 15), (32 * scaleFactor * 8), 0);
	
	//skull.Child(&spook);
	window.create(SDL_WINDOW_RESIZABLE);
	window.close();

	skull.Save();

	return 0;
}