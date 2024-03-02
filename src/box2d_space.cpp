#include "box2d_space.h"

#include <godot_cpp/core/memory.hpp>

#include <box2d/b2_body.h>

#include "box2d_body.h"
#include "box2d_collision_object.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters2d.hpp>
#include "GB2PS_Contact_Listener.h"

const SelfList<Box2DBody>::List &Box2DSpace::get_active_body_list() const {
	return active_list;
}

void Box2DSpace::body_add_to_active_list(SelfList<Box2DBody> *p_body) {
	active_list.add(p_body);
}

void Box2DSpace::body_remove_from_active_list(SelfList<Box2DBody> *p_body) {
	active_list.remove(p_body);
}

void Box2DSpace::add_object(Box2DCollisionObject *p_object) {
	ERR_FAIL_COND(!p_object);
	if(world)
	p_object->set_b2Body(world->CreateBody(p_object->get_b2BodyDef()));
}

void Box2DSpace::remove_object(Box2DCollisionObject *p_object) {
	ERR_FAIL_COND(!p_object);
	if(world)
	world->DestroyBody(p_object->get_b2Body());
	p_object->set_b2Body(0);
}

void Box2DSpace::body_add_to_state_query_list(SelfList<Box2DBody> *p_body) {
	state_query_list.add(p_body);
}

void Box2DSpace::body_remove_from_state_query_list(SelfList<Box2DBody> *p_body) {
	state_query_list.remove(p_body);
}

void Box2DSpace::call_queries() {
	while (state_query_list.first()) {
		Box2DBody *b = state_query_list.first()->self();
		state_query_list.remove(state_query_list.first());
		b->call_queries();
	}
	// TODO: areas
}

void Box2DSpace::step(float p_step) const {
	const int32 velocityIterations = 5;
	const int32 positionIterations = 3;

	world->Step(p_step, velocityIterations, positionIterations);

	const SelfList<Box2DBody>::List *body_list = &get_active_body_list();
	const SelfList<Box2DBody> *b = body_list->first();
	while (b) {
		b->self()->after_step();
		b = b->next();
	}
}

Box2DSpace::Box2DSpace() {
	b2Vec2 gravity(0.0f, 0.0f);
	if(!world)
		world = memnew(b2World(gravity));
	if(!m_contactTilstener)
		m_contactTilstener =  memnew(GB2PS_ContactListener());
	world->SetContactListener(m_contactTilstener);
}

Box2DSpace::~Box2DSpace() {
	if(world)
		memdelete(world);
	world=0;
	if(m_contactTilstener)
		memdelete(m_contactTilstener);
	m_contactTilstener=0;
}

Dictionary Box2DPhysicsDirectSpaceState2D::intersect_ray(const Ref<PhysicsRayQueryParameters2D> &parameters) {
	UtilityFunctions::print(this, " intersect_ray ");

	Dictionary result;
	
	b2Vec2 point1;
	godot_to_box2d(parameters->get_from(), point1);
	b2Vec2 point2;
	godot_to_box2d(parameters->get_to(), point2);

	RayCastClosestCallback callback;
	space->get_b2World()->RayCast(&callback, point1, point2);

	if (callback.m_hit)
	{
		//callback.m_point + 0.5f * callback.m_normal;
		Box2DCollisionObject* ud = reinterpret_cast<Box2DCollisionObject*>(callback.m_hit_fixture->GetBody()->GetUserData().pointer);

		//collider_id: The colliding object's ID.
		//collider_id = res_obj->get_instance_id();
		result["collider_id"]=ud->get_instance_id();
		
		//collider: The colliding object.
		/*if (r_result.collider_id.is_valid()) {
			r_result.collider = ObjectDB::get_instance(r_result.collider_id);
		}*/
		result["collider"] = ObjectDB::get_instance(ud->get_instance_id());
		
		//normal: The object's surface normal at the intersection point, or Vector2(0, 0) if the ray starts inside the shape and PhysicsRayQueryParameters2D.hit_from_inside is true.
		Vector2 n;
		box2d_to_godot(callback.m_normal, n);
		result["normal"]=n;
		//position: The intersection point.
		Vector2 p;
		box2d_to_godot(callback.m_point, p);
		result["position"]=p;
		//rid: The intersecting object's RID.
		result["rid"]=ud->get_self();

		//shape: The shape index of the colliding shape.
	}
	return result;

}

 