#pragma once
#include <Vector3.h>
#include <vector>

namespace AI {

	class Node {
	private:
		Node* getNeighbour();
	public:
		float x, y;
		operator Vector3() {
			return Vector3(x, y, 1);
		}
		float gCost = INFINITY;
		float fCost = INFINITY;

		Node() {};
		~Node() { 
			if(this != nullptr)
				delete this; 
		}
	};

	
	class Path {

	};

	bool processNode(Node* node, Node* goal) {
		if (node == goal) {
			return true;
		}
	}

	Node* Node::getNeighbour() {
		Node* n = nullptr;
		return n;
	}
}