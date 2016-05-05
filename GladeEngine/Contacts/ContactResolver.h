#pragma once
#ifndef GLADE_CONTACT_RESOLVER_H
#define GLADE_CONTACT_RESOLVER_H

#ifndef GLADE_CONTACT_BATCH_H
#include "ContactBatch.h"
#endif
#include <map>
#include <queue>

namespace Glade {
class ContactResolver
{
public:
	ContactResolver(unsigned int iter, gFloat impEp=0.01, gFloat penEp=0.01);
	ContactResolver(unsigned int iIter, unsigned int pIter, gFloat impEp=0.01, gFloat penEp=0.01);

	void SetIterations(unsigned int iter);
	void SetIterations(unsigned int iIter, unsigned int pIter);
	void SetEpsilon(gFloat impEp, gFloat penEp);

	void ResolveContacts(ContactBatch* contactBatch);

protected:
	void ResolveImpulse(ContactBatchNode* contactBatch, unsigned int numContacts);
#ifdef SOLVE_PENETRATION_SIMULTANEOUS
	//void ResolveInterpenetration2(ContactBatchNode* contactBatch, unsigned int numContacts);
	void ResolveInterpenetration3(ContactBatch* contactBatch, unsigned int numContacts);
	void ResolveInterpenetration4(ContactBatch* contactBatch, unsigned int numContacts);
#else
	void ResolveInterpenetration(ContactBatchNode* contactBatch, unsigned int numContacts);
#endif

public:
	bool IsValid()
	{
		return impulseIterations > 0 && penetrationIterations > 0 &&
				impulseEpsilon > 0 && penetrationEpsilon > 0;
	}

	unsigned int impulseIterationsUsed;
	unsigned int penetrationIterationsUsed;

protected:
	unsigned int impulseIterations;
	unsigned int penetrationIterations;

	gFloat	impulseEpsilon;
	gFloat	penetrationEpsilon;

private:
	bool	validSettings;
};
}	// namespace
#endif	// GLADE_CONTACT_RESOLVER_H
