#ifndef PROBABILITY_B_NODE_H
#define PROBABILITY_B_NODE_H

#include "behaviornode.h"

class ProbabilityBNode : public BehaviorNode {
OBJ_TYPE(ProbabilityBNode, BehaviorNode);
private:
    float probability;
protected:
    virtual bool    _pre_behavior(const Variant& target, Dictionary& env);

    static void     _bind_methods();
public:
    void set_probability(float p) {probability = p;}
    float get_probability() {return probability;}

    ProbabilityBNode() {probability=0.5;}
    ~ProbabilityBNode() {}
};

#endif