//
// Created by gen on 15-12-5.
//

#include "hit_area.h"

void HitArea::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            character = Object::cast_to<Character>(get_parent());
            if (character) {
                character->has_hit_area = true;
                set_collision_layer(character->get_collision_layer());
                set_collision_mask(character->get_collision_mask());
            }
        }
    }
}

bool HitArea::attack_by(Ref<HitStatus> p_hit_status, Object *from) {
    if (character) {
        return character->attack_by(p_hit_status, Object::cast_to<Character>(from), true);
    }else {
        return false;
    }
}

void HitArea::_bind_methods() {
    
    ClassDB::bind_method(D_METHOD("get_character"), &HitArea::get_character);
    ClassDB::bind_method(D_METHOD("attack_by", "hit_status:HitStatus", "from:Character"), &HitArea::attack_by);
    
}