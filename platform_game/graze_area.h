//
// Created by Gen on 2016/1/21.
//

#ifndef GODOT_NEW_GRAZE_AREA_H
#define GODOT_NEW_GRAZE_AREA_H

#include "../../scene/2d/area_2d.h"
#include "../../core/object.h"
#include "../../core/math/math_2d.h"

class GrazeArea : public Area2D {
    GDCLASS(GrazeArea, Area2D);
private:
    bool graze_enable;
    Vector<uint32_t> grazed;

protected:
    static void _bind_methods();

    virtual void _on_graze(const Vector2 &pos) {}

public:
    void graze(RID area);

    _FORCE_INLINE_ void set_graze_enable(bool p_enable) {graze_enable = p_enable;if (!graze_enable)grazed.clear();}
    _FORCE_INLINE_ bool get_graze_enable() {return graze_enable;}

    _FORCE_INLINE_ GrazeArea(){}
};

#endif //GODOT_NEW_GRAZE_AREA_H
