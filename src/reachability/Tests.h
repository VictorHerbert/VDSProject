#ifndef VDSPROJECT_REACHABILITY_TESTS_H
#define VDSPROJECT_REACHABILITY_TESTS_H

#include <gtest/gtest.h>
#include "Reachability.h"

using namespace ClassProject;

template <int state_size, int input_size = 0>
struct ReachabilityTest : testing::Test {

    std::unique_ptr<ClassProject::Reachability> fsm = std::make_unique<ClassProject::Reachability>(state_size, input_size);

    std::vector<BDD_ID> stateVars = fsm->getStates();
    std::vector<BDD_ID> inputVars = fsm->getInputs();
    std::vector<BDD_ID> transitionFunctions;
    std::vector<bool> initialState;

};

struct ReachabilityTest20 : ReachabilityTest<2,0> {};
struct ReachabilityTest22 : ReachabilityTest<2,2> {};

TEST(ReachabilityTestConstructor, ConstructorStateSize) {

    EXPECT_THROW(
        ClassProject::Reachability(0), 
        std::runtime_error
    );
    EXPECT_NO_THROW(
        ClassProject::Reachability(1)        
    );

}

TEST_F(ReachabilityTest20, ConstructorInputSize){

    // check if state variables were properly initialized 
    EXPECT_EQ(stateVars.size(), 2);
    EXPECT_EQ(inputVars.size(), 0);

    // check if the transition functions were properly initialized
    transitionFunctions = fsm->getTransitionFunctions();
    initialState = fsm->getInitState();

    ASSERT_EQ(transitionFunctions.size(), 2);

    EXPECT_EQ(transitionFunctions[0], stateVars[0]);
    EXPECT_EQ(transitionFunctions[1], stateVars[1]);

    // check if intitial state was properly initialized
    ASSERT_EQ(initialState.size(), 2); 

    EXPECT_EQ(initialState[0], false);
    EXPECT_EQ(initialState[1], false);
}

TEST_F(ReachabilityTest22, ConstructorDefaults){

    // check if state variables were properly initialized     
    EXPECT_EQ(inputVars.size(), 2);    
}

TEST_F(ReachabilityTest22, setInitStateException){
    
    EXPECT_THROW(
        fsm->setInitState({true, true, false}),
        std::runtime_error
    );    
}

TEST_F(ReachabilityTest22, setInitState){
    
    fsm->setInitState({true, true});

    EXPECT_EQ((fsm->getInitState()).at(0), true);
    EXPECT_EQ((fsm->getInitState()).at(1), true);       
}

TEST_F(ReachabilityTest22, setTransitionFunctionsException1){

    BDD_ID s0 = stateVars.at(0);
    BDD_ID s1 = stateVars.at(1);

    transitionFunctions.push_back(fsm->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm->neg(s1)); // s1' = not(s1)
    transitionFunctions.push_back(fsm->neg(s1)); // s1' = not(s1)    

    EXPECT_THROW(
        fsm->setTransitionFunctions(transitionFunctions),
        std::runtime_error
    );    
}

TEST_F(ReachabilityTest22, setTransitionFunctionsException2){

    BDD_ID s0 = stateVars.at(0);
    BDD_ID s1 = stateVars.at(1);

    transitionFunctions.push_back(fsm->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(100); // unknown ID

    EXPECT_THROW(
        fsm->setTransitionFunctions(transitionFunctions),
        std::runtime_error
    );    
}

TEST_F(ReachabilityTest22, setTransitionFunctions){

    BDD_ID s0 = stateVars.at(0);
    BDD_ID s1 = stateVars.at(1);

    transitionFunctions.push_back(fsm->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm->neg(s1)); // s1' = not(s1)

    fsm->setTransitionFunctions(transitionFunctions);

    EXPECT_EQ(
        (fsm->getTransitionFunctions()).at(0),
        transitionFunctions.at(0)
    );

    EXPECT_EQ(
        (fsm->getTransitionFunctions()).at(1),
        transitionFunctions.at(1)
    );
}

#endif
