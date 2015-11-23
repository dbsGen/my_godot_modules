#ifndef TIME_B_NODE_H
#define TIME_B_NODE_H

#include "behaviornode.h"

class TimerBNode : public BehaviorNode {
OBJ_TYPE(TimerBNode, BehaviorNode);
private:
    float delay;
    bool cancel;
protected:
    bool timeout;
    float _time;

    virtual Status  _step(const Variant& target, Dictionary &env);
    virtual Status _behavior(const Variant& target, Dictionary env);

    void _script_timeout_behavior(const Variant& target, Dictionary& env);
    virtual void _timeout_behavior(const Variant& target, Dictionary& env) {}

    void _script_during_behavior(const Variant& target, Dictionary& env);
    virtual void _during_behavior(const Variant& target, Dictionary& env) {}
    virtual void _reset(const Variant& target);

    static void _bind_methods();
public:
    void set_delay(float t) {delay = t;}
    float get_delay() {return delay;}

    float get_time() {return _time;}
    void time_out() {cancel = true;_time=0;}

    void recount();
    void recount_to(float t);

    TimerBNode() : BehaviorNode() {
        delay=1;
        _time=0;
        timeout = true;
        set_will_focus(true);
    }
    ~TimerBNode() {}
};

#endif
