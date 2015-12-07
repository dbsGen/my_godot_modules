//
// Created by gen on 15-12-5.
//

#ifndef GODOT_MASTER_HIT_AREA_H
#define GODOT_MASTER_HIT_AREA_H

#include "../../scene/2d/area_2d.h"
#include "character.h"

class HitArea : public Area2D {

    OBJ_TYPE(HitArea, Area2D);
private:
    Character *character;
protected:
    void _notification(int p_notification);
public:
    _FORCE_INLINE_ Character *get_owner() {return character;}
    bool attack_by(Ref<HitStatus> p_hit_status, Character *from);

    _FORCE_INLINE_ HitArea() {character=NULL;}
};

#endif //GODOT_MASTER_HIT_AREA_H
