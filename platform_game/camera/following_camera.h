//
// Created by gen on 15-7-1.
//

#ifndef GODOT_MASTER_FOLLOWING_CAMERA_H
#define GODOT_MASTER_FOLLOWING_CAMERA_H

#include "../../../scene/2d/camera_2d.h"
#include "../../../scene/2d/node_2d.h"
#include "../../../core/typedefs.h"

class FollowingCamera : public Camera2D {
    OBJ_TYPE( FollowingCamera, Camera2D );
private:

    Node2D *target;
    NodePath target_path;
    void update_target();
    void _on_target_exit();

    float follow;
    float limit_left;
    float limit_top;
    float limit_right;
    float limit_bottom;

    float shaking_during;
    float shaking_time;
    Vector2 shake_force;
    Vector2 shake_base;
protected:

    void _notification(int p_what);
    static void _bind_methods();
    virtual Matrix32 get_camera_transform();
public:

    _FORCE_INLINE_ NodePath get_target_path() {return target_path;}
    _FORCE_INLINE_ void set_target_path(NodePath p_target_path) {target_path=p_target_path;update_target();}

    _FORCE_INLINE_ float get_follow() {return follow;}
    _FORCE_INLINE_ void set_follow(float p_follow) {follow=p_follow;}

    _FORCE_INLINE_ float get_limit_left(){return limit_left;}
    _FORCE_INLINE_ void set_limit_left(float p_limit_left) {limit_left = p_limit_left;}

    _FORCE_INLINE_ float  get_limit_top() {return limit_top;}
    _FORCE_INLINE_ void set_limit_top(float p_limit_top){limit_top=p_limit_top;}

    _FORCE_INLINE_ float  get_limit_right() {return limit_right;}
    _FORCE_INLINE_ void set_limit_right(float p_limit_right){limit_right=p_limit_right;}

    _FORCE_INLINE_ float  get_limit_bottom() {return limit_bottom;}
    _FORCE_INLINE_ void set_limit_bottom(float p_limit_bottom){limit_bottom=p_limit_bottom;}

    void shake(float during, Vector2 force);

    FollowingCamera() {
        target=NULL;
        follow=1;
        limit_left      = -100000;
        limit_top       = -100000;
        limit_right     = 100000;
        limit_bottom    = 100000;
        shaking_during  = 0;
    }
};


#endif //GODOT_MASTER_FOLLOWING_CAMERA_H
