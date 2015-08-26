#ifndef INPUT_STORAGE_B_NODE_H
#define INPUT_STORAGE_B_NODE_H

#include "../../core/reference.h"
#include "../../scene/main/node.h"

class InputNode : public Reference {
private:
    Array _pressed_actions;
    Array _down_actions;
public:
    void pressed_action(const String& action);
    void down_action(const String& action);

    bool is_pressed(const String& action);
    bool is_pressed(const StringArray& actions);
    bool is_down(const String& action);
    bool is_down(const StringArray& actions);
    int  queue_down(const StringArray& actions, int offset);
};

class InputStorageNode;
class InputStorage : public Reference {
OBJ_TYPE(InputStorage, Reference);
private:
    DVector< Ref<InputNode> > storage_events;
    Ref<InputNode> _this_frame;
    int     storage_size;
    friend class    InputStorageNode;
    bool    _pressed_in_frame(const Variant& events, int frame);
    bool    _down_in_frame(const Variant& events, int frame);
    void    _add_node(Object *node);
    InputStorageNode    *storageNode;
    static InputStorage *singleton;
protected:
    static void     _bind_methods();

public:
    static InputStorage *get_singleton();

    void    start(StringArray events);
    void    close();

    void    frame_begin();
    void    pressed_event(const String& event);
    void    down_event(const String& event);

    int     get_storage_size() {return storage_size;}

    bool    pressed_at(const Variant &event, int at_frame = 0);
    int     down_frame(const Variant &event, int in_frame = 1);
    bool    is_pressed(const Variant &event, int in_frame = 1);
    bool    is_down(const Variant &event, int in_frame = 1);
    bool    test_down(const StringArray &events, int in_frame = 10);

    InputStorage() {storage_size = 30;storageNode=NULL;}
    ~InputStorage() {}
};

class InputStorageNode : public Node {
    OBJ_TYPE(InputStorageNode, Node);
private:
    List<String>    pressed;
    StringArray     events;
    void            _update_events();
protected:
    virtual void    _input(const InputEvent& p_event);

    void _notification(int p_what);
    static void     _bind_methods();
public:
    Ref<InputStorage>   _storage;
    StringArray     get_events() const {return events;}
    void            set_events(StringArray e) {events = e;}

    int             get_storage_size() {return _storage->storage_size;}
    void            set_storage_size(int size) {_storage->storage_size=size;}

    InputStorageNode() {
        set_process_input(true);
        set_fixed_process(true);
        set_pause_mode(Node::PAUSE_MODE_PROCESS);
    }
    ~InputStorageNode() {}
};

#endif