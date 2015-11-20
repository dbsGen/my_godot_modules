#ifndef INPUT_STORAGE_B_NODE_H
#define INPUT_STORAGE_B_NODE_H

#include "../../core/reference.h"
#include "../../scene/main/node.h"

class InputNode : public Object {
private:
    Array _pressed_actions;
    Array _down_actions;
public:
    void pressed_action(const String& action);
    void down_action(const String& action);

    bool is_pressed(const String& action) const;
    bool is_pressed(const StringArray& actions) const;
    bool is_down(const String& action) const;
    bool is_down(const StringArray& actions) const;
    int  queue_down(const StringArray& actions, int offset) const;
};

class InputStorageNode;
class InputStorage : public Reference {
OBJ_TYPE(InputStorage, Reference);
private:
    template <class T>
    class Queue {
        int _limit;
        int _size;
        int _offset;
        T *mem;

    public:

        void alloc(int p_size) {
            if (mem) {
                mem = (T*)memrealloc(mem, p_size* sizeof(T));
                _size = 0;
                _offset = 0;
            }else {
                mem = (T*)memalloc(p_size* sizeof(T));
            }
            _limit = p_size;
            _size = MIN(_limit, _size);
        }

        const T &get(int index) const {
            return operator[](index);
        }

        T &get(int index) {
            return operator[](index);
        }

        void set(int index, T value) {
            if (index < _size) {
                operator[](index);
            }
        }

        _FORCE_INLINE_ T &operator [](int p_index) {
            if (p_index >= _size){
                T& aux=*((T*)0);
                ERR_FAIL_COND_V(p_index >= _size, aux);
                return aux;
            }
            else
                return mem[(_offset+p_index)%_limit];
        }

        int size() const {
            return _size;
        }

        T* push(const T &p_value) {
            int off = (_offset+_size)%_limit;
            mem[off] = p_value;
            _size ++;
            if (_size > _limit) {
                int m = _size-_limit;
                _size = _limit;
                _offset = (_offset+m)%_limit;
            }
            return &mem[off];
        }

        T pop() {
            if (_size > 0) {
                T res = mem[_offset++];
                _size--;
                return res;
            }else {
                return T();
            }
        }

        Queue() {
            mem = NULL;
            _limit = 0;
            _size = 0;
            _offset = 0;
        }
        ~Queue() {
            if (mem) {
                memfree(mem);
            }
        }
    };

    Queue< InputNode > storage_events;
    InputNode *_this_frame;
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
    void    set_storage_size(int p_storage_size) {storage_size=p_storage_size;storage_events.alloc(storage_size);}

    bool    pressed_at(const Variant &event, int at_frame = 0);
    int     down_frame(const Variant &event, int in_frame = 1);
    bool    is_pressed(const Variant &event, int in_frame = 1);
    bool    is_down(const Variant &event, int in_frame = 1);
    bool    test_down(const StringArray &events, int in_frame = 10);

    InputStorage() {
        storage_size = 30;
        storageNode=NULL;
        _this_frame = NULL;
        storage_events.alloc(storage_size);
    }
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