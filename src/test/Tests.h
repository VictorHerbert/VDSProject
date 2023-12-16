//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include "Manager.h"
#include <vector>
#include <gtest/gtest.h>
#include <iostream>

namespace ClassProject {

    class BasicTest : public testing::Test {

        protected:

        Manager testObj;

        void SetUp() override {
            testObj = Manager();
        }
    };

    class VariablesTest : public testing::Test {

        protected:

        Manager testObj;

        void SetUp() override {
            testObj = Manager({
                {.label = "0", .data = {.low = 0, .high = 0, .topVar = 0}},
                {.label = "1", .data = {.low = 1, .high = 1, .topVar = 1}},
                {.label = "a", .data = {.low = 0, .high = 1, .topVar = 2}},
                {.label = "b", .data = {.low = 0, .high = 1, .topVar = 3}},
                {.label = "c", .data = {.low = 0, .high = 1, .topVar = 4}},
                {.label = "d", .data = {.low = 0, .high = 1, .topVar = 5}}
            });
        }
    };

    class FunctionsTest : public testing::Test {

        protected:

        Manager testObj;

        void SetUp() override {
            testObj = Manager({
                {.label = "0", .data = {.low = 0, .high = 0, .topVar = 0}},
                {.label = "1", .data = {.low = 1, .high = 1, .topVar = 1}},
                {.label = "a", .data = {.low = 0, .high = 1, .topVar = 2}},
                {.label = "b", .data = {.low = 0, .high = 1, .topVar = 3}},
                {.label = "c", .data = {.low = 0, .high = 1, .topVar = 4}},
                {.label = "d", .data = {.low = 0, .high = 1, .topVar = 5}},
                {.label = "a+b", .data = {.low = 3, .high = 1, .topVar = 2}},
                {.label = "c*d", .data = {.low = 0, .high = 5, .topVar = 4}}
            });
        }
    };

    TEST_F(FunctionsTest, VisualizeBDD){
        BDD_ID s = 7;
        testObj.visualizeBDD("visualization.md", s);
    }

    TEST_F(BasicTest, Constructor){
        ASSERT_EQ(testObj.nodeData(0), (NodeData{.low=0, .high=0, .topVar=0})); // false node
        ASSERT_EQ(testObj.nodeData(1), (NodeData{.low=1, .high=1, .topVar=1})); // true node
    }

    TEST_F(FunctionsTest, ConstructorWithVector){
        ASSERT_EQ(testObj.nodeData(0), (NodeData{.low=0, .high=0, .topVar=0})); // false node
        ASSERT_EQ(testObj.nodeData(1), (NodeData{.low=1, .high=1, .topVar=1})); // true node

        ASSERT_EQ(testObj.nodeData(2), (NodeData{.low = 0, .high = 1, .topVar = 2})); // a node
        ASSERT_EQ(testObj.nodeData(7), (NodeData{.low = 0, .high = 5, .topVar = 4})); // c*d node
    }

    TEST_F(BasicTest, FalseReturn){
        BDD_ID id_false = testObj.False();
        ASSERT_EQ(id_false, 0);
    }

    TEST_F(BasicTest, TrueReturn){
        BDD_ID id_true = testObj.True();
        ASSERT_EQ(id_true, 1);
    }

    TEST_F(BasicTest, CreateVar){
        BDD_ID id_a = testObj.createVar("a");
        BDD_ID id_b = testObj.createVar("b");

        // node a
        ASSERT_EQ(testObj.nodeData(id_a), (NodeData{.low=0, .high=1, .topVar=id_a}));

        // node b
        ASSERT_EQ(testObj.nodeData(id_b), (NodeData{.low=0, .high=1, .topVar=id_b}));
    }

    TEST_F(FunctionsTest, isConstant){
        ASSERT_EQ(testObj.isConstant(0), true);  // false node
        ASSERT_EQ(testObj.isConstant(1), true);  // true node
        ASSERT_EQ(testObj.isConstant(2), false); // variable node (a)
        ASSERT_EQ(testObj.isConstant(6), false); // function node (a+b)
    }

    TEST_F(FunctionsTest, isVariable){
        ASSERT_EQ(testObj.isVariable(0), false);  // false node
        ASSERT_EQ(testObj.isVariable(1), false);  // true node
        ASSERT_EQ(testObj.isVariable(2), true);   // variable node (a)
        ASSERT_EQ(testObj.isVariable(3), true);   // variable node (b)
        ASSERT_EQ(testObj.isVariable(6), false);  // function node (a+b)
    }

    TEST_F(FunctionsTest, isExpression){
        ASSERT_EQ(testObj.isExpression(0), false);  // false node
        ASSERT_EQ(testObj.isExpression(1), false);  // true node
        ASSERT_EQ(testObj.isExpression(2), false);   // variable node (a)
        ASSERT_EQ(testObj.isExpression(3), false);   // variable node (b)
        ASSERT_EQ(testObj.isExpression(6), true);  // function node (a+b)
        ASSERT_EQ(testObj.isExpression(7), true);  // function node (c*d)
    }

