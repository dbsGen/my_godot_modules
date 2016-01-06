//
// Created by Gen on 2016/1/5.
//

#include "trail_2d.h"

void Trail2D::_update_frame(bool minus) {
    _update_position(minus);
    if (trail_enable && (span_frame == 0 || (++span_count > span_frame))) {
        span_count = 0;
        TrailPoint np = TrailPoint();
        np.count = trail_positions.limit();
        trail_positions.push(np);
    }
}

void Trail2D::_update_position(bool minus) {
    Vector2 new_pos = get_global_pos();
    Vector2 offset = (new_pos - old_position)/get_global_transform().get_scale();
    old_position = new_pos;

    for (int n = trail_positions.size()-1; n >= 0; n--) {
        if (trail_positions[n].count > 0) {
            trail_positions[n].position -= offset;
            if (minus)
                trail_positions[n].count -= 1;
        }else break;
    }
}

void Trail2D::_normal_points(int idx, int total, Vector2 &res1, Vector2 &res2) {
    if (total <= 1) {
        if (total == 1) {
            res1 = res2 = trail_positions[idx].position;
        }
        return;
    }
    int count = trail_positions[idx].count;
    if (idx == total - 1) {
        Vector2 pos = trail_positions[idx].position - trail_positions[idx - 1].position;
        Vector2 v1 = Vector2(-pos.y, pos.x).normalized();
        Vector2 v2 = v1*(line_width*count/(total - 1))/2;
        res1 = pos + v2;
        res2 = pos - v2;
    }else if (idx == 0) {
        res1 = res2 = trail_positions[idx].position;
    }else {
        Vector2 pos_o = trail_positions[idx].position, pos_b = trail_positions[idx-1].position, pos_f = trail_positions[idx+1].position;
        Vector2 v3 = pos_f - pos_o;
        Vector2 v1 = (pos_b - pos_o + v3).normalized();
        Vector2 v2 = v1*(line_width*count/(total - 1))/2;
        Vector2 p1 = pos_o + v2, p2 = pos_o - v2;

        if((p1.x - pos_o.x)*v3.y-(p1.y - pos_o.y)*v3.x > 0) {
            res1 = p1;
            res2 = p2;
        }else {
            res1 = p2;
            res2 = p1;
        }
    }
}

float determinant(Vector2 v1, Vector2 v2) {
    return v1.x * v2.y - v2.x * v1.y;
}

bool intersect(Point2 point_a1, Point2 point_a2, Point2 point_b1, Point2 point_b2, Point2 *result)
{
    double delta = determinant(point_a2-point_a1, point_b2-point_b1);
    if ( delta<=(1e-6) && delta>=-(1e-6) )
    {
        return false;
    }
    double namenda = determinant(point_b1-point_a1, point_b1-point_b2) / delta;
    if ( namenda>1 || namenda<0 )
    {
        return false;
    }
    double miu = determinant(point_a2-point_a1, point_b1-point_a1) / delta;
    if (miu>1 || miu<0) {
        return false;
    }

    if (result != NULL) {
        *result = (point_a2-point_a1)*namenda+point_a1;
    }
    return true;
}

