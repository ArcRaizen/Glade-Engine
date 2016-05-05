#pragma once
#ifndef GLADE_CONTACT_BATCH_H
#define GLADE_CONTACT_BATCH_H

#ifndef GLADE_CONTACT_H
#include "Contact.h"
#endif

#include <set>
#include <vector>
#include <queue>

namespace Glade { 

class ContactBatchNode
{
public:
	ContactBatchNode() : next(nullptr), previous(nullptr) { }
	ContactBatchNode(Contact c) : contact(c), next(nullptr), previous(nullptr) { }

	void CalculateInternals() { contact.CalculateInternals(); }

	Contact GetContact() { return contact; }
	ContactBatchNode* GetNext() { return next; }
	ContactBatchNode* GetPrevious() { return previous; }
	void SetNext(ContactBatchNode* n) { next = n; }
	void SetPrevious(ContactBatchNode* p) { previous = p; }

#ifdef SOLVE_PENETRATION_SIMULTANEOUS
	void AddLeft(ContactBatchNode* p) { left.push_back(p); }
	void AddRight(ContactBatchNode* p) { right.push_back(p); }
	std::vector<ContactBatchNode*> GetLeft() { return left; }
	std::vector<ContactBatchNode*> GetRight() { return right; }
	friend class ContactBatch;
#endif

	friend class ContactResolver;

private:
	Contact				contact;
	ContactBatchNode*	next;
	ContactBatchNode*	previous;
#ifdef SOLVE_PENETRATION_SIMULTANEOUS
	std::vector<ContactBatchNode*> left, right;
#endif
};

// Container for multiple Contacts that share RigidBodies
// These Contact's resolutions can/will affect the resolutions of the other Contacts in the batch
// So they are resolved together as a group
//
// If/When using Simultaneous Interpenetration Resolution, this will create a 'graph'/'tree' of the Contacts
// by choosing the "most major" Contact in the batch. All nodes in this graph contain lists of the other Contacts
// in the Batch that share a RigidBody with it so their resolution can propagate down the tree to affect the other
// Contacts so they all resolve simultaneously without interfering with each other
class ContactBatch
{
public:
	ContactBatch();
	~ContactBatch();

	// Iterate through all Contacts and call their 'CalculateInternals' function
	// If/When using Simultaneous Interpenetration Resolution, this generates the Graph of Contacts
	// with the root being the 'most major' Contact
	void CalculateInternals();

	// Add a new Contact to this Batch
	// If/When using Simultaneous Interpenetration Resolution, tests if new Contact should be the 'most major' Contact in the Batch
	void AddContact(Contact c);

	// Adds all Contacts in another Batch to this Batch
	// If/When using Simultaneous Interpenetration Resolution, chooses the new 'most major' Contact
	// from both Batches 'most major' Contact
	void MergeBatch(ContactBatch* batch);

	ContactBatchNode* GetHead();
	ContactBatchNode* GetTail();
#ifdef SOLVE_PENETRATION_SIMULTANEOUS
	ContactBatchNode* GetMajor();
#endif
	unsigned int GetNumContacts();
	unsigned int ContainsRigidBodies(unsigned int id1, unsigned int id2);
	friend class ContactResolver;

private:
	ContactBatchNode* head;
	ContactBatchNode* tail;
	unsigned int numContacts;
	std::set<unsigned int> ids;

#ifdef SOLVE_PENETRATION_SIMULTANEOUS
	std::set<ContactBatchNode*> nodes;
	ContactBatchNode* major;
#endif
};
}	// namespace Glade
#endif	// GLADE_CONTACT_BATCH_H
