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
struct ReachabilityTest31 : ReachabilityTest<3,1> {};

TEST(ReachabilityTestConstructor, ConstructorStateSizeException) {

    EXPECT_THROW(
        ClassProject::Reachability(0),
        std::runtime_error
    );
    EXPECT_NO_THROW(
        ClassProject::Reachability(1)
    );

}

TEST_F(ReachabilityTest20, ConstructorDefaults){

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

TEST_F(ReachabilityTest22, ConstructorInputSize){

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

    fsm->setInitState({false, true});

    EXPECT_EQ((fsm->getInitState()).at(0), false);
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


TEST_F(ReachabilityTest31, stateDistanceException){
    EXPECT_THROW(
        fsm->stateDistance({false, false}),
        std::runtime_error
    );
}

TEST_F(ReachabilityTest20, stateDistanceTestBasic){

    BDD_ID s1 = stateVars.at(0);
    BDD_ID s2 = stateVars.at(1);

    transitionFunctions.push_back(fsm->xor2(s1,s2));
    transitionFunctions.push_back(
        fsm->or2(
            fsm->and2(fsm->neg(s1), fsm->neg(s2)),
            s1
        )
    );

    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, true});

    /*EXPECT_EQ(fsm->stateDistance({false, false}), -1);
    EXPECT_EQ(fsm->stateDistance({false, false}),  0);
    EXPECT_EQ(fsm->stateDistance({true, true}),    1);
    EXPECT_EQ(fsm->stateDistance({true, false}),   2);    */

    EXPECT_EQ(fsm->stateDistance({0, 0}), Reachability::UNREACHABLE);
    EXPECT_EQ(fsm->stateDistance({0, 1}), 0);
    EXPECT_EQ(fsm->stateDistance({1, 0}), 1);
    EXPECT_EQ(fsm->stateDistance({1, 1}), 2);
}

TEST_F(ReachabilityTest31, stateDistanceTest){

    BDD_ID s1 = stateVars.at(0);
    BDD_ID s2 = stateVars.at(1);
    BDD_ID s3 = stateVars.at(2);
    BDD_ID x  = inputVars.at(0);

    transitionFunctions.push_back(
        fsm->or2(
            fsm->or2(
                fsm->and2(s1,fsm->and2(fsm->neg(s2), fsm->neg(s3))),
                fsm->and2(x,fsm->and2(s1, fsm->neg(s2)))
            ),
            fsm->or2(
                fsm->and2(fsm->neg(x),fsm->and2(s2,s3)),
                fsm->and2(s1, fsm->and2(s2, s3))
            )
        )
    );

    transitionFunctions.push_back(
        fsm->or2(
            fsm->or2(
                fsm->and2(fsm->neg(s1),fsm->and2(fsm->neg(s2), s3)),
                fsm->and2(fsm->neg(s1),fsm->and2(s2, fsm->neg(s3)))
            ),
            fsm->or2(fsm->and2(x, s3),fsm->and2(x, fsm->neg(s1)))
        )
    );

    transitionFunctions.push_back(
        fsm->or2(
            fsm->or2(
                fsm->and2(s1,fsm->and2(s2, fsm->neg(s3))),
                fsm->and2(fsm->neg(s1),fsm->and2(s3, fsm->neg(s2)))
            ),
            fsm->and2(
                fsm->neg(x),
                fsm->and2(fsm->neg(s1),fsm->neg(s2))
            )
        )
    );

    fsm->setTransitionFunctions(transitionFunctions);

    fsm->setInitState({false, false, false});
    EXPECT_EQ(fsm->stateDistance({true, true, true}),   -1);
    EXPECT_EQ(fsm->stateDistance({false, false, false}), 0);
    EXPECT_EQ(fsm->stateDistance({false, true, false}),  1);
    EXPECT_EQ(fsm->stateDistance({false, false, true}),  1);
    EXPECT_EQ(fsm->stateDistance({false, true, true}),   2);
    EXPECT_EQ(fsm->stateDistance({true, false, false}),  3);

    fsm->setInitState({false, false, true});
    EXPECT_EQ(fsm->stateDistance({false, false, false}),-1);
    EXPECT_EQ(fsm->stateDistance({true, true, true}),   -1);
    EXPECT_EQ(fsm->stateDistance({false, false, true}),  0);
    EXPECT_EQ(fsm->stateDistance({false, true, true}),   1);
    EXPECT_EQ(fsm->stateDistance({false, true, false}),  2);
    EXPECT_EQ(fsm->stateDistance({true, false, false}),  2);

    fsm->setInitState({true, false, true});
    EXPECT_EQ(fsm->stateDistance({false, true, true}),   3);
}