    TEST_F(FunctionsTest, TopVar){
        ASSERT_EQ(testObj.topVar(0), 0);  // false node
        ASSERT_EQ(testObj.topVar(1), 1);  // true node
        ASSERT_EQ(testObj.topVar(2), 2);   // variable node (a)
        ASSERT_EQ(testObj.topVar(3), 3);   // variable node (b)
        ASSERT_EQ(testObj.topVar(6), 2);  // function node (a+b)
        ASSERT_EQ(testObj.topVar(7), 4);  // function node (c*d)
    }

    TEST_F(FunctionsTest, High){
        ASSERT_EQ(testObj.high(0), 0);  // false node
        ASSERT_EQ(testObj.high(1), 1);  // true node
        ASSERT_EQ(testObj.high(2), 1);   // variable node (a)
        ASSERT_EQ(testObj.high(3), 1);   // variable node (b)
        ASSERT_EQ(testObj.high(6), 1);  // function node (a+b)
        ASSERT_EQ(testObj.high(7), 5);  // function node (c*d)
    }

    TEST_F(FunctionsTest, Low){
        ASSERT_EQ(testObj.low(0), 0);  // false node
        ASSERT_EQ(testObj.low(1), 1);  // true node
        ASSERT_EQ(testObj.low(2), 0);   // variable node (a)
        ASSERT_EQ(testObj.low(3), 0);   // variable node (b)
        ASSERT_EQ(testObj.low(6), 3);  // function node (a+b)
        ASSERT_EQ(testObj.low(7), 0);  // function node (c*d)
    }

    TEST_F(FunctionsTest, UniqueTableSize){
        ASSERT_EQ(testObj.uniqueTableSize(), 8);
    }

    TEST_F(FunctionsTest, NodeDataReturn){
        BDD_ID id_a = 2, id_b = 3, id_or_ab = 6;
        ASSERT_EQ(testObj.nodeData(0), (NodeData{.low=testObj.low(0), .high=testObj.high(0), .topVar=testObj.topVar(0)}));
        ASSERT_EQ(testObj.nodeData(id_a), (NodeData{.low=testObj.low(id_a), .high=testObj.high(id_a), .topVar=testObj.topVar(id_a)}));
        ASSERT_EQ(testObj.nodeData(id_b), (NodeData{.low=testObj.low(id_b), .high=testObj.high(id_b), .topVar=testObj.topVar(id_b)}));
        ASSERT_EQ(testObj.nodeData(id_or_ab), (NodeData{.low=testObj.low(id_or_ab), .high=testObj.high(id_or_ab), .topVar=testObj.topVar(id_or_ab)}));
    }

    TEST_F(FunctionsTest, IteTerminalCases){
        BDD_ID id_a = 2, id_b = 3;
        ASSERT_EQ(testObj.ite(1, id_a, id_b),    id_a);
        ASSERT_EQ(testObj.ite(0, id_a, id_b),    id_b);
        ASSERT_EQ(testObj.ite(id_a, 1, 0),       id_a);
        ASSERT_EQ(testObj.ite(id_a, id_b, id_b), id_b);
    }

