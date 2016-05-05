#include "ContactBatch.h"

using namespace Glade;

ContactBatch::ContactBatch() : numContacts(0) 
{ }

ContactBatch::~ContactBatch() 
{
	ContactBatchNode* temp = head, *temp2 = head->GetNext();
	for(unsigned int i = 0; i < numContacts; ++i)
	{
		delete temp;
		temp = temp2;
		temp2 = temp2->GetNext();
	}
}

void ContactBatch::CalculateInternals()
{
	ContactBatchNode* temp = head;
	do
	{
		temp->CalculateInternals();
#ifdef SOLVE_PENETRATION_SIMULTANEOUS
		nodes.insert(temp);
#endif
		temp = temp->GetNext();
	} while (temp != head);

#ifdef SOLVE_PENETRATION_SIMULTANEOUS
	std::queue<ContactBatchNode*> queue;
	auto nodes2 = nodes;
	nodes2.erase(major);
	ContactBatchNode* current;
	queue.push(major);
	while(queue.size() > 0)
	{
		current = queue.front();
		queue.pop();

		for(auto iter = nodes2.begin(); iter != nodes2.end();)
		{
			if(current->contact.b1 == (*iter)->contact.b1 || current->contact.b1 == (*iter)->contact.b2)
			{
				if(current->contact.b1 == (*iter)->contact.b2)
					(*iter)->contact.ReverseContact();
				current->AddLeft(*iter);
				queue.push(*iter);
				nodes2.erase(iter++);
				if(nodes2.size() == 0)
					return;
			}
			else if(current->contact.b2 == (*iter)->contact.b1 || current->contact.b2 == (*iter)->contact.b2)
			{
				if(current->contact.b2 == (*iter)->contact.b1)
					(*iter)->contact.ReverseContact();
				current->AddRight(*iter);
				queue.push(*iter);
				nodes2.erase(iter++);
				if(nodes2.size() == 0)
					return;
			}
			else
				++iter;
		}
	}
#endif
}

// Add a new Contact to this Batch
void ContactBatch::AddContact(Contact c)
{
	ContactBatchNode* node = new ContactBatchNode(c);
#ifdef SOLVE_PENETRATION_SIMULTANEOUS
	if(major == nullptr || c.HasInfiniteMass() ||		// Any Contact with an infinitely massed body is "most major"
		(!major->contact.HasInfiniteMass() && c.relativeVelocity > major->contact.relativeVelocity) || // If no infinitely massed body, then the Contact with the highest relative velocity
		(!major->contact.HasInfiniteMass() && c.relativeVelocity == major->contact.relativeVelocity && c.point.y > major->contact.point.y))	// Otherwise, the highest Contact position
	{
		major = node;
	}
#endif
	if(head == nullptr)
	{
		head = node;
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

void ContactBatch::MergeBatch(ContactBatch* batch)
{
#ifdef SOLVE_PENETRATION_SIMULTANEOUS
	if(batch->major->contact.HasInfiniteMass() ||		// Any Contact with an infinitely massed body is "most major"
		(!batch->major->contact.HasInfiniteMass() && major->contact.relativeVelocity > batch->major->contact.relativeVelocity) || // If no infinitely massed body, then the Contact with the highest relative velocity
		(!batch->major->contact.HasInfiniteMass() && major->contact.relativeVelocity == batch->major->contact.relativeVelocity && major->contact.point.y > batch->major->contact.point.y))	// Otherwise, the highest Contact position
	{
		major = batch->major;
	}
#endif

	ContactBatchNode* head = batch->GetHead();
	ContactBatchNode* temp = head;
	do
	{
		AddContact(temp->GetContact());
		temp = temp->GetNext();
	}while(temp != head);
}

ContactBatchNode* ContactBatch::GetHead() { return head; }
ContactBatchNode* ContactBatch::GetTail() { return tail; }
#ifdef SOLVE_PENETRATION_SIMULTANEOUS
ContactBatchNode* ContactBatch::GetMajor() { return major; }
#endif
unsigned int ContactBatch::GetNumContacts() { return numContacts; }
unsigned int ContactBatch::ContainsRigidBodies(unsigned int id1, unsigned int id2) { return ids.count(id1) + ids.count(id2); }