//
// Created by Gen on 2016/1/17.
//

#include "barrage.h"

int Barrage::create_bullet(Point2 position, float rotation, int frame, float size, int count) {
    int index = bullets.size();
    for (int i = 0; i < count; ++i) {
        Bullet bullet;
        bullet.matrix = Matrix32(rotation, position);
        bullet.frame = frame;
        bullet.matrix.scale(Vector2(size/8,size/8));
        bullets.push_back(bullet);
    }
    return index;
}

void Barrage::_notification(int p_notification) {
    switch (p_notification) {
        case NOTIFICATION_PROCESS: {

        } break;
        case NOTIFICATION_FIXED_PROCESS: {

        } break;
    }
}

Barrage::Barrage() {
    h_frames = 1;
    v_frames = 1;
    set_fixed_process(true);
    set_process(true);
}