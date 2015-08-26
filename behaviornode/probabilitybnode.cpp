#include "probabilitybnode.h"
#include "../../core/math/math_funcs.h"

bool ProbabilityBNode::_pre_behavior(const Variant &target, Dictionary &env) {
    return Math::random(0, 1) < probability;
}

void ProbabilityBNode::_bind_methods() {
    ObjectTypeDB::bind_method(_MD("set_probability", "probability"), &ProbabilityBNode::set_probability);
    ObjectTypeDB::bind_method(_MD("get_probability"), &ProbabilityBNode::get_probability);

    ADD_PROPERTY( PropertyInfo( Variant::REAL, "probability/probability", PROPERTY_HINT_RANGE, "0,1,0.01" ), _SCS("set_probability"),_SCS("get_probability" ) );
}