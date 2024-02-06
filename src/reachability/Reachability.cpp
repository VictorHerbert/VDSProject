#include "Reachability.h"
#include "../Manager.h"

namespace ClassProject {

    Reachability::Reachability(unsigned int stateSize, unsigned int inputSize)    
        : ReachabilityInterface(stateSize, inputSize),                
        transition_functions(stateSize),    // initialized by user (setTransitionFunctions)
        initial_state(stateSize)            // initialized by user (setInitState)
    {
        // create current state, next state, and input variables
        state_variables = std::vector<BDD_ID>(stateSize);
        for(int i=0; i<stateSize; i++){
            state_variables[i] = createVar("-");
        }

        next_state_variables = std::vector<BDD_ID>(stateSize);
        for(int i=0; i<stateSize; i++){
            next_state_variables[i] = createVar("-");
        }

        input_variables = std::vector<BDD_ID>(inputSize);
        for(int i=0; i<inputSize; i++){
            input_variables[i] = createVar("-");
        }
    }

    const std::vector<BDD_ID> &Reachability::getStates() const
    {
        return state_variables;
    }

    const std::vector<BDD_ID> &Reachability::getInputs() const 
    {
        return input_variables;
    }

    bool Reachability::isReachable(const std::vector<bool> &stateVector)
    {

        BDD_ID tau, cs0, cr, cri, img;

        // compute the BDD for the transition relation
        tau = this->xnor2(next_state_variables[0], transition_functions[0]);
        for(int i=1; i<state_variables.size(); i++){
            tau = this->and2(
                tau,
                this->xnor2(next_state_variables[i], transition_functions[i])
            );
        }
        
        // compute the characteristic function of the initial state
        cs0 = this->xnor2(state_variables[0], initial_state[0]);
        for(int i=1; i<state_variables.size(); i++){
            cs0 = this->and2(
                cs0,
                this->xnor2(state_variables[i], initial_state[i])
            );
        }

        // fixed point iteration
        cri = cs0;
        do {

            cr = cri;

            // compute image of next state variables (step 7)
            img = this->and2(tau, cr);

            // existential quantification with respect to current state variables
            for(int i=0; i<state_variables.size(); i++){
                img = this->or2(this->coFactorTrue(img, state_variables[i]), this->coFactorFalse(img, state_variables[i]));
            }

            // existential quantification with respect to the input variables 
            for(int i=0; i<input_variables.size(); i++){
                img = this->or2(this->coFactorTrue(img, input_variables[i]), this->coFactorFalse(img, input_variables[i]));
            }

            // rename next state variables into current state variables (step 8)
            for(int i=0; i<state_variables.size(); i++){
                img = this->and2(img, this->xnor2(this->state_variables[i], this->next_state_variables[i]));
            }

            // existential quantification with respect to the next state variables
            for(int i=0; i<next_state_variables.size(); i++){
                img = this->or2(this->coFactorTrue(img, next_state_variables[i]), this->coFactorFalse(img, next_state_variables[i]));
            }            

            cri = this->or2(cr, img);

        } while(cri != cr);

        // check if the stateVector belong to the reachable state space
        for(int i=0; i<stateVector.size(); i++){
            BDD_ID sv = stateVector[i] ? True() : False();
            cri = this->and2(cri, this->xnor2(sv, state_variables[i]));
        }

        // existential quantification with respect to the next state variables
        for(int i=0; i<state_variables.size(); i++){
            cri = this->or2(this->coFactorTrue(cri, state_variables[i]), this->coFactorFalse(cri, state_variables[i]));
        }      

        return cri == True();      
    }

    int Reachability::stateDistance(const std::vector<bool> &stateVector)
    {
        return -1;
    }

    void Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions)
    {
        this->transition_functions = transitionFunctions;
    }

    void Reachability::setInitState(const std::vector<bool> &stateVector)
    {        
        for(int i=0; i<stateVector.size(); i++){
            this->initial_state[i] = stateVector[i] ? True() : False();
        }
    }

}
