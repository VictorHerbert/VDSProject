#include "Reachability.h"
#include "../Manager.h"

namespace ClassProject {

    Reachability::Reachability(unsigned int stateSize, unsigned int inputSize)    
    : ReachabilityInterface(stateSize, inputSize),
      state_variables(stateSize, 0),
      next_state_variables(stateSize, 0),
      input_variables(inputSize, 0),
      transition_functions(stateSize, 0),
      initial_state(stateSize, false)
    {

    }

    Reachability::~Reachability()
    {

    }
    
    const std::vector<BDD_ID> &Reachability::getStates() const
    {

    }

    const std::vector<BDD_ID> &Reachability::getInputs() const 
    {

    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector)
    {

    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector)
    {

    }

    void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions)
    {

    }

    void Reachability::setInitState(const std::vector<bool> &stateVector)
    {

    }

}
