#include "probabilitybnode.h"
#include "../../core/math/math_funcs.h"

bool ProbabilityBNode::_pre_behavior(const Variant &target, Dictionary &env) {
    return Math::random(0, 1) < probability;
}

void ProbabilityBNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_probability", "probability"), &ProbabilityBNode::set_probability);
    ClassDB::bind_method(D_METHOD("get_probability"), &ProbabilityBNode::get_probability);

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "probability/probability", PROPERTY_HINT_RANGE, "0,1,0.01" ), "set_probability","get_probability");
}