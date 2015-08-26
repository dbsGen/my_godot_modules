//
// Created by gen on 15-4-26.
//

#ifndef GODOT_MASTER_NEW_TIMER_H
#define GODOT_MASTER_NEW_TIMER_H

#include "../../core/reference.h"
#include "../../scene/main/node.h"

class TimerObject : public Reference {
OBJ_TYPE( TimerObject, Reference );
private:

    bool is_cancel;
protected:

    static void _bind_methods();

public:
    float time;
    Object *target;
    String method;

    bool step(float delta);
    void cancel();

    TimerObject() {is_cancel = false;target=NULL;}
};

class TimerNode : public Node {
OBJ_TYPE( TimerNode, Node );
public:
    Vector< Ref<TimerObject> > timer_objs;
    void check_queue();
protected:
    void _notification(int p_what);
};

class NewTimer: public Reference {
OBJ_TYPE(NewTimer, Reference);
private:
    TimerNode   *timerNode;
protected:
    static void _bind_methods();
    static NewTimer *singleton;
public:
    static NewTimer* get_singleton();

    Ref<TimerObject> wait(float p_time);
    Ref<TimerObject> wait_trigger(float p_time, Object* p_target, String p_method);
    void _add_node(Object *node);

    NewTimer() {timerNode=NULL;}
};


#endif //GODOT_MASTER_NEW_TIMER_H
