#pragma once
#include "Core.h"

using namespace Glade;
class DrawableOctree : public Octree
{
public:
	DrawableOctree(Octree::OctreeType t, bool resize, Vector c, gFloat w, gFloat h, gFloat d, int maxE, int minE, gFloat loose=1);
	~DrawableOctree();

	void Update(bool saveBoxes);
	void Draw();

	std::map<Vector, Vector> GetBoxes() { return boxes; }

private: 
	std::map<Vector, Vector> boxes;
};

