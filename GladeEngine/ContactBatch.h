#pragma once
#ifndef GLADE_CONTACT_BATCH_H
#define GLADE_CONTACT_BATCH_H

#ifndef GLADE_CONTACT_H
#include "Contact.h"
#endif

#include <set>

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

	friend class ContactResolver;

private:
	Contact				contact;
	ContactBatchNode*	next;
	ContactBatchNode*	previous;
};

// Linked-List of all related Contacts 
// All Contacts sequentially connected to each other through shared RigidBodies refered to as a 'Batch'
class ContactBatch
{
public:
	ContactBatch() : numContacts(0) { }
	~ContactBatch() 
	{
		ContactBatchNode* temp = head, *temp2 = head->GetNext();
		do
		{
			delete temp;
			temp = temp2;
			temp2 = temp2->GetNext();
		} while (temp != nullptr);
	}

	void CalculateInternals()
	{
		ContactBatchNode* temp = head;
		do
		{
			temp->CalculateInternals();
			temp = temp->GetNext();
		} while (temp != head);
	}

	// Add a new Contact to this Batch
	void AddContact(Contact c)
	{
		ContactBatchNode* node = new ContactBatchNode(c);
		if(head == nullptr)
		{
			head = new ContactBatchNode(c);
			head->SetNext(head);
			head->SetPrevious(head);
		}
		else if(tail == nullptr)
		{
			tail = node;
			head->SetNext(tail);
			head->SetPrevious(tail);
			tail->SetNext(head);
			tail->SetPrevious(head);
		}
		else
		{
			tail->SetNext(node);
			head->SetPrevious(node);
			node->SetPrevious(tail);
			node->SetNext(head);
			tail = node;
		}

		++numContacts;
		ids.insert(c.b1->GetID());
		ids.insert(c.b2->GetID());
	}

	void MergeBatch(ContactBatch* batch)
	{
		ContactBatchNode* head = batch->GetHead();
		ContactBatchNode* temp = head;

		do
		{
			AddContact(temp->GetContact());
			temp = temp->GetNext();
		}while(temp != head);
	}

	ContactBatchNode* GetHead() { return head; }
	ContactBatchNode* GetTail() { return tail; }
	unsigned int GetNumContacts() { return numContacts; }
	unsigned int ContainsRigidBodies(unsigned int id1, unsigned int id2) { return ids.count(id1) + ids.count(id2); }

private:
	ContactBatchNode* head;
	ContactBatchNode* tail;
	unsigned int numContacts;
	std::set<unsigned int> ids;
};
}	// namespace Glade
#endif	// GLADE_CONTACT_BATCH_H
