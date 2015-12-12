//
// Created by gen on 15-7-12.
//

#include "kill_zone.h"
#include "../../core/array.h"
#include "../../core/object.h"
#include "character.h"

void KillZone::_on_body_enter(Object *body) {
    Character *cha = body->cast_to<Character>();
    if (cha) {
        cha->call("kill");
    }
}

void KillZone::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("_on_body_enter", "body"), &KillZone::_on_body_enter);
}