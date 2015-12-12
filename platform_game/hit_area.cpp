//
// Created by gen on 15-12-5.
//

#include "hit_area.h"

void HitArea::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_ENTER_TREE: {
            character = get_parent()->cast_to<Character>();
            if (character) {
                character->has_hit_area = true;
                set_layer_mask(character->get_layer_mask());
                set_collision_mask(character->get_collision_mask());
            }
        }
    }
}

bool HitArea::attack_by(Ref<HitStatus> p_hit_status, Character *from) {
    if (character) {
        return character->attack_by(p_hit_status, from, true);
    }else {
        return false;
    }
}