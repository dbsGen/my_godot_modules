//
// Created by gen on 15-7-12.
//

#ifndef GODOT_MASTER_KILL_ZONE_H
#define GODOT_MASTER_KILL_ZONE_H

#include "../../scene/2d/area_2d.h"

class KillZone : public Area2D {
    OBJ_TYPE(KillZone, Area2D);
private:
    void _on_body_enter(Object *body);
protected:
    static void _bind_methods();
public:
    KillZone() {
        connect("body_enter", this, "_on_body_enter");
    }
};


#endif //GODOT_MASTER_KILL_ZONE_H
