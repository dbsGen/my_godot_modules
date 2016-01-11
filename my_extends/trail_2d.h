//
// Created by Gen on 2016/1/5.
//

#ifndef GODOT_MASTER_TRAIL_2D_H
#define GODOT_MASTER_TRAIL_2D_H

#include "../../scene/2d/node_2d.h"
#include "../../scene/2d/position_2d.h"
#include "../../core/typedefs.h"
#include "../../core/math/math_2d.h"
#include "../../core/object.h"
#include "../../scene/2d/canvas_item.h"
#include "../../scene/resources/color_ramp.h"
#include "queue.hpp"

using namespace MyTools;

class TrailPoint2D : public Node2D {
    OBJ_TYPE(TrailPoint2D, Node2D);
private:
    struct TrailItem {
        Vector2 position;
        int count;

        _FORCE_INLINE_ TrailItem() {
            count = 0;
        };
    };

    bool trail_enable;
    int span_frame;
    int span_count;
    float line_width;
    Ref<ColorRamp> line_color;

    Vector2 old_position;

    Queue<TrailItem> trail_items;

    void _normal_points(int idx, int total, Vector2 &res1, Vector2 &res2);
    void _update_position(bool minus = false);
    void _update_frame(bool minus = false);

    NodePath target_path;
    CanvasItem *trail_target;
    void _update_trail_target();
    void _on_exit_tree();

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    virtual Rect2 get_item_rect() const;

    _FORCE_INLINE_ bool get_trail_enable() {return trail_enable;}
    _FORCE_INLINE_ void set_trail_enable(bool p_enable) {trail_enable=p_enable;span_count=span_frame;}

    _FORCE_INLINE_ int get_span_frame() {return span_frame;}
    _FORCE_INLINE_ void set_span_frame(int p_frame) {span_frame=p_frame;}

    _FORCE_INLINE_ float get_trail_count() {return trail_items.limit();}
    _FORCE_INLINE_ void set_trail_count(float p_count) {trail_items.alloc(p_count);}

    _FORCE_INLINE_ float get_line_width() {return line_width;}
    _FORCE_INLINE_ void set_line_width(float p_width) {line_width=p_width<0?0:(p_width>10?10:p_width);}

    _FORCE_INLINE_ Ref<ColorRamp> get_line_color() {return line_color;}
    _FORCE_INLINE_ void set_line_color(const Ref<ColorRamp>& p_color) {line_color=p_color;}

    _FORCE_INLINE_ NodePath get_target_path() {return target_path;}
    _FORCE_INLINE_ void set_target_path(const NodePath &p_path) {target_path=p_path;_update_trail_target();}

    _FORCE_INLINE_ TrailPoint2D() {
        trail_enable = false;
        span_frame = 0;
        line_width = 1;
        set_fixed_process(true);
        set_process(true);
        trail_items.alloc(30);
        trail_target = NULL;
    }

    _FORCE_INLINE_ ~TrailPoint2D() {
        if (trail_target != NULL) {
            trail_target->disconnect("exit_tree", this, "_on_exit_tree");
        }
    }
};

class TrailLine2D : public Node2D {
    OBJ_TYPE(TrailLine2D, Node2D);
private:
    struct TrailItem {
        Vector2 position1, position2;
        int count;

        _FORCE_INLINE_ void offset(Vector2 p_off) {
            position1 += p_off;
            position2 += p_off;
        }

        _FORCE_INLINE_ TrailItem() {
            count = 0;
        };
    };

    Vector2 old_position;

    bool trail_enable;
    int span_frame;
    int span_count;
    Ref<ColorRamp> line_color;

    Queue<TrailItem> trail_items;

    Position2D *terminal;
    Point2 terminal_position;

    void _update_position(bool minus = false);
    void _update_frame(bool minus = false);

protected:
    void _notification(int p_what);
    static void _bind_methods();

public:
    virtual Rect2 get_item_rect() const;

    _FORCE_INLINE_ Vector2 get_terminal() {
        if (terminal)
            return terminal->get_pos();
        else
            return terminal_position;
    }
    _FORCE_INLINE_ void set_terminal(const Vector2 &p_terminal) {
        if (terminal)
            terminal->set_pos(p_terminal);
        else
            terminal_position = p_terminal;
    }

    _FORCE_INLINE_ bool get_trail_enable() {return trail_enable;}
    _FORCE_INLINE_ void set_trail_enable(bool p_enable) {trail_enable=p_enable;span_count=span_frame;}

    _FORCE_INLINE_ int get_span_frame() {return span_frame;}
    _FORCE_INLINE_ void set_span_frame(int p_frame) {span_frame=p_frame;}

    _FORCE_INLINE_ float get_trail_count() {return trail_items.limit();}
    _FORCE_INLINE_ void set_trail_count(float p_count) {trail_items.alloc(p_count);}

    _FORCE_INLINE_ Ref<ColorRamp> get_line_color() {return line_color;}
    _FORCE_INLINE_ void set_line_color(const Ref<ColorRamp>& p_color) {line_color=p_color;}

    _FORCE_INLINE_ TrailLine2D (){
        trail_enable = false;
        span_frame = 0;
        set_fixed_process(true);
        set_process(true);
        trail_items.alloc(30);
        terminal_position = Vector2(10,10);
        terminal = NULL;
    }
};

#endif //GODOT_MASTER_TRAIL_2D_H
