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

    public:

    explicit Reachability(unsigned int stateSize, unsigned int inputSize = 0);
    ~Reachability();
    const std::vector<BDD_ID> &getStates() const;
    const std::vector<BDD_ID> &getInputs() const;
    bool isReachable(const std::vector<bool> &stateVector);
    int stateDistance(const std::vector<bool> &stateVector);
    void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions);        
    void setInitState(const std::vector<bool> &stateVector);

   };
}

#endif
