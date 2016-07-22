#pragma once
#include <Vector3.h>
#include <vector>

namespace AI {

	class Node {
	private:

	public:
		float x, y;
		operator Vector3() {
			return Vector3(x, y, 1);
		}
		float gCost = INFINITY;
		float fCost = INFINITY;

		std::vector<Node> neighbours;

		Node() {};
		Node(Vector3 position) {
			this->x = position.x;
			this->y = position.y;
		}
		~Node() { 
			//if(this != nullptr)
				//delete this; 
		}
	};

	
	class Path {

	};

	bool processNode(Node* node, Node* goal) {
		if (node->x == goal->x && node->y == goal->y) {
			return true;
		}
	}

	Node* getNeighbour(float unit) {
		Node* n = nullptr;
		return n;
	}
	Node* findNodeFromPosition(std::vector<Node*> nodes, Vector3 pos) {
		Node* n = nullptr;
		for (int i = 0; i < nodes.size(); i++) {
			if (nodes[i]->x == pos.x && nodes[i]->y == pos.y) {
				n = nodes[i];
			}
		}
		return n;
	}
}