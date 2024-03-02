#ifndef PHYSICS_GB2PS_ContactListener
#define PHYSICS_GB2PS_ContactListener

#include "box2d_area.h"
#include "box2d/b2_world_callbacks.h"

class Box2DCollisionObject;
class Box2DArea;
class b2Contact;

class GB2PS_ContactListener : public b2ContactListener
{
public:
	void BeginContact(b2Contact* contact);

	void EndContact(b2Contact* contact);

	bool ProcessContact(b2Contact* contact, bool enter);

	void AreaToBody(bool enter, Box2DArea* coA, Box2DCollisionObject* coB);
};

#endif