void Trail2D::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            old_position = get_global_pos();
        } break;
        case NOTIFICATION_FIXED_PROCESS: {
            _update_frame(true);
        } break;
        case NOTIFICATION_PROCESS: {
            _update_frame();
            update();
        } break;
        case NOTIFICATION_DRAW: {
            if (line_width <= 10) {
                int total = trail_positions.size();
                for (int n = total-2; n >= 0; n--) {
                    if (trail_positions[n].count > 0) {
                        const Vector2& p1 = trail_positions[n].position, p2 = trail_positions[n+1].position;
                        if (p1 != p2) {
                            float p = trail_positions[n].count/(float)(total-1);
                            draw_line(p1, p2, line_color->get_color_at_offset(p), p*line_width);
                        }
                    }else break;
                }
            }else {
                int total = trail_positions.size();
                Vector2 last_points[2];
                Color last_color;
                for (int n = total-2; n >= 0; n--) {
                    if (trail_positions[n].count > 0) {
                        if (n == total-2) {
                            _normal_points(n+1, total, last_points[1], last_points[0]);
                            last_color = line_color->get_color_at_offset(1);
                        }
                        Vector2 ps[2];
                        _normal_points(n, total, ps[0], ps[1]);
                        Vector2 cross_point;
                        if ((trail_positions[n].position - trail_positions[n+1].position).length() < 1e-6) {
                        }else if (intersect(ps[0], ps[1], last_points[0], last_points[1], &cross_point)) {
//                            Color color = line_color->get_color_at_offset(n/(float)(total-1));
//                            Vector<Vector2> points;
//                            points.resize(3);
//                            points[0] = ps[0];
//                            points[1] = last_points[1];
//                            points[2] = cross_point;
//                            Vector<Color> colors;
//                            colors.resize(3);
//                            colors[0] = color;
//                            colors[1] = last_color;
//                            colors[2] = last_color;
//                            draw_polygon(points, colors);
//                            points[0] = ps[1];
//                            points[1] = last_points[0];
//                            points[2] = cross_point;
//                            colors[0] = color;
//                            colors[1] = last_color;
//                            colors[2] = last_color;
//                            last_color = color;
//                            last_points[0] = ps[1];
//                            last_points[1] = ps[0];
//                            draw_polygon(points, colors);
                        }else if (ps[0] == ps[1]) {
                            Color color = line_color->get_color_at_offset(n/(float)(total-1));
                            Vector<Vector2> points;
                            points.resize(3);
                            points[0] = ps[0];
                            points[1] = last_points[1];
                            points[2] = last_points[0];
                            Vector<Color> colors;
                            colors.resize(3);
                            colors[0] = color;
                            colors[1] = last_color;
                            colors[2] = last_color;
                            last_color = color;
                            draw_polygon(points, colors);
                            last_points[0] = ps[1];
                            last_points[1] = ps[0];
                        }else {
                            Vector<Vector2> points;
                            points.resize(4);
                            points[0] = ps[0];
                            points[1] = ps[1];
                            points[2] = last_points[0];
                            points[3] = last_points[1];
                            Color color = line_color->get_color_at_offset(n/(float)(total-1));
                            Vector<Color> colors;
                            colors.resize(4);
                            colors[0] = color;
                            colors[1] = color;
                            colors[2] = last_color;
                            colors[3] = last_color;
                            last_color = color;
                            last_points[0] = ps[1];
                            last_points[1] = ps[0];
                            draw_polygon(points, colors);
                        }
                    }else break;
                }
            }
        } break;
    }
}

void Trail2D::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("get_trail_enable"), &Trail2D::get_trail_enable);
    ObjectTypeDB::bind_method(_MD("set_trail_enable", "trail_enable"), &Trail2D::set_trail_enable);

    ObjectTypeDB::bind_method(_MD("get_trail_count"),&Trail2D::get_trail_count);
    ObjectTypeDB::bind_method(_MD("set_trail_count", "trail_count"),&Trail2D::set_trail_count);

    ObjectTypeDB::bind_method(_MD("get_span_frame"),&Trail2D::get_span_frame);
    ObjectTypeDB::bind_method(_MD("set_span_frame", "span_frame"),&Trail2D::set_span_frame);

    ObjectTypeDB::bind_method(_MD("get_line_width"),&Trail2D::get_line_width);
    ObjectTypeDB::bind_method(_MD("set_line_width", "line_width"),&Trail2D::set_line_width);

    ObjectTypeDB::bind_method(_MD("get_line_color"),&Trail2D::get_line_color);
    ObjectTypeDB::bind_method(_MD("set_line_color", "line_color:ColorRamp"),&Trail2D::set_line_color);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trail_enable"), _SCS("set_trail_enable"), _SCS("get_trail_enable"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "trail_count"), _SCS("set_trail_count"), _SCS("get_trail_count"));
    ADD_PROPERTY(PropertyInfo(Variant::INT, "span_frame"), _SCS("set_span_frame"), _SCS("get_span_frame"));
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "line_width"), _SCS("set_line_width"), _SCS("get_line_width"));
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "line_color",PROPERTY_HINT_RESOURCE_TYPE,"ColorRamp"), _SCS("set_line_color"), _SCS("get_line_color"));
}