//
// Created by Gen on 15-5-21.
//

#ifndef GODOT_MASTER_HITSTATUSPROGRESS_H
#define GODOT_MASTER_HITSTATUSPROGRESS_H

#include "../../behaviornode/behaviornode.h"
#include "../hit_status/hit_status.h"

class HitStatusProgress : public BehaviorNode {
    OBJ_TYPE(HitStatusProgress, BehaviorNode);
private:
    HitStatus::HSType old_hit_type;
    HitStatus::HSType hit_type;
protected:
    static void _bind_methods();

    virtual bool   _pre_behavior(const Variant& target, Dictionary env);
    virtual Status _behavior(const Variant& target, Dictionary env);
    virtual bool _on_type_change(HitStatus::HSType new_type) {return true;}

public:
    _FORCE_INLINE_ HitStatus::HSType get_hit_type() {return hit_type;}
    Vector2 process_hit(const Variant& target, Dictionary env);

    HitStatusProgress(){old_hit_type=HitStatus::HS_NO_HIT;}
};


#endif //GODOT_MASTER_HITSTATUSPROGRESS_H