TEST_F(ReachabilityTest31, isReachableException){
    EXPECT_THROW(
        fsm->isReachable({false, false}),
        std::runtime_error
    );
}

TEST_F(ReachabilityTest20, isReachableTestBasic){

    BDD_ID s1 = stateVars.at(0);
    BDD_ID s2 = stateVars.at(1);

    transitionFunctions.push_back(fsm->xor2(s1, s2));
    transitionFunctions.push_back(
        fsm->or2(
            fsm->and2(fsm->neg(s1), fsm->neg(s2)),
            s1
        )
    );

    fsm->setTransitionFunctions(transitionFunctions);
    fsm->setInitState({false, true});
    EXPECT_EQ(fsm->isReachable({false, false}), false);    
    EXPECT_EQ(fsm->isReachable({false, true}), true);    
    EXPECT_EQ(fsm->isReachable({true, false}),  true);    
    EXPECT_EQ(fsm->isReachable({true, true}),   true);    

}


TEST_F(ReachabilityTest31, isReachableTest){

    BDD_ID s1 = stateVars.at(0);
    BDD_ID s2 = stateVars.at(1);
    BDD_ID s3 = stateVars.at(2);
    BDD_ID x  = inputVars.at(0);

    transitionFunctions.push_back(
        fsm->or2(
            fsm->or2(
                fsm->and2(
                    s1,
                    fsm->and2(fsm->neg(s2), fsm->neg(s3))
                ),
                fsm->and2(
                    x,
                    fsm->and2(s1, fsm->neg(s2))
                )
            ), 
            fsm->or2(
                fsm->and2(
                    fsm->neg(x),
                    fsm->and2(s2,s3)
                ),
                fsm->and2(
                    s1,
                    fsm->and2(s2, s3)
                )
            )       
        )
    );

    transitionFunctions.push_back(
        fsm->or2(
            fsm->or2(
                fsm->and2(
                    fsm->neg(s1),
                    fsm->and2(fsm->neg(s2), s3)
                ),
                fsm->and2(
                    fsm->neg(s1),
                    fsm->and2(s2, fsm->neg(s3))
                )
            ), 
            fsm->or2(
                fsm->and2(x, s3),
                fsm->and2(x, fsm->neg(s1))
            )       
        )
    );   

    transitionFunctions.push_back(
        fsm->or2(
            fsm->or2(
                fsm->and2(
                    s1,
                    fsm->and2(s2, fsm->neg(s3))
                ),
                fsm->and2(
                    fsm->neg(s1),
                    fsm->and2(s3, fsm->neg(s2))
                )
            ), 
            fsm->and2(
                fsm->neg(x),
                fsm->and2(fsm->neg(s1),fsm->neg(s2))
            )            
        )
    );    

    fsm->setTransitionFunctions(transitionFunctions);

    fsm->setInitState({false, false, false});
    EXPECT_EQ(fsm->isReachable({false, false, false}), true );
    EXPECT_EQ(fsm->isReachable({false, false, true}), true);
    EXPECT_EQ(fsm->isReachable({false, true, false}), true);
    EXPECT_EQ(fsm->isReachable({false, true, true}), true);
    EXPECT_EQ(fsm->isReachable({true, false, false}), true);
    EXPECT_EQ(fsm->isReachable({true, false, true}), false);
    EXPECT_EQ(fsm->isReachable({true, true, false}), false);
    EXPECT_EQ(fsm->isReachable({true, true, true}), false);

    fsm->setInitState({false, false, true});
    EXPECT_EQ(fsm->isReachable({false, false, false}), false );
    EXPECT_EQ(fsm->isReachable({false, false, true}), true);
    EXPECT_EQ(fsm->isReachable({false, true, false}), true);
    EXPECT_EQ(fsm->isReachable({false, true, true}), true);
    EXPECT_EQ(fsm->isReachable({true, false, false}), true);
    EXPECT_EQ(fsm->isReachable({true, false, true}), false);
    EXPECT_EQ(fsm->isReachable({true, true, false}), false);
    EXPECT_EQ(fsm->isReachable({true, true, true}), false);

    fsm->setInitState({false, true, false});
    EXPECT_EQ(fsm->isReachable({false, false, false}), false );
    EXPECT_EQ(fsm->isReachable({false, false, true}), false);
    EXPECT_EQ(fsm->isReachable({false, true, false}), true);
    EXPECT_EQ(fsm->isReachable({false, true, true}), false);
    EXPECT_EQ(fsm->isReachable({true, false, false}), false);
    EXPECT_EQ(fsm->isReachable({true, false, true}), false);
    EXPECT_EQ(fsm->isReachable({true, true, false}), false);
    EXPECT_EQ(fsm->isReachable({true, true, true}), false);

    fsm->setInitState({false, true, true});
    EXPECT_EQ(fsm->isReachable({false, false, false}), false );
    EXPECT_EQ(fsm->isReachable({false, false, true}), false);
    EXPECT_EQ(fsm->isReachable({false, true, false}), true);
    EXPECT_EQ(fsm->isReachable({false, true, true}), true);
    EXPECT_EQ(fsm->isReachable({true, false, false}), true);
    EXPECT_EQ(fsm->isReachable({true, false, true}), false);
    EXPECT_EQ(fsm->isReachable({true, true, false}), false);
    EXPECT_EQ(fsm->isReachable({true, true, true}), false);

    fsm->setInitState({true, false, false});
    EXPECT_EQ(fsm->isReachable({false, false, false}), false );
    EXPECT_EQ(fsm->isReachable({false, false, true}), false);
    EXPECT_EQ(fsm->isReachable({false, true, false}), false);
    EXPECT_EQ(fsm->isReachable({false, true, true}), false);
    EXPECT_EQ(fsm->isReachable({true, false, false}), true);
    EXPECT_EQ(fsm->isReachable({true, false, true}), false);
    EXPECT_EQ(fsm->isReachable({true, true, false}), false);
    EXPECT_EQ(fsm->isReachable({true, true, true}), false);

    fsm->setInitState({true, false, true});
    EXPECT_EQ(fsm->isReachable({false, false, false}), true );
    EXPECT_EQ(fsm->isReachable({false, false, true}), true);
    EXPECT_EQ(fsm->isReachable({false, true, false}), true);
    EXPECT_EQ(fsm->isReachable({false, true, true}), true);
    EXPECT_EQ(fsm->isReachable({true, false, false}), true);
    EXPECT_EQ(fsm->isReachable({true, false, true}), true);
    EXPECT_EQ(fsm->isReachable({true, true, false}), true);
    EXPECT_EQ(fsm->isReachable({true, true, true}), false);

    fsm->setInitState({true, true, false});
    EXPECT_EQ(fsm->isReachable({false, false, false}), false );
    EXPECT_EQ(fsm->isReachable({false, false, true}), true);
    EXPECT_EQ(fsm->isReachable({false, true, false}), true);
    EXPECT_EQ(fsm->isReachable({false, true, true}), true);
    EXPECT_EQ(fsm->isReachable({true, false, false}), true);
    EXPECT_EQ(fsm->isReachable({true, false, true}), false);
    EXPECT_EQ(fsm->isReachable({true, true, false}), true);
    EXPECT_EQ(fsm->isReachable({true, true, true}), false);

     fsm->setInitState({true, true, true});
    EXPECT_EQ(fsm->isReachable({false, false, false}), false );
    EXPECT_EQ(fsm->isReachable({false, false, true}), true);
    EXPECT_EQ(fsm->isReachable({false, true, false}), true);
    EXPECT_EQ(fsm->isReachable({false, true, true}), true);
    EXPECT_EQ(fsm->isReachable({true, false, false}), true);
    EXPECT_EQ(fsm->isReachable({true, false, true}), false);
    EXPECT_EQ(fsm->isReachable({true, true, false}), true);
    EXPECT_EQ(fsm->isReachable({true, true, true}), true);
}

