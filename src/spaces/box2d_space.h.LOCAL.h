#ifndef BOX2D_SPACE_H
#define BOX2D_SPACE_H


#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/templates/self_list.hpp>
#include <godot_cpp/variant/rid.hpp>

#include <box2d/b2_world.h>

#include <godot_cpp/classes/physics_direct_space_state2d.hpp>

using namespace godot;

class Box2DCollisionObject;
class Box2DBody;
class GB2PS_ContactListener;

class Box2DSpace {
private:
	RID self;

	b2World *world = nullptr;

	SelfList<Box2DBody>::List active_list;
	SelfList<Box2DBody>::List state_query_list;

	bool locked = false;
	GB2PS_ContactListener* m_contactTilstener=0;
public:
	_FORCE_INLINE_ void set_self(const RID &p_self) { self = p_self; }
	_FORCE_INLINE_ RID get_self() const { return self; }

	b2World *get_b2World() const { return world; }

	const SelfList<Box2DBody>::List &get_active_body_list() const;
	void body_add_to_active_list(SelfList<Box2DBody> *p_body);
	void body_remove_from_active_list(SelfList<Box2DBody> *p_body);

	void body_add_to_state_query_list(SelfList<Box2DBody> *p_body);
	void body_remove_from_state_query_list(SelfList<Box2DBody> *p_body);

	void add_object(Box2DCollisionObject *p_object);
	void remove_object(Box2DCollisionObject *p_object);

	void step(float p_step) const;
	void call_queries();

	bool is_locked() const { return locked; }
	void lock() { locked = true; }
	void unlock() { locked = false; }

	Box2DSpace();
	~Box2DSpace();
};


// This callback finds the closest hit. Polygon 0 is filtered.
class RayCastClosestCallback : public b2RayCastCallback
{
public:
	RayCastClosestCallback()
	{
		m_hit = false;
	}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override
	{
		m_hit = true;
		m_point = point;
		m_normal = normal;
		m_hit_fixture = fixture;
		return fraction;
	}
	
	bool m_hit;
	b2Vec2 m_point;
	b2Vec2 m_normal;
	b2Fixture* m_hit_fixture;
};

// This callback finds any hit. Polygon 0 is filtered. For this type of query we are usually
// just checking for obstruction, so the actual fixture and hit point are irrelevant. 
class RayCastAnyCallback : public b2RayCastCallback
{
public:
	RayCastAnyCallback()
	{
		m_hit = false;
	}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float) override
	{

		m_hit = true;
		m_point = point;
		m_normal = normal;

		// At this point we have a hit, so we know the ray is obstructed.
		// By returning 0, we instruct the calling code to terminate the ray-cast.
		return 0.0f;
	}

	bool m_hit;
	b2Vec2 m_point;
	b2Vec2 m_normal;
};


// This ray cast collects multiple hits along the ray. Polygon 0 is filtered.
// The fixtures are not necessary reported in order, so we might not capture
// the closest fixture.
class RayCastMultipleCallback : public b2RayCastCallback
{
public:
	enum
	{
		e_maxCount = 3
	};

	RayCastMultipleCallback()
	{
		m_count = 0;
	}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float) override
	{

		b2Assert(m_count < e_maxCount);

		m_points[m_count] = point;
		m_normals[m_count] = normal;
		m_hit_fixtures[m_count] = fixture;
		++m_count;

		if (m_count == e_maxCount)
		{
			// At this point the buffer is full.
			// By returning 0, we instruct the calling code to terminate the ray-cast.
			return 0.0f;
		}

		// By returning 1, we instruct the caller to continue without clipping the ray.
		return 1.0f;
	}

	b2Vec2 m_points[e_maxCount];
	b2Vec2 m_normals[e_maxCount];
	b2Fixture* m_hit_fixtures[e_maxCount];
	int32 m_count;
};

class Box2DPhysicsDirectSpaceState2D : public PhysicsDirectSpaceState2D { 
	GDCLASS(Box2DPhysicsDirectSpaceState2D, PhysicsDirectSpaceState2D);
	//PhysicsDirectSpaceState2D
public:
	static void _bind_methods(){};

	Box2DSpace *space = nullptr;

	//TypedArray<Dictionary> intersect_point(const Ref<PhysicsPointQueryParameters2D> &parameters, int32_t max_results = 32);
	Dictionary intersect_ray(const Ref<PhysicsRayQueryParameters2D> &parameters);
	//TypedArray<Dictionary> intersect_shape(const Ref<PhysicsShapeQueryParameters2D> &parameters, int32_t max_results = 32);
	//PackedFloat32Array cast_motion(const Ref<PhysicsShapeQueryParameters2D> &parameters);
	//TypedArray<PackedVector2Array> collide_shape(const Ref<PhysicsShapeQueryParameters2D> &parameters, int32_t max_results = 32);
	//Dictionary get_rest_info(const Ref<PhysicsShapeQueryParameters2D> &parameters);

	Box2DPhysicsDirectSpaceState2D() {}
};

#endif // BOX2D_SPACE_H
