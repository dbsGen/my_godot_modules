//
// Created by gen on 15-4-26.
//

#include "new_timer.h"
#include "../../core/bind/core_bind.h"
#include "../../scene/main/scene_main_loop.h"
#include "../../core/os/main_loop.h"
#include "../../scene/main/viewport.h"
#include "../../core/os/os.h"


bool TimerObject::step(float delta) {
    if (is_cancel)
        return true;
    time -= delta;
    if (time < 0) {
        if (target)
            target->call(method);
        emit_signal("timeout");
        is_cancel = true;
        return false;
    }else {
        return false;
    }
}

void TimerObject::cancel() {
    is_cancel = true;
}

void TimerObject::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("cancel"),&TimerObject::cancel);
    ADD_SIGNAL( MethodInfo("timeout") );
}



void TimerNode::check_queue() {
    if (timer_objs.size() > 0 && !is_processing()) {
        set_process(true);
        set_pause_mode(Node::PAUSE_MODE_PROCESS);
    }
}

void TimerNode::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_PROCESS: {
            float delta = get_process_delta_time();
            for (int n = 0, t = timer_objs.size(); n < t; n++) {
                Ref<TimerObject> obj = timer_objs[n];
                if (obj->step(delta)) {
                    timer_objs.remove(n);
                    n -= 1;
                    t -= 1;
                }
            }
            if (timer_objs.size() == 0) {
                set_process(false);
            }
        };
    }
}

Ref<TimerObject> NewTimer::wait(float p_time) {
    const String timer_key = "new_timer";
    MainLoop *main_loop = OS::get_singleton()->get_main_loop();
    SceneTree *tree = main_loop->cast_to<SceneTree>();
    ERR_FAIL_COND_V(tree == NULL, NULL);

    Viewport *viewport = tree->get_root();
    ERR_FAIL_COND_V(viewport == NULL, NULL);
    if (timerNode==NULL) {
        timerNode = memnew(TimerNode);
        timerNode->set_name(timer_key);

        Vector<Variant> vector;
        vector.push_back(Variant(timerNode));

        tree->connect(StringName("idle_frame"), this, StringName("_add_node"), vector, 0);

    }

    Ref<TimerObject> obj = memnew(TimerObject);
    obj->time = p_time;
    timerNode->timer_objs.push_back(obj);
    timerNode->check_queue();
    return obj;
}

Ref<TimerObject> NewTimer::wait_trigger(float p_time, Object *p_target, String p_method) {
    const String timer_key = "new_timer";
    MainLoop *main_loop = OS::get_singleton()->get_main_loop();
    SceneTree *tree = main_loop->cast_to<SceneTree>();
    ERR_FAIL_COND_V(tree == NULL, NULL);

    Viewport *viewport = tree->get_root();
    ERR_FAIL_COND_V(viewport == NULL, NULL);
    TimerNode *timerNode;
    if (timerNode == NULL) {
        timerNode = memnew(TimerNode);
        timerNode->set_name(timer_key);

        Vector<Variant> vector;
        vector.push_back(Variant(timerNode));

        tree->connect("idle_frame", this, "_add_node", vector, 0);

    }else {
        timerNode = viewport->get_node(timer_key)->cast_to<TimerNode>();
    }

    Ref<TimerObject> obj = memnew(TimerObject);
    obj->time = p_time;
    obj->target = p_target;
    obj->method = p_method;
    timerNode->timer_objs.push_back(obj);
    timerNode->check_queue();
    return obj;
}

NewTimer *NewTimer::singleton = NULL;
NewTimer *NewTimer::get_singleton() {
    if (!singleton) {
        singleton = memnew(NewTimer);
    }
    return singleton;
}

void NewTimer::_add_node(Object *node) {
    MainLoop *main_loop = OS::get_singleton()->get_main_loop();
    SceneTree *tree = main_loop->cast_to<SceneTree>();
    ERR_FAIL_COND(tree == NULL);

    tree->disconnect("idle_frame", this, "_add_node");
    tree->get_root()->add_child(node->cast_to<TimerNode>());
}

void NewTimer::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("wait_trigger:TimerObject", "time", "target", "method"),&NewTimer::wait_trigger);
    ObjectTypeDB::bind_method(_MD("wait:TimerObject", "time"),&NewTimer::wait);
    ObjectTypeDB::bind_method(_MD("_add_node", "node"),&NewTimer::_add_node);
}