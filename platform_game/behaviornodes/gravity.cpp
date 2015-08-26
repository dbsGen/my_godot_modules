//
// Created by gen on 15-5-24.
//

#include "gravity.h"
#include "../character.h"

BehaviorNode::Status Gravity::_behavior(const Variant &target, Dictionary env) {
    Character *character = ((Object*)target)->cast_to<Character>();
    if (character->get_on_floor()) {
    }else {
        Vector2 v2 = env["move"];
        v2 += gravity_direction*accelerated;
        env["move"] = v2;
    }
	return BehaviorNode::_behavior(target, env);
}

void Gravity::_bind_methods() {


    ObjectTypeDB::bind_method(_MD("set_accelerated", "accelerated"), &Gravity::set_accelerated);
    ObjectTypeDB::bind_method(_MD("get_accelerated"), &Gravity::get_accelerated);

    ObjectTypeDB::bind_method(_MD("set_gravity_direction", "gravity_direction"), &Gravity::set_gravity_direction);
    ObjectTypeDB::bind_method(_MD("get_gravity_direction"), &Gravity::get_gravity_direction);

    ADD_PROPERTY(PropertyInfo( Variant::REAL, "gravity/accelerated" ), _SCS("set_accelerated"),_SCS("get_accelerated" ));
    ADD_PROPERTY(PropertyInfo( Variant::VECTOR2, "gravity/gravity_direction" ), _SCS("set_gravity_direction"),_SCS("get_gravity_direction" ));
}