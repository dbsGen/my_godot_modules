//
// Created by gen on 15-7-12.
//

#include "kill_zone.h"
#include "../../core/array.h"
#include "../../core/object.h"
#include "character.h"

void KillZone::_on_body_enter(Object *body) {
    Character *cha = Object::cast_to<Character>(body);
    if (cha) {
        cha->call("kill");
    }
}

void KillZone::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_body_enter", "body"), &KillZone::_on_body_enter);
}