//
// Created by Gen on 2016/1/5.
//

#ifndef GODOT_MASTER_TRAIL_2D_H
#define GODOT_MASTER_TRAIL_2D_H

#include "../../scene/2d/node_2d.h"
#include "../../core/typedefs.h"
#include "../../core/math/math_2d.h"
#include "../../core/object.h"
#include "../../scene/resources/color_ramp.h"
#include "queue.hpp"

using namespace MyTools;

class Trail2D : public Node2D {
    OBJ_TYPE(Trail2D, Node2D);
private:
    struct TrailPoint {
        Vector2 position;
        int count;

        _FORCE_INLINE_ TrailPoint() {
            count = 0;
        };
    };

    bool trail_enable;
    int span_frame;
    int span_count;
    float line_width;
    Ref<ColorRamp> line_color;

    Vector2 old_position;

    Queue<TrailPoint> trail_positions;

    void _normal_points(int idx, int total, Vector2 &res1, Vector2 &res2);
    void _update_position(bool minus = false);
    void _update_frame(bool minus = false);

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    _FORCE_INLINE_ bool get_trail_enable() {return trail_enable;}
    _FORCE_INLINE_ void set_trail_enable(bool p_enable) {trail_enable=p_enable;span_count=span_frame;}

    _FORCE_INLINE_ int get_span_frame() {return span_frame;}
    _FORCE_INLINE_ void set_span_frame(int p_frame) {span_frame=p_frame;}

    _FORCE_INLINE_ float get_trail_count() {return trail_positions.limit();}
    _FORCE_INLINE_ void set_trail_count(float p_count) {trail_positions.alloc(p_count);}

    _FORCE_INLINE_ float get_line_width() {return line_width;}
    _FORCE_INLINE_ void set_line_width(float p_width) {line_width=p_width;}

    _FORCE_INLINE_ Ref<ColorRamp> get_line_color() {return line_color;}
    _FORCE_INLINE_ void set_line_color(const Ref<ColorRamp>& p_color) {line_color=p_color;}

    _FORCE_INLINE_ Trail2D() {
        trail_enable = false;
        span_frame = 0;
        set_fixed_process(true);
        set_process(true);
        trail_positions.alloc(30);
    }
};


#endif //GODOT_MASTER_TRAIL_2D_H
