#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ReachabilityInterface.h"
#include "../Manager.h"
#include <vector>

namespace ClassProject {

   class Reachability : public ReachabilityInterface {

    private: 
    
    std::vector<BDD_ID> state_variables;
    std::vector<BDD_ID> next_state_variables;
    std::vector<BDD_ID> input_variables;
    std::vector<BDD_ID> transition_functions;
    std::vector<BDD_ID> initial_state;

    /**
     * @brief computes the existential quantification of an equation, given by its BDD
     * with respect to a set variables
     * 
     * @param formula 
     * @param variables 
     * @return BDD_ID 
     */
    BDD_ID existential_quantification(BDD_ID equation, std::vector<BDD_ID> variables);

    /**
     * @brief computes the characteristic function of a set of equations in terms of 
     *  a set of variables
     * 
     * @param equation 
     * @param variables 
     * @return BDD_ID 
     */
    BDD_ID characteristic_function(std::vector<BDD_ID> equations, std::vector<BDD_ID> variables);

    /**
     * @brief computes the characteristic function of the image (next state set) starting from the characteristic functions 
     * of both the state state (current state set) and the transition relation
     * 
     * it also renames the next state variables into current state variables
     * 
     * @param transition_relation 
     * @param initial_state 
     * @return BDD_ID 
     */
    BDD_ID compute_image(BDD_ID transition_relation, BDD_ID starting_state);

    public:

    explicit Reachability(unsigned int stateSize, unsigned int inputSize = 0);
    ~Reachability(){}
    const std::vector<BDD_ID> &getStates() const;
    const std::vector<BDD_ID> &getInputs() const;
    bool isReachable(const std::vector<bool> &stateVector);
    int stateDistance(const std::vector<bool> &stateVector);
    void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions);        
    void setInitState(const std::vector<bool> &stateVector);

   };
}

#endif
