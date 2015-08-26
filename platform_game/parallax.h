//
// Created by gen on 15-8-15.
//

#ifndef GODOT_MASTER_PARALLAX_H
#define GODOT_MASTER_PARALLAX_H

#include "../../core/typedefs.h"
#include "../../scene/2d/node_2d.h"
#include "../../core/math/math_2d.h"
#include "../../core/ustring.h"

class ParallaxBG : public Node2D {
    OBJ_TYPE(ParallaxBG, Node2D);
private:
    bool set_camera;
    Vector2 camera_base_position;
    Vector2 original_position;
    Vector2 scroll;
    String group_name;

    void _camera_moved(const Matrix32& p_transform);
    static void _bind_methods();

protected:

    void _notification(int p_notification);

public:

    _FORCE_INLINE_ Vector2 get_scroll() {return scroll;}
    void set_scroll(Vector2 p_scroll) {
        scroll = p_scroll;
        scroll.x = scroll.x > 1 ? 1 : (scroll.x < 0 ? 0 : scroll.x);
        scroll.y = scroll.y > 1 ? 1 : (scroll.y < 0 ? 0 : scroll.y);
    }

};


#endif //GODOT_MASTER_PARALLAX_H
