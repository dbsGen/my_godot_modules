//
// Created by gen on 15-5-20.
//

#ifndef GODOT_MASTER_BUFF_H
#define GODOT_MASTER_BUFF_H

#include "../../core/reference.h"
#include "../../core/ustring.h"

class Character;

class Buff : public Reference {
    GDCLASS(Buff, Reference);
private:
    int time_count;
    int time_skip;
    float life_time;

    String unique;
    String buffer_name;

protected:
    static void _bind_methods();
    virtual void _step(Object *character, Dictionary env) {}
public:
    _FORCE_INLINE_ void set_skip(int skip){time_skip = skip;}
    _FORCE_INLINE_ int get_skip(){return time_skip;}

    _FORCE_INLINE_ void set_life_time(float lt) {life_time=lt;}
    _FORCE_INLINE_ float get_life_time() {return life_time;}

    _FORCE_INLINE_ String get_unique() {return unique;}
    _FORCE_INLINE_ void set_unique(String p_unique) {unique = p_unique;}

    _FORCE_INLINE_ String get_buff_name() {return buffer_name;}
    _FORCE_INLINE_ void set_buff_name(String p_buffer_name) {buffer_name = p_buffer_name;}

    void step(Object *character, Dictionary env);

    Buff() {time_skip=0;life_time=0;}
};


#endif //GODOT_MASTER_BUFF_H
