//
// Created by gen on 15-5-24.
//

#ifndef GODOT_MASTER_ACTION_H
#define GODOT_MASTER_ACTION_H

#include "../../behaviornode/timebnode.h"
#include "../../../core/object.h"
#include "../../../core/reference.h"

class Action : public TimerBNode {
OBJ_TYPE(Action, TimerBNode);
public:
    enum CancelType{
        CANCEL_TYPE_TIME,
        CANCEL_TYPE_HIT,
        CANCEL_TYPE_NONE
    };
private:
    Vector< Action* > cancel_list;
    bool checked_cancel_list;
    bool _is_hit;
    float cancel_time;
    CancelType cancel_type;

    float max_move;
    float drag;
    float old_move;

    bool reset_from_cancel;

    Node*       animation_node;
    NodePath    animation_path;
    void update_animation_path();
    String      animation_type;
    String      animation_name;

    void        cancel_animation();
protected:
    void _notification(int p_notification);
    virtual void    _during_behavior(const Variant& target, Dictionary& env);
    virtual void    _timeout_behavior(const Variant& target, Dictionary& env);
    virtual void    _cancel_behavior(const Variant& target);
    virtual Status  _behavior(const Variant& target, Dictionary env);
    virtual Status  _traversal_children(const Variant& target, Dictionary& env);
    virtual void    _reset(const Variant& target);
    static void _bind_methods();
public:
    void refresh_cancel_list();

    _FORCE_INLINE_ bool get_hit() {return _is_hit;}
    _FORCE_INLINE_ void set_hit(bool hit) {_is_hit = hit;}

    _FORCE_INLINE_ float get_max_move() {return max_move;}
    _FORCE_INLINE_ void set_max_move(float p_max_move) {if (p_max_move>=0)max_move=p_max_move;}

    _FORCE_INLINE_ float get_drag() {return drag;}
    _FORCE_INLINE_ void set_drag(float p_drag) {if (p_drag >= 0 && p_drag < max_move)drag=p_drag;}

    _FORCE_INLINE_ float get_cancel_time() {return cancel_time;}
    _FORCE_INLINE_ void set_cancel_time(float p_cancel_time) {cancel_time=p_cancel_time;}

    _FORCE_INLINE_ CancelType get_cancel_type() {return cancel_type;}
    _FORCE_INLINE_ void set_cancel_type(CancelType p_cancel_type) {cancel_type=p_cancel_type;}

    _FORCE_INLINE_ NodePath get_animation_path() {return animation_path;}
    void set_animation_path(NodePath path);

    _FORCE_INLINE_ Node *get_animation_node() {return animation_node;}

    _FORCE_INLINE_ String get_animation_type() {return animation_type;}
    _FORCE_INLINE_ void set_animation_type(String type) {animation_type=type;}

    _FORCE_INLINE_ String get_animation_name() {return animation_name;}
    _FORCE_INLINE_ void set_animation_name(String name) {animation_name = name;}

    Action() {checked_cancel_list= false;_is_hit= false;cancel_time=0.5;animation_node=NULL;max_move=2;drag=0.2;reset_from_cancel = false;}
};

VARIANT_ENUM_CAST(Action::CancelType);


#endif //GODOT_MASTER_ACTION_H
