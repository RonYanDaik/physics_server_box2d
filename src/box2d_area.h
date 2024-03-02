#ifndef BOX2D_AREA_H
#define BOX2D_AREA_H

#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/variant/callable.hpp>

#include "box2d_collision_object.h"
#include "box2d_space.h"

#include <godot_cpp/classes/area2d.hpp>

using namespace godot;

class Box2DArea : public Box2DCollisionObject {
public:

	Callable ares_body_callback;
	Callable ares_area_callback;
	bool is_monitorable = false;
	bool monitoring = true;
	
	void set_transform(const Transform2D &p_transform);

	void set_space(Box2DSpace *p_space) override;

	Box2DArea();
	~Box2DArea();

};

#endif // BOX2D_AREA_H
