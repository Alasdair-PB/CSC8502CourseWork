#pragma once
#include "../nclgl/SceneNode.h"
#include <vector>

class Pathing 
{
	public:

		Pathing() {

		}

		~Pathing(void) {
			sceneNode.clear();
		}
	protected:
		vector<SceneNode*> sceneNode;



};