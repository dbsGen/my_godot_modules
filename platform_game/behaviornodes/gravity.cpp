//
// Created by gen on 15-5-24.
//

#include "gravity.h"
#include "../character.h"

BehaviorNode::Status Gravity::_behavior(const Variant &target, Dictionary env) {
    Character *character = Object::cast_to<Character>((Object*)target);
    if (character->get_on_floor()) {
    }else {
        Vector2 v2 = env["move"];
        v2 += gravity_direction*accelerated;
        env["move"] = v2;
    }
	return BehaviorNode::_behavior(target, env);
}

void Gravity::_bind_methods() {

    ClassDB::bind_method(D_METHOD("set_accelerated", "accelerated"), &Gravity::set_accelerated);
    ClassDB::bind_method(D_METHOD("get_accelerated"), &Gravity::get_accelerated);

    ClassDB::bind_method(D_METHOD("set_gravity_direction", "gravity_direction"), &Gravity::set_gravity_direction);
    ClassDB::bind_method(D_METHOD("get_gravity_direction"), &Gravity::get_gravity_direction);

    ADD_PROPERTY(PropertyInfo( Variant::REAL, "gravity/accelerated" ), "set_accelerated","get_accelerated");
    ADD_PROPERTY(PropertyInfo( Variant::VECTOR2, "gravity/gravity_direction" ), "set_gravity_direction","get_gravity_direction");
}