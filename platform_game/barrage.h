//
// Created by Gen on 2016/1/17.
//

#ifndef GODOT_NEW_BARRAGE_H
#define GODOT_NEW_BARRAGE_H

#include "../../scene/2d/node_2d.h"
#include "../../scene/resources/texture.h"
#include "../../core/reference.h"
#include "../../core/math/math_2d.h"

class Barrage : public Node2D {
    OBJ_TYPE(Barrage, Node2D);
protected:
    struct Bullet {
        int index;
        int frame;
        Vector2 speed;
        Matrix32 matrix;
        bool dead;

        _FORCE_INLINE_ Bullet(){index=0;dead = false;}
    };

private:
    float bullet_size;
    Ref<Texture> bullet_sprite;
    Vector<Bullet> bullets;
    int h_frames;
    int v_frames;

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    int create_bullet(Point2 position, float rotation, int frame, float size = 8, int count = 1);

    Barrage();
};

#endif //GODOT_NEW_BARRAGE_H
