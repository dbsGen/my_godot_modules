#include "timebnode.h"

void TimerBNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_delay"), &TimerBNode::get_delay);
    ClassDB::bind_method(D_METHOD("set_delay", "delay"), &TimerBNode::set_delay);

    ClassDB::bind_method(D_METHOD("get_time"), &TimerBNode::get_time);
    ClassDB::bind_method(D_METHOD("time_out"), &TimerBNode::time_out);
    ClassDB::bind_method(D_METHOD("cancel"), &TimerBNode::cancel);

    ClassDB::bind_method(D_METHOD("recount"), &TimerBNode::recount);
    ClassDB::bind_method(D_METHOD("recount_to", "time"), &TimerBNode::recount_to);

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "timer/delay" ), "set_delay","get_delay");
    BIND_VMETHOD( MethodInfo("_during_behavior", PropertyInfo(Variant::OBJECT,"target"), PropertyInfo(Variant::DICTIONARY,"env")) );
    BIND_VMETHOD( MethodInfo("_timeout_behavior", PropertyInfo(Variant::OBJECT,"target"), PropertyInfo(Variant::DICTIONARY,"env")) );
    BIND_VMETHOD( MethodInfo("_cancel_behavior", PropertyInfo(Variant::OBJECT,"target"), PropertyInfo(Variant::DICTIONARY,"env")) );
}


BehaviorNode::Status TimerBNode::_step(const Variant& target, Dictionary &env) {
    if (_time <= 0) {
	    if (!timeout) {
            timeout = true;
            _timeout_behavior(target, env);
            _script_timeout_behavior(target, env);
            return STATUS_FAILURE;//_cancel ? STATUS_CONTINUE : STATUS_FAILURE;
        }else {
            return BehaviorNode::_step(target, env);
        }
    }else {
        float timestep = get_fixed_process_delta_time();
        _time -= timestep;
        _during_behavior(target, env);
        _script_during_behavior(target, env);
        if (_cancel) {
            _time = 0;
            timeout = true;
            _cancel = false;
            _cancel_behavior(target, env);
            _script_cancel_behavior(target, env);
            return STATUS_FAILURE;
        }
        else {
            _traversal_children(target, env);
            return STATUS_RUNNING;
        }
    }
}

void TimerBNode::recount() {
    _time = delay;
    _cancel = false;
    timeout = false;
}

void TimerBNode::_reset(const Variant &target) {
    BehaviorNode::_reset(target);
    _time=0;
    timeout = true;
//    if (!timeout) {
//        timeout = true;
//        Dictionary dic;
//        _timeout_behavior(target, dic);
//        if (get_script_instance()) {
//            Variant v = dic;
//            const Variant* ptr[2]={&target, &v};
//            get_script_instance()->call_multilevel(StringName("_timeout_behavior"),ptr,2);
//        }
//    }
}

void TimerBNode::recount_to(float t) {
    _time = t;
    _cancel = false;
    timeout = false;
}

BehaviorNode::Status TimerBNode::_behavior(const Variant &target, Dictionary env) {
    _time = delay;
    _cancel = false;
    timeout = false;
    return STATUS_RUNNING;
}

void TimerBNode::_script_during_behavior(const Variant &target, Dictionary &env) {
    if (get_script_instance()) {
        Variant var_env = Variant(env);
        const Variant* ptr[2]={&target,&var_env};
        get_script_instance()->call_multilevel(StringName("_during_behavior"),ptr,2);
    }
}

void TimerBNode::_script_timeout_behavior(const Variant &target, Dictionary &env) {
    if (get_script_instance()) {
        Variant var_env = Variant(env);
        const Variant* ptr[2]={&target,&var_env};
        get_script_instance()->call_multilevel(StringName("_timeout_behavior"),ptr,2);
    }
}

void TimerBNode::_script_cancel_behavior(const Variant &target, Dictionary &env) {
    if (get_script_instance()) {
        Variant var_env = Variant(env);
        const Variant* ptr[2]={&target,&var_env};
        get_script_instance()->call_multilevel(StringName("_cancel_behavior"),ptr,2);
    }
}