    TEST_F(FunctionsTest, IteVariables){
        BDD_ID id_a = 2, id_b = 3, id_c = 4;

        BDD_ID id_n = testObj.ite(id_a, id_b, id_c);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=id_c, .high=id_b, .topVar=id_a}));
    }

    TEST_F(FunctionsTest, IteComputedTable){
        BDD_ID id_a = 2, id_b = 3, id_c = 4;

        BDD_ID id_n1 = testObj.ite(id_a, id_b, id_c);
        BDD_ID id_n2 = testObj.ite(id_a, id_b, id_c);
        ASSERT_EQ(id_n1, id_n2);
    }

    TEST_F(FunctionsTest, CoFactorFalseStandard){
        ASSERT_EQ(testObj.low(0), testObj.coFactorFalse(0)); // constant node
        ASSERT_EQ(testObj.low(2), testObj.coFactorFalse(2)); // variable
        ASSERT_EQ(testObj.low(5), testObj.coFactorFalse(5)); // expression
    }

    TEST_F(FunctionsTest, CoFactorFalse){
        ASSERT_EQ(testObj.coFactorFalse(7), testObj.coFactorFalse(7, testObj.topVar(7))); // Must be equivalent
        ASSERT_EQ(testObj.coFactorFalse(2, 3), 2); // 3 is not in wrt of 2
        ASSERT_EQ(testObj.coFactorFalse(7, 5), 0); // coFactorFalse(c*d, d) = 0
    }

    TEST_F(FunctionsTest, CoFactorTrueStandard){
        ASSERT_EQ(testObj.high(0), testObj.coFactorTrue(0)); // constant node
        ASSERT_EQ(testObj.high(2), testObj.coFactorTrue(2)); // variable
        ASSERT_EQ(testObj.high(5), testObj.coFactorTrue(5)); // expression
    }

    TEST_F(FunctionsTest, CoFactorTrue){
        ASSERT_EQ(testObj.coFactorTrue(7), testObj.coFactorTrue(7, testObj.topVar(7))); // Must be equivalent
        ASSERT_EQ(testObj.coFactorTrue(2, 3), 2); // 3 is not in wrt of 2
        ASSERT_EQ(testObj.coFactorTrue(7, 5), 4); // coFactorFalse(c*d, d) = 0
    }

    TEST_F(FunctionsTest, GetTopVarName){
        ASSERT_EQ(testObj.getTopVarName(0), "0"); // false node
        ASSERT_EQ(testObj.getTopVarName(1), "1"); // true node
        ASSERT_EQ(testObj.getTopVarName(2), "a"); // variable node
        ASSERT_EQ(testObj.getTopVarName(7), "c"); // expression node
    }

    TEST_F(FunctionsTest, FindNodes){
        std::set<BDD_ID> nodes;

        testObj.findNodes(2, nodes); // Variable node
        ASSERT_EQ(nodes, (std::set<BDD_ID>{0,1,2}));

        nodes.clear();
        testObj.findNodes(7, nodes); // Expression node
        ASSERT_EQ(nodes, (std::set<BDD_ID>{0,1,5,7}));
    }

    TEST_F(FunctionsTest, FindVariables){
        std::set<BDD_ID> nodes;

        testObj.findVars(2, nodes); // Variable node
        ASSERT_EQ(nodes, (std::set<BDD_ID>{2}));

        nodes.clear();
        testObj.findVars(6, nodes); // Expression node
        ASSERT_EQ(nodes, (std::set<BDD_ID>{2,3}));

        nodes.clear();
        testObj.findVars(7, nodes); // Expression node
        ASSERT_EQ(nodes, (std::set<BDD_ID>{4,5}));
    }


    TEST_F(VariablesTest, And2Constants){
        BDD_ID id_n, id_a = 2;
        id_n = testObj.and2(0, 0);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.and2(0, 1);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.and2(1, 0);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.and2(1, 1);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.and2(id_a, 1);
        ASSERT_EQ(id_n, id_a);

        id_n = testObj.and2(1, id_a);
        ASSERT_EQ(id_n, id_a);

        id_n = testObj.and2(id_a, 0);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.and2(0, id_a);
        ASSERT_EQ(id_n, 0);
    }

    TEST_F(VariablesTest, And2Variables1){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.and2(id_a, id_b);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=0, .high=id_b, .topVar=id_a}));
    }

    TEST_F(VariablesTest, And2Variables2){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.and2(id_b, id_a);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=0, .high=id_b, .topVar=id_a}));
    }

    TEST_F(VariablesTest, Or2Constants){
        BDD_ID id_n, id_a = 2;
        id_n = testObj.or2(0, 0);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.or2(0, 1);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.or2(1, 0);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.or2(1, 1);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.or2(id_a, 1);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.or2(1, id_a);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.or2(id_a, 0);
        ASSERT_EQ(id_n, id_a);

        id_n = testObj.or2(0, id_a);
        ASSERT_EQ(id_n, id_a);
    }

    TEST_F(VariablesTest, Or2Variables1){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.or2(id_a, id_b);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=id_b, .high=1, .topVar=id_a}));
    }

    TEST_F(VariablesTest, Or2Variables2){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.or2(id_b, id_a);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=id_b, .high=1, .topVar=id_a}));
    }

    TEST_F(BasicTest, NegConstants){
        BDD_ID id_n;
        id_n = testObj.neg(0);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.neg(1);
        ASSERT_EQ(id_n, 0);
    }

    TEST_F(VariablesTest, NegVariable){
        BDD_ID id_n, id_a = 2;
        id_n = testObj.neg(id_a);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=1, .high=0, .topVar=id_a}));
    }

    TEST_F(VariablesTest, Xor2Constants){
        BDD_ID id_n, id_m, id_a = 2;
        id_n = testObj.xor2(0, 0);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.xor2(0, 1);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.xor2(1, 0);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.xor2(1, 1);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.xor2(id_a, 1);
        id_m = testObj.neg(id_a);
        ASSERT_EQ(id_n, id_m);

        id_n = testObj.xor2(1, id_a);
        id_m = testObj.neg(id_a);
        ASSERT_EQ(id_n, id_m);

        id_n = testObj.xor2(id_a, 0);
        ASSERT_EQ(id_n, id_a);

        id_n = testObj.xor2(0, id_a);
        ASSERT_EQ(id_n, id_a);
    }

    TEST_F(VariablesTest, Xor2Variables1){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.xor2(id_a, id_b);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=id_b, .high=testObj.neg(id_b), .topVar=id_a}));
    }

    TEST_F(VariablesTest, Xor2Variables2){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.xor2(id_b, id_a);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=id_b, .high=testObj.neg(id_b), .topVar=id_a}));
    }

    TEST_F(VariablesTest, Nand2Constants){
        BDD_ID id_n, id_m, id_a = 2;
        id_n = testObj.nand2(0, 0);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.nand2(0, 1);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.nand2(1, 0);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.nand2(1, 1);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.nand2(id_a, 1);
        id_m = testObj.neg(id_a);
        ASSERT_EQ(id_n, id_m);

        id_n = testObj.nand2(1, id_a);
        id_m = testObj.neg(id_a);
        ASSERT_EQ(id_n, id_m);

        id_n = testObj.nand2(id_a, 0);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.nand2(0, id_a);
        ASSERT_EQ(id_n, 1);
    }

    TEST_F(VariablesTest, Nand2Variables1){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.nand2(id_a, id_b);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=1, .high=testObj.neg(id_b), .topVar=id_a}));
    }

    TEST_F(VariablesTest, Nand2Variables2){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.nand2(id_b, id_a);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=1, .high=testObj.neg(id_b), .topVar=id_a}));
    }

    TEST_F(VariablesTest, Nor2Constants){
        BDD_ID id_n, id_m, id_a = 2;
        id_n = testObj.nor2(0, 0);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.nor2(0, 1);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.nor2(1, 0);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.nor2(1, 1);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.nor2(id_a, 0);
        id_m = testObj.neg(id_a);
        ASSERT_EQ(id_n, id_m);

        id_n = testObj.nor2(0, id_a);
        id_m = testObj.neg(id_a);
        ASSERT_EQ(id_n, id_m);

        id_n = testObj.nor2(id_a, 1);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.nor2(1, id_a);
        ASSERT_EQ(id_n, 0);
    }

    TEST_F(VariablesTest, Nor2Variables1){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.nor2(id_a, id_b);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=testObj.neg(id_b), .high=0, .topVar=id_a}));
    }

    TEST_F(VariablesTest, Nor2Variables2){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.nor2(id_b, id_a);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=testObj.neg(id_b), .high=0, .topVar=id_a}));
    }

    TEST_F(VariablesTest, Xnor2Constants){
        BDD_ID id_n, id_m, id_a = 2;
        id_n = testObj.xnor2(0, 0);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.xnor2(0, 1);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.xnor2(1, 0);
        ASSERT_EQ(id_n, 0);

        id_n = testObj.xnor2(1, 1);
        ASSERT_EQ(id_n, 1);

        id_n = testObj.xnor2(id_a, 0);
        id_m = testObj.neg(id_a);
        ASSERT_EQ(id_n, id_m);

        id_n = testObj.xnor2(0, id_a);
        id_m = testObj.neg(id_a);
        ASSERT_EQ(id_n, id_m);

        id_n = testObj.xnor2(id_a, 1);
        ASSERT_EQ(id_n, id_a);

        id_n = testObj.xnor2(1, id_a);
        ASSERT_EQ(id_n, id_a);
    }

    TEST_F(VariablesTest, Xnor2Variables1){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.xnor2(id_b, id_a);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=testObj.neg(id_b), .high=id_b, .topVar=id_a}));
    }

    TEST_F(VariablesTest, Xnor2Variables2){
        BDD_ID id_n, id_a = 2, id_b = 3;
        id_n = testObj.xnor2(id_b, id_a);
        ASSERT_EQ(testObj.nodeData(id_n), (NodeData{.low=testObj.neg(id_b), .high=id_b, .topVar=id_a}));
    }

    TEST_F(BasicTest, InterfaceTest){
        BDD_ID id_a, id_b, id_c, id_d, id_or_ab, id_and_cd, id_f, id_f1;

        id_a = testObj.createVar("a");
        id_b = testObj.createVar("b");
        id_c = testObj.createVar("c");
        id_d = testObj.createVar("d");

        id_or_ab = testObj.or2(id_a, id_b);
        id_and_cd = testObj.and2(id_c, id_d);

        id_f = testObj.and2(id_or_ab, id_and_cd);

        // extra node inserted by and 2 operation
        id_f1 = testObj.low(id_f);

        ASSERT_EQ(testObj.uniqueTableSize(), 10);

        ASSERT_EQ(testObj.high(id_f), id_and_cd);
        ASSERT_EQ(testObj.topVar(id_f), id_a);

        ASSERT_EQ(testObj.nodeData(id_f1), (NodeData{.low=0, .high=id_and_cd, .topVar=id_b}));
    }
}

#endif