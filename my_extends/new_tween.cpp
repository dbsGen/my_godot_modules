//
// Created by gen on 15-4-26.
//

#include "new_tween.h"
#include "../../core/math/math_funcs.h"
#include "tween_easing.h"
#include "../../core/os/os.h"
#include "../../scene/main/viewport.h"
#include "../../scene/main/scene_main_loop.h"

//===================TweenNode

void TweenNode::check_queue() {
    if (actions.size() > 0 && !is_processing()) {
        set_process(true);
        set_pause_mode(Node::PAUSE_MODE_PROCESS);
    }
}

void TweenNode::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_PROCESS: {
            float delta = get_process_delta_time();
            for (int n = 0, t = actions.size(); n < t; n++) {
                Ref<TweenAction> act = actions[n];
                if (act->step(delta)) {
                    actions.remove(n);
                    n -= 1;
                    t -= 1;
                }
            }
            if (actions.size() == 0) {
                set_process(false);
            }
        };
    }
}

//===================TweenProperty

Variant TweenProperty::interpolation(float per) {
    return lerp(from_value, to_value, per);
}

Variant TweenProperty::lerp(Variant from, Variant to, float per) {
    Variant result;

#define APPLY_EQUATION(element)\
	r.element = Math::lerp(i.element, d.element, per);

    switch(from.get_type())
    {

        case Variant::BOOL:
            result = ((int) Math::lerp((int)from, (int)to, per)) >= 0.5;
            break;

        case Variant::INT:
            result = ((int) Math::lerp((int)from, (int)to, per));
            break;

        case Variant::REAL:
            result = Math::lerp(from, to, per);
            break;

        case Variant::VECTOR2:
        {
            Vector2 i = from;
            Vector2 d = to;
            Vector2 r;

            APPLY_EQUATION(x);
            APPLY_EQUATION(y);

            result = r;
        }
            break;

        case Variant::VECTOR3:
        {
            Vector3 i = from;
            Vector3 d = to;
            Vector3 r;

            APPLY_EQUATION(x);
            APPLY_EQUATION(y);
            APPLY_EQUATION(z);

            result = r;
        }
            break;

        case Variant::MATRIX3:
        {
            Matrix3 i = from;
            Matrix3 d = to;
            Matrix3 r;

            APPLY_EQUATION(elements[0][0]);
            APPLY_EQUATION(elements[0][1]);
            APPLY_EQUATION(elements[0][2]);
            APPLY_EQUATION(elements[1][0]);
            APPLY_EQUATION(elements[1][1]);
            APPLY_EQUATION(elements[1][2]);
            APPLY_EQUATION(elements[2][0]);
            APPLY_EQUATION(elements[2][1]);
            APPLY_EQUATION(elements[2][2]);

            result = r;
        }
            break;

        case Variant::MATRIX32:
        {
            Matrix3 i = from;
            Matrix3 d = to;
            Matrix3 r;

            APPLY_EQUATION(elements[0][0]);
            APPLY_EQUATION(elements[0][1]);
            APPLY_EQUATION(elements[1][0]);
            APPLY_EQUATION(elements[1][1]);
            APPLY_EQUATION(elements[2][0]);
            APPLY_EQUATION(elements[2][1]);

            result = r;
        }
            break;
        case Variant::QUAT:
        {
            Quat i = from;
            Quat d = to;
            Quat r;

            APPLY_EQUATION(x);
            APPLY_EQUATION(y);
            APPLY_EQUATION(z);
            APPLY_EQUATION(w);

            result = r;
        }
            break;
        case Variant::_AABB:
        {
            AABB i = from;
            AABB d = to;
            AABB r;

            APPLY_EQUATION(pos.x);
            APPLY_EQUATION(pos.y);
            APPLY_EQUATION(pos.z);
            APPLY_EQUATION(size.x);
            APPLY_EQUATION(size.y);
            APPLY_EQUATION(size.z);

            result = r;
        }
            break;
        case Variant::TRANSFORM:
        {
            Transform i = from;
            Transform d = to;
            Transform r;

            APPLY_EQUATION(basis.elements[0][0]);
            APPLY_EQUATION(basis.elements[0][1]);
            APPLY_EQUATION(basis.elements[0][2]);
            APPLY_EQUATION(basis.elements[1][0]);
            APPLY_EQUATION(basis.elements[1][1]);
            APPLY_EQUATION(basis.elements[1][2]);
            APPLY_EQUATION(basis.elements[2][0]);
            APPLY_EQUATION(basis.elements[2][1]);
            APPLY_EQUATION(basis.elements[2][2]);
            APPLY_EQUATION(origin.x);
            APPLY_EQUATION(origin.y);
            APPLY_EQUATION(origin.z);

            result = r;
        }
            break;
        case Variant::COLOR:
        {
            Color i = from;
            Color d = to;
            Color r;

            APPLY_EQUATION(r);
            APPLY_EQUATION(g);
            APPLY_EQUATION(b);
            APPLY_EQUATION(a);

            result = r;
        }
            break;
        default:
            ERR_FAIL_V("Type not support.");
            break;
    };
#undef APPLY_EQUATION

    return result;
}

