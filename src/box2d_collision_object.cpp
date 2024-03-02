#include "box2d_collision_object.h"
#include "b2_user_settings.h"

#include <godot_cpp/core/memory.hpp>

#include <box2d/b2_fixture.h>

void Box2DCollisionObject::_update_shapes() {
	if (!space) {
		return;
	}

	for (int i = 0; i < shapes.size(); i++) {
		Shape &s = shapes.write[i];
		if (s.disabled) {
			continue;
		}

		//not quite correct, should compute the next matrix..
		//Transform2D xform = transform * s.xform;

		if (s.fixtures.is_empty()) {
			int box2d_shape_count = s.shape->get_b2Shape_count();
			s.fixtures.resize(box2d_shape_count);
			for (int j = 0; j < box2d_shape_count; j++) {
				b2FixtureDef fixture_def;
				fixture_def.shape = s.shape->get_transformed_b2Shape(j, s.xform);
				fixture_def.density = 1.0f;
				fixture_def.isSensor = type == Type::TYPE_AREA;
				fixture_def.userData.shape_idx = i;
				fixture_def.userData.box2d_fixture_idx = j;
				fixture_def.filter.categoryBits = collision_layer;
				fixture_def.filter.maskBits = collision_mask;
				s.fixtures.write[j] = body->CreateFixture(&fixture_def);
			}
		}

		//space->get_broadphase()->move(s.bpid, shape_aabb);
	}
}

void Box2DCollisionObject::add_shape(Box2DShape *p_shape, const Transform2D &p_transform, bool p_disabled) {
	Shape s;
	s.shape = p_shape;
	s.xform = p_transform;
	s.disabled = p_disabled;
	shapes.push_back(s);

	// TODO (queue) update
}

void Box2DCollisionObject::set_shape(int p_index, Box2DShape *p_shape) {
	ERR_FAIL_INDEX(p_index, shapes.size());
	//shapes[p_index].shape->remove_owner(this);
	shapes.write[p_index].shape = p_shape;

	// TODO: (queue) update
}

void Box2DCollisionObject::set_shape_transform(int p_index, const Transform2D &p_transform) {
	ERR_FAIL_INDEX(p_index, shapes.size());

	shapes.write[p_index].xform = p_transform;

	// TODO: (queue) update
}

void Box2DCollisionObject::set_shape_disabled(int p_index, bool p_disabled) {
	ERR_FAIL_INDEX(p_index, shapes.size());

	Shape &shape = shapes.write[p_index];
	if (shape.disabled == p_disabled) {
		return;
	}

	shape.disabled = p_disabled;

	if (!space) {
		return;
	}

	for (int j = 0; j < shape.fixtures.size(); j++) {
		body->DestroyFixture(shape.fixtures[j]);
		shape.fixtures.write[j] = nullptr;
	}
	shape.fixtures.clear();

	// TODO: (queue) update
}

void Box2DCollisionObject::remove_shape(Box2DShape *p_shape) {
	//remove a shape, all the times it appears
	for (int i = 0; i < shapes.size(); i++) {
		if (shapes[i].shape == p_shape) {
			remove_shape(i);
			i--;
		}
	}
}

void Box2DCollisionObject::remove_shape(int p_index) {
	//remove anything from shape to be erased to end, so subindices don't change
	ERR_FAIL_INDEX(p_index, shapes.size());
	for (int i = p_index; i < shapes.size(); i++) {
		Shape &shape = shapes.write[i];
		for (int j = 0; j < shape.fixtures.size(); j++) {
			// should never get here with a null owner
			body->DestroyFixture(shape.fixtures[j]);
			shape.fixtures.write[j] = nullptr;
		}
		shape.fixtures.clear();
	}
	shapes.remove_at(p_index);

	// TODO: (queue) update
}

void Box2DCollisionObject::_set_space(Box2DSpace *p_space) {
	if (space) {
		// NOTE: Remember the transform by copying it from the b2Body to the b2BodyDef.
		body_def->position = body->GetPosition();
		body_def->angle = body->GetAngle();

		for (int i = 0; i < shapes.size(); i++) {
			Shape &shape = shapes.write[i];
			for (int j = 0; j < shape.fixtures.size(); j++) {
				body->DestroyFixture(shape.fixtures[j]);
				shape.fixtures.write[j] = nullptr;
			}
			shape.fixtures.clear();
		}
		space->remove_object(this);
	}
	space = p_space;
	if (space) {
		space->add_object(this);
		_update_shapes();
	}
}

Box2DCollisionObject::Box2DCollisionObject(Type p_type) {
	type = p_type;
	body_def = memnew(b2BodyDef);
	body_def->userData.pointer = reinterpret_cast<uintptr_t>(this);
}

void Box2DCollisionObject::set_collision_layer(uint32_t layer) {
	//get the existing filter
	collision_layer = layer;
	if(!body)
		return;
	b2Fixture* m_fixtureList = body->GetFixtureList();
	for (b2Fixture* f = m_fixtureList; f; f = f->GetNext()) {
		b2Filter filter = f->GetFilterData();
		//change whatever you need to, eg.
		filter.categoryBits = layer;
		//and set it back
		f->SetFilterData(filter);
		
	}
}

void Box2DCollisionObject::set_collision_mask(uint32_t mask) {
	//get the existing filter
	collision_mask = mask;
	
	if(!body)
		return;
	b2Fixture* m_fixtureList = body->GetFixtureList();
	for (b2Fixture* f = m_fixtureList; f; f = f->GetNext()) {
		b2Filter filter = f->GetFilterData();
		//change whatever you need to, eg.
		filter.maskBits = mask;
		//and set it back
		f->SetFilterData(filter);
	}
}

uint32_t Box2DCollisionObject::get_collision_mask() {
	//get the existing filter
	if(!body)
		return collision_mask;
	b2Fixture* m_fixtureList = body->GetFixtureList();
	if(m_fixtureList) {
		b2Filter filter = m_fixtureList->GetFilterData();
		//change whatever you need to, eg.
		return filter.maskBits;
	}
	return collision_mask;
}

uint32_t Box2DCollisionObject::get_collision_layer() {
	//get the existing filter
	if(!body)
		return collision_layer;
	b2Fixture* m_fixtureList = body->GetFixtureList();
	if(m_fixtureList) {
		b2Filter filter = m_fixtureList->GetFilterData();
		//change whatever you need to, eg.
		return filter.categoryBits;
	}
	return collision_layer;
}

Box2DCollisionObject::~Box2DCollisionObject() {
	memdelete(body_def);
}
