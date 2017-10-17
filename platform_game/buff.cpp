//
// Created by gen on 15-5-20.
//

#include "buff.h"
#include "../../core/script_language.h"

void Buff::step(Object *character, Dictionary env) {
    life_time -= (float)env["timestep"];
    if (life_time < 0)
        return;
    time_count++;
    if (time_count > time_skip) {
        _step(character, env);
        if (get_script_instance()) {
            Variant v1 = Variant(character);
            Variant var_env = Variant(env);
            const Variant* ptr[2]={&v1,&var_env};
            get_script_instance()->call_multilevel(StringName("_step"),ptr,2);
        }
        time_count = 0;
    }
}

void Buff::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_skip", "skip"), &Buff::set_skip);
    ClassDB::bind_method(D_METHOD("get_skip"), &Buff::get_skip);
    ClassDB::bind_method(D_METHOD("set_life_time", "life_time"), &Buff::set_life_time);
    ClassDB::bind_method(D_METHOD("get_life_time"), &Buff::get_life_time);
    ClassDB::bind_method(D_METHOD("set_unique", "unique"), &Buff::set_unique);
    ClassDB::bind_method(D_METHOD("get_unique"), &Buff::get_unique);
    ClassDB::bind_method(D_METHOD("set_buff_name", "buff_name"), &Buff::set_buff_name);
    ClassDB::bind_method(D_METHOD("get_buff_name"), &Buff::get_buff_name);

    ClassDB::bind_method(D_METHOD("step", "character", "env"), &Buff::step);

    BIND_VMETHOD( MethodInfo("_step", PropertyInfo(Variant::OBJECT,"target"), PropertyInfo(Variant::DICTIONARY,"env")) );
}