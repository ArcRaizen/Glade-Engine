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