//===================TweenAction

TweenAction * TweenAction::add_method(const StringName &method_name, const Variant &from, const Variant &to) {
    Ref<TweenProperty> property = memnew(TweenProperty);
    property->property_type = 0;
    property->property_name = method_name;
    property->to_value = to;
    property->from_value = from;
    properties.push_back(property);
    return this;
}

TweenAction * TweenAction::add_property(const StringName &property_name, const Variant &from, const Variant &to) {
    Ref<TweenProperty> property = memnew(TweenProperty);
    property->property_type = 1;
    property->property_name = property_name;
    property->to_value = to;
    property->from_value = from;
    properties.push_back(property);
    return this;
}

TweenAction *TweenAction::add_callback(const Variant &target, const StringName& method_name, float step) {
    Ref<TweenProperty> property = memnew(TweenProperty);
    property->property_type = 2;
    property->property_name = method_name;
    property->step = step;
    property->step_count = 0;
    property->callback_target = target;
    properties.push_back(property);
    return this;
}

bool TweenAction::step(float delta) {
    if (status == TWEEN_STATUS_CANCEL) {
        return true;
    }
    delta_time += delta;
    float pe = 0;
    bool br = false;
    if (delta_time >= total_time+delay_time || status == TWEEN_STATUS_END) {
        pe = 1;
        br = true;
    }else {
        pe = (delta_time-delay_time)/total_time;
    }
    float np = tweenEasingFuncs[easing](pe);
    for (int i = 0, t = properties.size(); i < t; ++i) {
        Ref<TweenProperty> property = properties[i];
        switch (property->property_type) {
            case 0: {
                Variant res = property->interpolation(np);
                target->call(property->property_name, res);
                break;
            }
            case 1: {
                Variant res = property->interpolation(np);
                target->set(property->property_name, res);
                break;
            }
            case 2:
            {
                int count = (int)(delta_time/property->step);
                if (count > property->step_count) {
                    ((Object*)property->callback_target)->call(property->property_name);
                    property->step_count = count;
                }
                break;
            }

        }
    }

    emit_signal("update", Variant(pe));
    if (br) {
        emit_signal("finished");
    }
    return br;
}

void TweenAction::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("add_method:TweenAction", "method_name", "from", "to"),&TweenAction::add_method);
    ObjectTypeDB::bind_method(_MD("add_property:TweenAction", "property_name", "from", "to"),&TweenAction::add_property);
    ObjectTypeDB::bind_method(_MD("add_callback:TweenAction", "target", "method_name", "step"),&TweenAction::add_callback);
    ObjectTypeDB::bind_method(_MD("cancel"),&TweenAction::cancel);
    ObjectTypeDB::bind_method(_MD("end"),&TweenAction::end);
    ObjectTypeDB::bind_method(_MD("set_easing", "easing"),&TweenAction::set_easing);
    ObjectTypeDB::bind_method(_MD("get_easing"),&TweenAction::get_easing);
    ObjectTypeDB::bind_method(_MD("get_total_time"),&TweenAction::get_total_time);
    ObjectTypeDB::bind_method(_MD("get_delta_time"),&TweenAction::get_delta_time);
    ObjectTypeDB::bind_method(_MD("get_delay_time"),&TweenAction::get_delay_time);
    ObjectTypeDB::bind_method(_MD("set_delay_time", "delay_time"),&TweenAction::set_delay_time);

    ADD_SIGNAL( MethodInfo("finished") );
    ADD_SIGNAL( MethodInfo("update", PropertyInfo(Variant::REAL, "progress")) );
}

void TweenAction::cancel() {
    status = TWEEN_STATUS_CANCEL;
}
void TweenAction::end() {
    status = TWEEN_STATUS_END;
}

//==================NewTween