TEST(Distance_Test, distanceExample) { /* NOLINT */

    std::unique_ptr<ClassProject::Reachability> distanceFSM = std::make_unique<ClassProject::Reachability>(2,1);
    std::vector<BDD_ID> stateVars6 = distanceFSM->getStates();
    std::vector<BDD_ID> transitionFunctions;

    auto s0 = stateVars6.at(0);
    auto s1 = stateVars6.at(1);
    auto inputs = distanceFSM->getInputs();
    auto i = inputs.at(0);

    auto nots1 = distanceFSM->neg(s1);
    auto nots0 = distanceFSM->neg(s0);
    auto noti = distanceFSM->neg(i);

    //s0' = not(s1)*not(s0)*not(i) + !s1*s0*!i + s1*!s0*!i
    auto s0trans = distanceFSM->or2(distanceFSM->or2(distanceFSM->and2(distanceFSM->and2(nots1, nots0), noti), distanceFSM->and2(distanceFSM->and2(nots1, s0), noti)), distanceFSM->and2(distanceFSM->and2(s1, nots0), noti));
    //s1' = !s1*!s0 + s1*!s0*i + s1s0i
    auto s1trans = distanceFSM->or2(distanceFSM->or2(distanceFSM->and2(nots1, nots0), distanceFSM->and2(distanceFSM->and2(s1, nots0), i)), distanceFSM->and2(distanceFSM->and2(s1, s0), i));

    transitionFunctions.push_back(s0trans);
    transitionFunctions.push_back(s1trans);
    //s1' = not(s3) and (s3 or (s3 nand (s0 and s1)))
    distanceFSM->setTransitionFunctions(transitionFunctions);
    /*
     * Reset state A {false, false}
     * B {false, true}
     * C {true, false}
     * D {true, true}
     * input i
     *
     * A, i = 1 -> B
     * A, i = 0 -> D
     * B, i = 1 -> B
     * B, i = 0 -> C
     * C, i = 1 -> A
     * C, i = 0 -> C
     * D, i = 1 -> B
     * D, i = 0 -> A
     */

    distanceFSM->setInitState({false,false});
    ASSERT_TRUE (distanceFSM->isReachable({true, false}));
    ASSERT_TRUE (distanceFSM->isReachable({false,  true}));
    ASSERT_TRUE (distanceFSM->isReachable({true, true}));
    ASSERT_TRUE (distanceFSM->isReachable({false, false}));

    ASSERT_EQ (distanceFSM->stateDistance({false, false}), 0);
    ASSERT_EQ (distanceFSM->stateDistance({false, true}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({true, true}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({true, false}), 2);

    distanceFSM->setInitState({false,true});
    ASSERT_TRUE (distanceFSM->isReachable({true, false}));
    ASSERT_TRUE (distanceFSM->isReachable({false,  true}));
    ASSERT_TRUE (distanceFSM->isReachable({true, true}));
    ASSERT_TRUE (distanceFSM->isReachable({false, false}));

    ASSERT_EQ (distanceFSM->stateDistance({false, false}), 2);
    ASSERT_EQ (distanceFSM->stateDistance({false, true}), 0);
    ASSERT_EQ (distanceFSM->stateDistance({true, true}), 3);
    ASSERT_EQ (distanceFSM->stateDistance({true, false}), 1);


    distanceFSM->setInitState({true,false});
    ASSERT_TRUE (distanceFSM->isReachable({true, false}));
    ASSERT_TRUE (distanceFSM->isReachable({false,  true}));
    ASSERT_TRUE (distanceFSM->isReachable({true, true}));
    ASSERT_TRUE (distanceFSM->isReachable({false, false}));

    ASSERT_EQ (distanceFSM->stateDistance({false, false}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({false, true}), 2);
    ASSERT_EQ (distanceFSM->stateDistance({true, true}), 2);
    ASSERT_EQ (distanceFSM->stateDistance({true, false}), 0);

    distanceFSM->setInitState({true,true});
    ASSERT_TRUE (distanceFSM->isReachable({true, false}));
    ASSERT_TRUE (distanceFSM->isReachable({false,  true}));
    ASSERT_TRUE (distanceFSM->isReachable({true, true}));
    ASSERT_TRUE (distanceFSM->isReachable({false, false}));

    ASSERT_EQ (distanceFSM->stateDistance({false, false}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({false, true}), 1);
    ASSERT_EQ (distanceFSM->stateDistance({true, true}), 0);
    ASSERT_EQ (distanceFSM->stateDistance({true, false}), 2);

}

#endif
