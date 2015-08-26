//
// Created by gen on 15-5-21.
//

#include "buff_process.h"
#include "../character.h"

BehaviorNode::Status BuffProcess::_behavior(const Variant &target, Dictionary env) {
    Character *character = ((Object*)target)->cast_to<Character>();
    Array buffs = character->get_buffs();
    for (int i = 0, t = buffs.size(); i < t; ++i) {
        Ref<Buff> buff = buffs[i];
        buff->step(character, env);
    }
    return BehaviorNode::STATUS_RUNNING;
}