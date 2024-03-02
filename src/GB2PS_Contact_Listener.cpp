#include "GB2PS_Contact_Listener.h"
#include "box2d/b2_contact.h"
#include "box2d_collision_object.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/callable.hpp>

void GB2PS_ContactListener::BeginContact(b2Contact *contact) {
	ProcessContact(contact, true);
}

void GB2PS_ContactListener::EndContact(b2Contact *contact) {
	ProcessContact(contact, false);
}

bool GB2PS_ContactListener::ProcessContact(b2Contact *contact, bool enter) {
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
  
	//make sure only one of the fixtures was a sensor
	bool sensorA = fixtureA->IsSensor();
	bool sensorB = fixtureB->IsSensor();

	if( ! (sensorA ^ sensorB) )
		return false;
  
	if ( sensorA ) { //fixtureB must be an enemy aircraft
		AreaToBody(enter,reinterpret_cast<Box2DArea*>( fixtureA->GetBody()->GetUserData().pointer ), reinterpret_cast<Box2DCollisionObject*>( fixtureB->GetBody()->GetUserData().pointer ));
	}
	else { //fixtureA must be an enemy aircraft
		AreaToBody(enter,reinterpret_cast<Box2DArea*>( fixtureB->GetBody()->GetUserData().pointer ), reinterpret_cast<Box2DCollisionObject*>( fixtureA->GetBody()->GetUserData().pointer ));
	}
	return true;
}

void GB2PS_ContactListener::AreaToBody(bool enter, Box2DArea *coA, Box2DCollisionObject *coB) {
	if(!coA || !coB)
		return;
	//UtilityFunctions::print("Area to body collidion: ",coA->get_self(), " - ", coB->get_self());
	//(int p_status, const RID &p_area, ObjectID p_instance, int p_area_shape, int p_self_shape)
	//Variant ret = coA->ares_body_callback.call<int,RID,ObjectID,int,int>((int)enter,coA->get_self(),coB->get_instance_id(),0,0);
	//if(coA->ares_body_callback)
	{
		//UtilityFunctions::print("Area to body collision. Callable: ",*coA->ares_body_callback);
		Variant ret = coA->ares_body_callback.callv(Array::make((int)(!enter),coA->get_self(),coB->get_instance_id(),0,0));
		//UtilityFunctions::print("Area to body collision. ret var: ",ret);
	}

}
