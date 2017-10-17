
#ifndef BEHAVIOR_NODE_H
#define BEHAVIOR_NODE_H

#include "../../core/ustring.h"
#include "../../scene/main/node.h"

class BehaviorNode : public Node {
    GDCLASS(BehaviorNode, Node);
public:
    enum BNodeType {
        TYPE_SEQUENCE,
        TYPE_CONDITION
    };
    enum Status {
        STATUS_DEPEND_ON_CHILDREN,
        STATUS_RUNNING,
        STATUS_FAILURE,
        STATUS_CONTINUE
    };
private:
    NodePath        _focus_node_path;
    BNodeType       _behavior_node_type;
    bool            _behavior_enable;
    bool            _will_focus;
protected:
    virtual Status  _traversal_children(const Variant& target, Dictionary& env);

    void            _script_reset(const Variant& target);

    virtual bool    _pre_behavior(const Variant& target, Dictionary env) {return true;}
    virtual Status  _behavior(const Variant& target, Dictionary env) {return STATUS_DEPEND_ON_CHILDREN;}
    virtual Status  _step(const Variant& target, Dictionary &env);
    virtual void    _reset(const Variant& target);
    virtual void    _on_notify(const Variant& from, const StringName &key, const Variant& value){}
    static void     _bind_methods();


public:
    void set_focus();
    void clear_focus();

    void send_notify(const Variant& from, const StringName &key, const Variant& value);

    _FORCE_INLINE_ bool get_behavior_enable() {return _behavior_enable;}
    _FORCE_INLINE_ void set_behavior_enable(bool e) {_behavior_enable = e;}

    int get_behavior_node_type(){return _behavior_node_type;}
    void set_behavior_node_type(int behavior_node_type){_behavior_node_type = (BNodeType)behavior_node_type;}

    _FORCE_INLINE_ bool get_will_focus() {return _will_focus;}
    _FORCE_INLINE_ void set_will_focus(bool focus) {_will_focus = focus;}

    Status step(const Variant& target, Dictionary env);
    void reset(const Variant& target);

    BehaviorNode() {
        _behavior_enable=true;
        _will_focus = false;
        _behavior_node_type=TYPE_SEQUENCE;
    }
    ~BehaviorNode(){}
};

VARIANT_ENUM_CAST(BehaviorNode::BNodeType);
VARIANT_ENUM_CAST(BehaviorNode::Status);

#endif