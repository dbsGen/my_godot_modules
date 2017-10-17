//
// Created by gen on 15-4-26.
//

#ifndef GODOT_MASTER_NEW_TWEEN_H
#define GODOT_MASTER_NEW_TWEEN_H

#include "../../core/reference.h"
#include "../../core/object.h"
#include "../../scene/main/node.h"

class TweenAction;

class TweenNode: public Node {
    GDCLASS(TweenNode, Node);
private:
    Vector< Ref<TweenAction> > actions;
    void check_queue();
    friend class NewTween;
protected:
    void _notification(int p_what);
};

class TweenProperty : public Reference {
    GDCLASS(TweenProperty, Reference);
private:
    friend class TweenAction;
    StringName property_name;
    int property_type;
    Variant from_value;
    Variant to_value;

    Variant callback_target;
    float step;
    int step_count;

    Variant interpolation(float per);
    Variant lerp(Variant from, Variant to, float per);
};

class NewTween: public Reference {
    GDCLASS(NewTween, Reference);
private:
    TweenNode   *tween_node;
protected:
    static void _bind_methods();
public:
    typedef enum {
        TWEEN_EASING_LINEAR,
        TWEEN_EASING_QUADRATIC_IN,
        TWEEN_EASING_QUADRATIC_OUT,
        TWEEN_EASING_QUADRATIC_IN_OUT,
        TWEEN_EASING_CUBIC_IN,
        TWEEN_EASING_CUBIC_OUT,
        TWEEN_EASING_CUBIC_IN_OUT,
        TWEEN_EASING_QUARTIC_IN,
        TWEEN_EASING_QUARTIC_OUT,
        TWEEN_EASING_QUARTIC_IN_OUT,
        TWEEN_EASING_QUINTIC_IN,
        TWEEN_EASING_QUINTIC_OUT,
        TWEEN_EASING_QUINTIC_IN_OUT,
        TWEEN_EASING_SINUSOIDAL_IN,
        TWEEN_EASING_SINUSOIDAL_OUT,
        TWEEN_EASING_SINUSOIDAL_IN_OUT,
        TWEEN_EASING_EXPONENTIAL_IN,
        TWEEN_EASING_EXPONENTIAL_OUT,
        TWEEN_EASING_EXPONENTIAL_IN_OUT,
        TWEEN_EASING_CIRCULAR_IN,
        TWEEN_EASING_CIRCULAR_OUT,
        TWEEN_EASING_CIRCULAR_IN_OUT,
        TWEEN_EASING_ELASTIC_IN,
        TWEEN_EASING_ELASTIC_OUT,
        TWEEN_EASING_ELASTIC_IN_OUT,
        TWEEN_EASING_BACK_IN,
        TWEEN_EASING_BACK_OUT,
        TWEEN_EASING_BACK_IN_OUT,
        TWEEN_EASING_BOUNCE_IN,
        TWEEN_EASING_BOUNCE_OUT,
        TWEEN_EASING_BOUNCE_IN_OUT,
    } Tween_Easing;
    void _add_node(Object *node);
    Ref<TweenAction> to(Object *target, float during);
    void cancel(Object *target);

    NewTween() {tween_node = NULL;}
    ~NewTween();
};

class TweenAction : public Reference {
    GDCLASS(TweenAction, Reference);
private:
    typedef enum {
        TWEEN_STATUS_NORMAL,
        TWEEN_STATUS_CANCEL,
        TWEEN_STATUS_END
    } TweenStatus;
    float total_time;
    float delta_time;
    float delay_time;
    Object *target;
    TweenStatus status;
    NewTween::Tween_Easing easing;

    bool step(float delta);

    Vector< Ref<TweenProperty> > properties;
    friend class NewTween;
    friend class TweenNode;
    void set_target(Object* target);
    void _on_target_exit();
protected:
    static void _bind_methods();
public:
    TweenAction *add_method(const StringName& method_name, const Variant &from, const Variant& to);
    TweenAction *add_property(const StringName& property_name, const Variant &from, const Variant& to);
    TweenAction *add_callback(const Variant &target, const StringName& method_name, float step);

    void cancel();
    void end();

    TweenAction *set_easing(NewTween::Tween_Easing e) {easing = e;return this;}
    NewTween::Tween_Easing get_easing() {return easing;}

    float get_total_time() {return total_time;}
    float get_delta_time() {return delta_time;}
    float get_delay_time() {return delay_time;}
    TweenAction *set_delay_time(float dt) {delay_time = dt;return this;}

    TweenAction() {
        easing = NewTween::TWEEN_EASING_LINEAR;
        status = TWEEN_STATUS_NORMAL;
        delay_time = 0;
        total_time = 0;
        delta_time = 0;
        target = NULL;
    }
    ~TweenAction();
};

VARIANT_ENUM_CAST(NewTween::Tween_Easing);

#endif //GODOT_MASTER_NEW_TWEEN_H
