//
// Created by gen on 15-5-24.
//

#ifndef GODOT_MASTER_GRAVITY_H
#define GODOT_MASTER_GRAVITY_H

#include "../../behaviornode/behaviornode.h"
#include "../../../core/object.h"
#include "../../../core/math/math_2d.h"

class Gravity : public BehaviorNode {
    GDCLASS(Gravity, BehaviorNode);
private:
    float accelerated;
    Vector2 gravity_direction;
protected:
    static void _bind_methods();
    virtual Status _behavior(const Variant& target, Dictionary env);

public:

    _FORCE_INLINE_ void set_accelerated(float p_accelerated) {accelerated = p_accelerated;}
    _FORCE_INLINE_ float get_accelerated() {return accelerated;}

    _FORCE_INLINE_ void set_gravity_direction(Vector2 p_gravity_direction) {gravity_direction=p_gravity_direction.normalized();}
    _FORCE_INLINE_ Vector2 get_gravity_direction() {return gravity_direction;}

    Gravity(){accelerated=20;gravity_direction=Vector2(0,1);}
};


#endif //GODOT_MASTER_GRAVITY_H