Ref<TweenAction> NewTween::to(Object *target, float during) {
    const String tween_key = "new_tween";
    MainLoop *main_loop = OS::get_singleton()->get_main_loop();
    SceneTree *tree = main_loop->cast_to<SceneTree>();
    ERR_FAIL_COND_V(tree == NULL, NULL);

    Viewport *viewport = tree->get_root();
    ERR_FAIL_COND_V(viewport == NULL, NULL);
    if (tweenNode == NULL) {
        tweenNode = memnew(TweenNode);
        tweenNode->set_name(tween_key);

        Vector<Variant> vector;
        vector.push_back(Variant(tweenNode));

        tree->connect(StringName("idle_frame"), this, StringName("_add_node"), vector, 0);

    }


    Ref<TweenAction> action = memnew(TweenAction);
    action->total_time = during;
    action->target = target;
    tweenNode->actions.push_back(action);
    tweenNode->check_queue();
    return action;
}

void NewTween::cancel(Object *target) {
    if (tweenNode != NULL) {
        for (int i = 0, t = tweenNode->actions.size(); i < t; ++i) {
            Ref<TweenAction> action = tweenNode->actions[i];
            if (target == action->target) {
                action->cancel();
            }
        }
    }
}

void NewTween::_add_node(Object *node) {
    MainLoop *main_loop = OS::get_singleton()->get_main_loop();
    SceneTree *tree = main_loop->cast_to<SceneTree>();
    ERR_FAIL_COND(tree == NULL);

    tree->disconnect("idle_frame", this, "_add_node");
    tree->get_root()->add_child(node->cast_to<TweenNode>());
}

void NewTween::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("to:TweenAction", "target", "during"),&NewTween::to);
    ObjectTypeDB::bind_method(_MD("cancel", "target"),&NewTween::cancel);
    ObjectTypeDB::bind_method(_MD("_add_node", "node"),&NewTween::_add_node);

    BIND_CONSTANT(TWEEN_EASING_LINEAR);
    BIND_CONSTANT(TWEEN_EASING_QUADRATIC_IN);
    BIND_CONSTANT(TWEEN_EASING_QUADRATIC_OUT);
    BIND_CONSTANT(TWEEN_EASING_QUADRATIC_IN_OUT);
    BIND_CONSTANT(TWEEN_EASING_CUBIC_IN);
    BIND_CONSTANT(TWEEN_EASING_CUBIC_OUT);
    BIND_CONSTANT(TWEEN_EASING_CUBIC_IN_OUT);
    BIND_CONSTANT(TWEEN_EASING_QUARTIC_IN);
    BIND_CONSTANT(TWEEN_EASING_QUARTIC_OUT);
    BIND_CONSTANT(TWEEN_EASING_QUARTIC_IN_OUT);
    BIND_CONSTANT(TWEEN_EASING_QUINTIC_IN);
    BIND_CONSTANT(TWEEN_EASING_QUINTIC_OUT);
    BIND_CONSTANT(TWEEN_EASING_QUINTIC_IN_OUT);
    BIND_CONSTANT(TWEEN_EASING_SINUSOIDAL_IN);
    BIND_CONSTANT(TWEEN_EASING_SINUSOIDAL_OUT);
    BIND_CONSTANT(TWEEN_EASING_SINUSOIDAL_IN_OUT);
    BIND_CONSTANT(TWEEN_EASING_EXPONENTIAL_IN);
    BIND_CONSTANT(TWEEN_EASING_EXPONENTIAL_OUT);
    BIND_CONSTANT(TWEEN_EASING_EXPONENTIAL_IN_OUT);
    BIND_CONSTANT(TWEEN_EASING_CIRCULAR_IN);
    BIND_CONSTANT(TWEEN_EASING_CIRCULAR_OUT);
    BIND_CONSTANT(TWEEN_EASING_CIRCULAR_IN_OUT);
    BIND_CONSTANT(TWEEN_EASING_ELASTIC_IN);
    BIND_CONSTANT(TWEEN_EASING_ELASTIC_OUT);
    BIND_CONSTANT(TWEEN_EASING_ELASTIC_IN_OUT);
    BIND_CONSTANT(TWEEN_EASING_BACK_IN);
    BIND_CONSTANT(TWEEN_EASING_BACK_OUT);
    BIND_CONSTANT(TWEEN_EASING_BACK_IN_OUT);
    BIND_CONSTANT(TWEEN_EASING_BOUNCE_IN);
    BIND_CONSTANT(TWEEN_EASING_BOUNCE_OUT);
    BIND_CONSTANT(TWEEN_EASING_BOUNCE_IN_OUT);
}