#ifndef GLADE_CONFIG_H
#define GLADE_CONFIG_H

// Change between single/double precision floating point numbers
// Comment out following line to use double precision
//#define SINGLE_PRECISION

// Define mathematics to utilize a left-handed coordinate system
// Comment out/remove line to use right-handed coordinate system
#define LEFT_HANDED_COORDS

// Define Particle physics & integration to use Verlet integration
// Comment out/remove line to use Euler integration
#define VERLET

// Define which method of interpenetration resolution is used during Contact Resolution
// Iterative takes a specified number of iterations and solves the most-penetrative Contact
//		on each iteration until all Contacts are solved or it runs of out iterations
// Simultaneous chooses the "most major" Contact out of a related-batch of Contacts then
//		constructs a graph of Contacts in that batch based on which RigidBody they share.
//		The "most major" Contact is solved, then all further Contacts are solved assuming
//		that the previous Contacts solutions cannot be changed. 
// Comment out the following line to use Iterative. Leave following line to use Simultaneous
#define SOLVE_PENETRATION_SIMULTANEOUS
#ifndef SOLVE_PENETRATION_SIMULTANEOUS
#define SOLVE_PENETRATION_ITERATIVE
#endif

// Define whether Bounding Spheres or AABB are used for Frustum Culling tests for rendering
// Comment out the following line to use AABBs. Leave the following line to use Spheres
//#define FRUSTUM_CULLING_SPHERES
#ifndef FRUSTUM_CULLING_SPHERES
	#define FRUSTUM_CULLING_BOXES
	#define FRUSTUM_CULLING_GEOMETRIC

	// When testing against Boxes, we will initially test against the boxes that represent
	// each cell in the Spatial Hash used to organize the World.
	// Rigorous will do an additional test against each Object in a passing Spatial Hash cell.
	// Lenient will draw every Object in a passing Spatial Hash Cell without doing an extra test
	// Comment out the following line to use Lenient. Leave the following line to use Rigorous.
//	#define FRUSTUM_CULLING_RIGOROUS
	#ifndef FRUSTUM_CULLING_RIGOROUS
	#define FRUSTUM_CULLING_LENIENT
	#endif

#else
	// Extra option available is using FRUSTUM_CULLING_SPHERES
	// Define which method of Frustum Culling with Spheres will be used for rendering.
	// Radar calculates the relative position of vertices to the camera and checks if they are 
	//		inside of the view Frustum.
	// Geometric calculates the 6 planes that make up the view Frustum each frame and tests whether
	//		vertices are on the inside/outside of those planes.
	// Comment out the following line to use Geometric. Leave the following line to use Radar.
	#define FRUSTUM_CULLING_RADAR
	#ifndef FRUSTUM_CULLING_RADAR
	#define FRUSTUM_CULLING_GEOMETRIC
	#endif
#endif

// Define which sleep test to use for RigidBodies: Energy or SleepBox
// Energy tests a running weigted average (RWA) of the linear and angular energy of a RigidBody
//		If this energy falls below a threshold for a given amount of time, the RigidBody is put to sleep
// Box maintains and updates 3 separate AABBs representing the motion of 3 points on a RigidBody.
//		If all the AABB's are small enough below some threshold after a given amount of time, the RigidBody
//		is put to sleep. 
// Comment out the following line to use Energy. Leave following line to use Box
#define SLEEP_TEST_BOX
#ifndef SLEEP_TEST_BOX
#define SLEEP_TEST_ENERGY
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4800)
#pragma warning (disable: 4018)
#endif

#ifndef _DEBUG
#define COMPILE_WITHOUT_TRACE
#endif

#endif	// GLADE_CONFIG_H