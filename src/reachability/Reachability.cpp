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
        tau = characteristic_function(next_state_variables, transition_functions);        
        
        // compute the characteristic function of the initial state
        cs0 = characteristic_function(state_variables, initial_state);        

        // fixed point iteration
        cri = cs0;
        do {

            cr = cri;

            // compute image of next state variables (step 7)
            img = compute_image(tau, cr);          

            cri = this->or2(cr, img);

        } while(cri != cr);

        // check if the stateVector belong to the reachable state space
        for(int i=0; i<stateVector.size(); i++){
            BDD_ID sv = stateVector[i] ? True() : False();
            cri = this->and2(cri, this->xnor2(sv, state_variables[i]));
        }

        // existential quantification with respect to the next state variables
        cri = existential_quantification(cri, state_variables);    

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

    const std::vector<BDD_ID> Reachability::getTransitionFunctions(){
        return this->transition_functions;
    }

    const std::vector<bool> Reachability::Reachability::getInitState(){
        std::vector<bool> initial_state = std::vector<bool>();
        for(auto s : this->initial_state){
            initial_state.push_back(s == True() ? true : false);                        
        }    
        return initial_state;
    }

    BDD_ID Reachability::existential_quantification(BDD_ID equation, std::vector<BDD_ID> variables)
    {
        BDD_ID eq = equation;
        for(int i=0; i<variables.size(); i++){
            eq = this->or2(this->coFactorTrue(eq, variables[i]), this->coFactorFalse(eq, variables[i]));
        }

        return eq;
    }

    BDD_ID Reachability::characteristic_function(std::vector<BDD_ID> equations, std::vector<BDD_ID> variables)
    {
        BDD_ID cs = this->xnor2(variables[0], equations[0]);
        for(int i=1; i<equations.size(); i++){
            cs = this->and2(
                cs,
                this->xnor2(variables[i], equations[i])
            );
        }

        return cs;
    }

    BDD_ID Reachability::compute_image(BDD_ID tau, BDD_ID cr)
    {
        // compute image for next state variables
        BDD_ID image = this->and2(tau, cr);

        // get rid of the input and the current state variables
        // (we are only interested in the reached states, not how to reach them)
        image = existential_quantification(image, state_variables);
        image = existential_quantification(image, input_variables);

        // rename the next state variables into current state variables for next iteration
        for(int i=0; i<state_variables.size(); i++){
            image = this->and2(image, this->xnor2(this->state_variables[i], this->next_state_variables[i]));
        }
        image = existential_quantification(image, next_state_variables);

        return image;
    }

}