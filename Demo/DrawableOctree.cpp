#include "DrawableOctree.h"


DrawableOctree::DrawableOctree(Octree::OctreeType t, bool resize, Vector c, gFloat w, gFloat h, gFloat d, int maxE, int minE, gFloat loose) : 
				Octree(t, resize, c, w, h, d, maxE, minE, loose)
{
}


DrawableOctree::~DrawableOctree()
{
}

void DrawableOctree::Update(bool saveBoxes)
{
	Octree::Update();
	if(!saveBoxes) return;

	boxes.clear(); 
	for(auto iter = nodes.begin(); iter != nodes.end(); ++iter)
	{
		//if(iter->second->GetNumElements() > 0)
			boxes[iter->second->GetBox().center] = iter->second->GetBox().halfWidths;
	}
}

void DrawableOctree::Draw()
{
	Vector c, halfWidths;
	for(auto iter = boxes.begin(); iter != boxes.end(); iter++)
	{
		c = iter->first;
		halfWidths = iter->second;

		glColor3f(0,0,0);
		glBegin(GL_LINE_STRIP);		// Left Side
		glVertex3f(c.x - halfWidths.x, c.y + halfWidths.y, c.z + halfWidths.z);
		glVertex3f(c.x - halfWidths.x, c.y - halfWidths.y, c.z + halfWidths.z);
		glVertex3f(c.x - halfWidths.x, c.y - halfWidths.y, c.z - halfWidths.z);
		glVertex3f(c.x - halfWidths.x, c.y + halfWidths.y, c.z - halfWidths.z);
		glVertex3f(c.x - halfWidths.x, c.y + halfWidths.y, c.z + halfWidths.z);
		glEnd();
		glBegin(GL_LINE_STRIP);		// Right Side
		glVertex3f(c.x + halfWidths.x, c.y + halfWidths.y, c.z + halfWidths.z);
		glVertex3f(c.x + halfWidths.x, c.y - halfWidths.y, c.z + halfWidths.z);
		glVertex3f(c.x + halfWidths.x, c.y - halfWidths.y, c.z - halfWidths.z);
		glVertex3f(c.x + halfWidths.x, c.y + halfWidths.y, c.z - halfWidths.z);
		glVertex3f(c.x + halfWidths.x, c.y + halfWidths.y, c.z + halfWidths.z);
		glEnd();
		glBegin(GL_LINE_STRIP);		// Top Front Edge
		glVertex3f(c.x - halfWidths.x, c.y + halfWidths.y, c.z + halfWidths.z);
		glVertex3f(c.x + halfWidths.x, c.y + halfWidths.y, c.z + halfWidths.z);
		glEnd();
		glBegin(GL_LINE_STRIP);		// Top Back Edge
		glVertex3f(c.x - halfWidths.x, c.y + halfWidths.y, c.z - halfWidths.z);
		glVertex3f(c.x + halfWidths.x, c.y + halfWidths.y, c.z - halfWidths.z);
		glEnd();
		glBegin(GL_LINE_STRIP);		// Bottom Front Edge
		glVertex3f(c.x - halfWidths.x, c.y - halfWidths.y, c.z + halfWidths.z);
		glVertex3f(c.x + halfWidths.x, c.y - halfWidths.y, c.z + halfWidths.z);
		glEnd();
		glBegin(GL_LINE_STRIP);		// Bottom Back Edge
		glVertex3f(c.x - halfWidths.x, c.y - halfWidths.y, c.z - halfWidths.z);
		glVertex3f(c.x + halfWidths.x, c.y - halfWidths.y, c.z - halfWidths.z);
		glEnd();
	}
}