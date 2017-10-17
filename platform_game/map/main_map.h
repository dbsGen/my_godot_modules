//
// Created by gen on 15-7-18.
//

#ifndef GODOT_MASTER_MAIN_MAP_H
#define GODOT_MASTER_MAIN_MAP_H

#include "../../../core/resource.h"
#include "../../../scene/resources/texture.h"

class MainMap : public Resource {
    GDCLASS(MainMap, Resource);
private:
    int width;
    int height;
    int x;
    int y;
    bool *map_checked;

    Ref<Texture> texture;

    void update_size();

protected:
    static void _bind_methods();
public:
    _FORCE_INLINE_ int get_width() {return width;}
    _FORCE_INLINE_ void set_width(int p_width) {width=p_width;update_size();}

    _FORCE_INLINE_ int get_height() {return height;}
    _FORCE_INLINE_ void set_height(int p_height) {height=p_height;update_size();}

    _FORCE_INLINE_ void set_texture(const Ref<Texture>& p_texture) {texture = p_texture;}
    _FORCE_INLINE_ Ref<Texture> get_texture() {return texture;}

    bool checked_at(int x, int y);
    void set_checked(int x, int y, bool checked);
    void at(int x, int y);
    _FORCE_INLINE_ int get_x() {return x;}
    _FORCE_INLINE_ int get_y() {return y;}

    Dictionary get_data();
    void set_data(Dictionary data);

    MainMap() {
        width=0;
        height=0;
        map_checked=NULL;
    }
};


#endif //GODOT_MASTER_MAIN_MAP_H
