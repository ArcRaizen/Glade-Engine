#ifndef GLADE_PHYSIC_MATERIAL_H
#define GLADE_PHYSIC_MATERIAL_H
#include "System\Resource.h"
#include "GladeConfig.h"
#include "Math\MathMisc.h"
#include "Math\Precision.h"
#include "Utils\Assert.h"

namespace Glade {

class PhysicMaterial : public Resource<PhysicMaterial>
{
public:
	// Describes how 2 material's that are colliding determine that collisions overall
	// Coefficient of Restitution and Friction from both Materials
	// The Largest option between the 2 Materials has precedence (Max,Mult,Min,GAvg,Avg)
	enum class PhysicMaterialCombine { AVERAGE=0, GEOMETRIC_AVERAGE=1, PYTHAGOREAN=2, MINIMUM=3, MULTIPLY=4, MAXIMUM=5 };

	static SmartPointer<PhysicMaterial> CreateFromData(const std::string& name, bool saveSmart, PhysicMaterialCombine bCombine, PhysicMaterialCombine fCombine, gFloat b, gFloat sMu, gFloat dMu=-1.0f)
	{
		auto newMat = SmartPointer<PhysicMaterial>(new PhysicMaterial(name.length()>0? name : GenerateName("Unnamed PhysicMaterial %i"), bCombine, fCombine, b, sMu, dMu));
		if(newMat) 
		{
			if(saveSmart)
				RegisterSmartResource(newMat);
			else
				RegisterWeakResource(newMat);
		}
		return newMat;
	}

protected:
	PhysicMaterial(std::string& name, PhysicMaterialCombine bCombine, PhysicMaterialCombine fCombine, gFloat b, gFloat sMu, gFloat dMu=-1.0f) :
		Resource(name), bouncinessCombine(bCombine), frictionCombine(fCombine), bounciness(b), staticFriction(sMu), dynamicFriction(dMu<0.0f ? sMu : dMu)
	{
		AssertMsg(b <= gFloat(1.0f) && b >= gFloat(0.0f), "Illegal value for Bounciness of PhysicMaterial: must be in range [0, 1]");
		AssertMsg(sMu >= gFloat(0.0f), "Illegal value for Static Friction of PhysicMaterial: must not be negative");
		AssertMsg(dMu >= gFloat(0.0f), "Illegal value for Dynamic Friction of PhysicMaterial: must not be negative");
		AssertMsg(sMu >= dMu, "Illegal values for Friction of PhysicMaterials: Dynamic Friction mus not be larger than Static Friction");
	}

public:
	gFloat GetCombinedBounciness(const SmartPointer<PhysicMaterial> m)
	{
		switch(Max(bouncinessCombine, m->bouncinessCombine))
		{
			case PhysicMaterialCombine::AVERAGE:
				return ((bounciness + m->bounciness) * gFloat(0.5f));
			case PhysicMaterialCombine::GEOMETRIC_AVERAGE:
				return Sqrt(bounciness * m->bounciness);
			case PhysicMaterialCombine::PYTHAGOREAN:
				return PythagoreanSolve(bounciness, m->bounciness);
			case PhysicMaterialCombine::MINIMUM:
				return Min(bounciness, m->bounciness);
			case PhysicMaterialCombine::MULTIPLY:
				return bounciness * m->bounciness;
			case PhysicMaterialCombine::MAXIMUM:
				return Max(bounciness, m->bounciness);
		}
	}

	gFloat GetCombinedStaticFriction(const SmartPointer<PhysicMaterial> m)
	{
		switch(Max(frictionCombine, m->frictionCombine))
		{
			case PhysicMaterialCombine::AVERAGE:
				return ((staticFriction + m->staticFriction) * gFloat(0.5f));
			case PhysicMaterialCombine::GEOMETRIC_AVERAGE:
				return Sqrt(staticFriction * m->staticFriction);
			case PhysicMaterialCombine::PYTHAGOREAN:
				return PythagoreanSolve(staticFriction, m->staticFriction);
			case PhysicMaterialCombine::MINIMUM:
				return Min(staticFriction, m->staticFriction);
			case PhysicMaterialCombine::MULTIPLY:
				return staticFriction * m->staticFriction;
			case PhysicMaterialCombine::MAXIMUM:
				return Max(staticFriction, m->staticFriction);
		}
	}

	gFloat GetCombinedDynamicFriction(const SmartPointer<PhysicMaterial> m)
	{
		switch(Max(frictionCombine, m->frictionCombine))
		{
			case PhysicMaterialCombine::AVERAGE:
				return ((dynamicFriction + m->dynamicFriction) * gFloat(0.5f));
			case PhysicMaterialCombine::GEOMETRIC_AVERAGE:
				return Sqrt(dynamicFriction * m->dynamicFriction);
			case PhysicMaterialCombine::PYTHAGOREAN:
				return PythagoreanSolve(dynamicFriction, m->dynamicFriction);
			case PhysicMaterialCombine::MINIMUM:
				return Min(dynamicFriction, m->dynamicFriction);
			case PhysicMaterialCombine::MULTIPLY:
				return dynamicFriction * m->dynamicFriction;
			case PhysicMaterialCombine::MAXIMUM:
				return Max(dynamicFriction, m->dynamicFriction);
		}
	}

	template <typename T> friend class SmartPointer;
private:
	gFloat					bounciness;			// How bouncy is this material/collider? 0 = no bounce, 1 = perfectly elastic
	PhysicMaterialCombine	bouncinessCombine;	// How bounciness is combined

	gFloat					staticFriction;		// Friction coefficient when Obejcts are barely moving against each other
	gFloat					dynamicFriction;	// Friction coefficient when Objects are already moving
	PhysicMaterialCombine	frictionCombine;	// How friction is combined

//	static constexpr const char* autoNameFormat = "Unnamed PhysicMaterial %i";
};
}
#endif	// GLADE_PHYSIC_MATERIAL_H