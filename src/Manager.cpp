#include "Manager.h"
#include <stdexcept>
#include <fstream>
#include <unordered_set>

namespace ClassProject {

    std::ostream& operator <<(std::ostream& stream, const Node& node) {
        return stream << node.label << " : " << node.data.low << "," << node.data.high << "," << node.data.topVar;
    }

    const BDD_ID Manager::FALSE_ADDRESS = 0;
    const BDD_ID Manager::TRUE_ADDRESS = 1;

    const Node Manager::FALSE_NODE = {.label = "0", .data = {.low = 0, .high = 0, .topVar = Manager::FALSE_ADDRESS}};
    const Node Manager::TRUE_NODE =  {.label = "1", .data = {.low = 1, .high = 1, .topVar = Manager::TRUE_ADDRESS}};

    /**
    * @brief Manager class standard constructor
    *
    * @return BDD_ID ID of top variable node
    * @author Victor Herbert
    */
    Manager::Manager(){
        nodes = {FALSE_NODE, TRUE_NODE};
        unique_table.insert({{.low = 0, .high = 0, .topVar = Manager::FALSE_ADDRESS}, FALSE_ADDRESS});
        unique_table.insert({{.low = 1, .high = 1, .topVar = Manager::TRUE_ADDRESS}, TRUE_ADDRESS});
    }

    /**
     * @brief Construct a new Manager:: Manager object
     *
     * Takes and already constructed set of nodes and applies validation
     * on it to ensure its consistency
     *
     * @param nodes
     * @author Kamel Fakih
     */
    Manager::Manager(std::vector<Node> nodes) : nodes(nodes){
        if(!(nodes[0] == FALSE_NODE) || !(nodes[1] == TRUE_NODE)){
            throw std::invalid_argument("true and false nodes are invalid");
        }

        // TODO add more validation on input nodes

        for(size_t i=0; i < nodes.size(); i++){
            unique_table.insert({nodes[i].data, i});
        }
    }

    /**
     * @brief returns the number of nodes in the unique table
     *
     * @return size_t
     * @author Kamel Fakih
     */
    size_t Manager::uniqueTableSize(){
        return unique_table.size();
    }

    /**
     * @brief returns a pointer for the True node
     *
     * @return const BDD_ID& of the True node
     * @author Victor Herbert
     */
    const BDD_ID &Manager::True(){
        return TRUE_ADDRESS;
    }

    /**
     * @brief returns a pointer for the False node
     *
     * @return const BDD_ID& of the False node
     * @author Victor Herbert
     */
    const BDD_ID &Manager::False(){
        return FALSE_ADDRESS;
    }


    /**
     * @brief Checks if a given node is a constant
     *
     * @return bool stating if the node is constant
     * @author Victor Herbert
     */
    bool Manager::isConstant(BDD_ID f){
        return (f == True() || f == False());
    }

    /**
     * @brief Checks if a given node is a variable
     *
     * @return bool stating if the node is avariable
     * @author Victor Herbert
     */
    bool Manager::isVariable(BDD_ID x){
        return nodes[x].data.topVar == x && !isConstant(x);
    }

    /**
     * @brief Checks if a given node is an expression
     *
     * @return bool stating if the node is a expression
     * @author Victor Herbert
     */
    bool Manager::isExpression(BDD_ID x){
        return !isConstant(x) && !isVariable(x);
    }

    /**
    * @brief Creates a variable in the tree
    *
    * @return BDD_ID ID of the variable node
    * @author Victor Herbert
    */
    BDD_ID Manager::createVar(const std::string &label){
        return addNode({
            .label=label,
            .data={.low=FALSE_ADDRESS, .high=TRUE_ADDRESS, .topVar=nodes.size()}
        });
    }

    /**
     * @brief Adds a new entry on the unique table only if there is not a previous entry with same data
     *
     * @param data of the to be added node
     * @return BDD_ID of the added node
     * @author Victor Herbert
     */
    BDD_ID Manager::addNode(Node node){
        if(unique_table.find(node.data) == unique_table.end()){
            nodes.push_back(node);
            unique_table[node.data] = nodes.size()-1;
        }
        return unique_table[node.data];
    }

    /**
     * @brief If Then Else routine
     *
     * i t + ~i e
     * Performs the IF Then Else recursive algorithm, adding nodes on demand
     *
     * @param i ID of the If node
     * @param t ID of the Then node
     * @param e ID of the Else node
     * @return BDD_ID of the added node
     * @author Victor Herbert
     */
    BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e){
        if(i == True()) return t;
        if(i == False()) return e;
        if(t == True() and e == False()) return i;
        if(t == e) return t;
        //std::cout << i << " " << t << " " << e << std::endl;

        NodeData nodeData = {.low=t, .high=e, .topVar=i};

        if(computed_table.find(nodeData) == computed_table.end()){
            BDD_ID top = topVar(i);
            if(!isConstant(t))
                top = std::min(top, topVar(t));
            if(!isConstant(e))
                top = std::min(top, topVar(e));

            BDD_ID high = ite(coFactorTrue(i,top), coFactorTrue(t,top), coFactorTrue(e,top));
            BDD_ID low = ite(coFactorFalse(i,top), coFactorFalse(t,top), coFactorFalse(e,top));

            if(high == low){
                computed_table[nodeData] = high;
            }
            else{
                BDD_ID result = addNode({.data={.low=low, .high=high, .topVar=top}});
                computed_table[nodeData] = result;
            }
        }

        return computed_table[nodeData];
    }

    /**
     * @brief Update the label of a node given the name of the operation it realizes
     *
     * @param id of the created node
     * @param a first parameter of the operation
     * @param b second parameter of the operation
     * @param op string containning the name of the operation
     * @author Victor Herbert
     */
    void Manager::updateNodeLabel(BDD_ID id, BDD_ID a, BDD_ID b, std::string op){
        if(nodes[id].label == "")
            nodes[id].label = op + "(" + nodes[a].label + "," + nodes[b].label + ")";
    }

    /**
     * @brief returns the label of the top variable of root
     *
     * @param root
     * @return std::string
     * @author Kamel Fakih
     */
    std::string Manager::getTopVarName(const BDD_ID &root){
        return nodes[topVar(root)].label;
    }

    /**
     * @brief creates a node that represents the AND operation between other two nodes
     *
     * @param a operand
     * @param b operand
     * @return BDD_ID of the node which encapsulates the operation
     * @author Victor Herbert
     */
    BDD_ID Manager::and2(BDD_ID a, BDD_ID b){
        BDD_ID id = ite(a,b, 0);
        #ifdef UPDATE_LABEL
        updateNodeLabel(id, a, b, "and2");
        #endif
        return id;
    }

    /**
     * @brief creates a node that represents the OR operation between other two nodes
     *
     * @param a operand
     * @param b operand
     * @return BDD_ID of the node which encapsulates the operation
     * @author Victor Herbert
     */
    BDD_ID Manager::or2(BDD_ID a, BDD_ID b){
        BDD_ID id = ite(a, 1, b);
        #ifdef UPDATE_LABEL
        updateNodeLabel(id, a, b, "or2");
        #endif

        return id;
    }

    /**
     * @brief creates a node that represents the XOR operation between other two nodes
     *
     * @param a operand
     * @param b operand
     * @return BDD_ID of the node which encapsulates the operation
     * @author Victor Herbert
     */
    BDD_ID Manager::xor2(BDD_ID a, BDD_ID b){
        BDD_ID id = ite(a,neg(b), b);
        #ifdef UPDATE_LABEL
        updateNodeLabel(id, a, b, "xor2");
        #endif

        return id;
    }

    /**
     * @brief creates a node that represents the negated version of a node
     *
     * @param a operand
     * @return BDD_ID of the node which encapsulates the operation
     * @author Victor Herbert
     */
    BDD_ID Manager::neg(BDD_ID a){
        BDD_ID id = ite(a, 0, 1);
        #ifdef UPDATE_LABEL
        if(nodes[id].label == "")
            nodes[id].label = "~" + nodes[a].label;
        #endif

        return id;
    }

    /**
     * @brief creates a node that represents the NAND operation between other two nodes
     *
     * @param a operand
     * @param b operand
     * @return BDD_ID of the node which encapsulates the operation
     * @author Victor Herbert
     */
    BDD_ID Manager::nand2(BDD_ID a, BDD_ID b){
        BDD_ID id = ite(a,neg(b), 1);
        #ifdef UPDATE_LABEL
        updateNodeLabel(id, a, b, "nand2");
        #endif

        return id;
    }

    /**
     * @brief creates a node that represents the NOR operation between other two nodes
     *
     * @param a operand
     * @param b operand
     * @return BDD_ID of the node which encapsulates the operation
     * @author Victor Herbert
     */
    BDD_ID Manager::nor2(BDD_ID a, BDD_ID b){
        BDD_ID id = ite(a, 0, neg(b));
        #ifdef UPDATE_LABEL
        updateNodeLabel(id, a, b, "nor2");
        #endif

        return id;
    }

    /**
     * @brief creates a node that represents the XNOR operation between other two nodes
     *
     * @param a operand
     * @param b operand
     * @return BDD_ID of the node which encapsulates the operation
     * @author Victor Herbert
     */
    BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b){
        BDD_ID id = ite(a,b, neg(b));
        #ifdef UPDATE_LABEL
        updateNodeLabel(id, a, b, "xnor2");
        #endif

        return id;
    }

    /**
     * @brief Breath first travel of the tree starting from node
     *
     * @param root node from which the traversal starts
     * @param order output vector of the traversal in postorder
     * @param marc vector cointaning which nodes have been visited
     *
     * @author Victor Herbert
     */
    void Manager::bfs(BDD_ID root, std::vector<BDD_ID> &order, std::vector<bool> &marc){
        if(!isConstant(root)){
            order.push_back(root);

            if(!marc[nodes[root].data.low]){
                marc[nodes[root].data.low] = true;
                bfs(nodes[root].data.low, order, marc);
            }

            if(!marc[nodes[root].data.high]){
                marc[nodes[root].data.high] = true;
                bfs(nodes[root].data.high, order, marc);
            }
        }
    }


    /**
     * @brief Creates an human readable visualization of the OBDD
     *
     * @param filepath of the output
     * @param root of the subtree to be printed
     *
     * Creates a Mermaid diagram of the desired subtree in a markdown file
     * @author Victor Herbert
     */
    void Manager::visualizeBDD(std::string filepath, BDD_ID &root){
        std::ofstream file;
        file.open(filepath);

        std::vector<bool> marc (uniqueTableSize(), false);
        std::vector<BDD_ID> order;
        order.reserve(uniqueTableSize());

        bfs(root, order, marc);

        file << "```mermaid\nstateDiagram-v2\n";

        for (BDD_ID node : order){
            file << node << " --> " << nodes[node].data.low << ": 0\n";
            file << node << " --> " << nodes[node].data.high << ": 1\n";
        }
        file << "classDef leaf fill:white\nclass 1 leaf\nclass 0 leaf\n```";

        file.close();
    }


    /**
    * @brief returns the id of the top variable of the given node f
    *
    * @param f ID of node representing function f
    * @return BDD_ID ID of top variable node
    * @author Kamel Fakih
    */
    BDD_ID Manager::topVar(BDD_ID f){
        return nodes[f].data.topVar;
    }

    /**
    * @brief returns the id of the high node of the given node f
    *
    * @param f ID of node representing function f
    * @return BDD_ID ID of top variable node
    * @author Kamel Fakih
    */
    BDD_ID Manager::low(BDD_ID f){
        return nodes[f].data.low;
    }

    /**
    * @brief returns the id of the low of the given node f
    *
    * @param f ID of node representing function f
    * @return BDD_ID ID of top variable node
    * @author Kamel Fakih
    */
    BDD_ID Manager::high(BDD_ID f){
        return nodes[f].data.high;
    }

    /**
    * @brief returns the NodeData of the low of the given node f
    *
    * @param f ID of node representing function f
    * @return BDD_ID ID of top variable node
    * @author Victor Herbert
    */
    NodeData Manager::nodeData(BDD_ID f){
        return nodes[f].data;
    }

    /**
    * @brief positive cofactor of the function f with respect to variable x
    *
    *
    * returns the positive cofactor of the function represented by ID f with respect to
    * the variable represented by ID x.
    *
    * @param f ID of node representing function f
    * @param x ID of node representing variable x
    * @return BDD_ID ID of the node representing the postive cofactor
    * @author Kamel Fakih
    */
    BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x){

        BDD_ID fTopVar = topVar(f);
        BDD_ID fHigh   = high(f);
        BDD_ID fLow    = low(f);

        if(isConstant(f) || isConstant(x) || fTopVar > x){
            return f;
        }
        if(fTopVar == x){
            return fHigh;
        }else{
            auto T = coFactorTrue(fHigh, x);
            auto F = coFactorTrue(fLow, x);
            return ite(fTopVar, T, F);
        }
    }

    /**
    * @brief positive cofactor of the function f with respect to its top variable
    *
    * calls coFactorTrue(BDD_ID f, BDD_ID x) with x as f's top variable
    * @param f ID of node representing function f
    * @return BDD_ID ID of the node representing the postive cofactor
    *
    * @author Kamel Fakih
    *
    */
    BDD_ID Manager::coFactorTrue(BDD_ID f){
        return coFactorTrue(f, topVar(f));
    }

    /**
    * @brief negative cofactor of the function f with respect to variable x
    *
    *
    * returns the negative cofactor of the function represented by ID f with respect to
    * the variable represented by ID x.
    * @param f ID of node representing function f
    * @param x ID of node representing variable x
    * @return BDD_ID ID of the node representing the negative cofactor
    * @author Kamel Fakih
    */
    BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x){

        BDD_ID fTopVar = topVar(f);
        BDD_ID fHigh   = high(f);
        BDD_ID fLow    = low(f);

        if(isConstant(f) || isConstant(x) || fTopVar > x){
            return f;
        }
        if(fTopVar == x){
            return fLow;
        }else{
            auto T = coFactorFalse(fHigh, x);
            auto F = coFactorFalse(fLow, x);
            return ite(fTopVar, T, F);
        }
    }

    /**
    * @brief negative cofactor of the function f with respect to its top variable
    *
    * calls coFactorFalse(BDD_ID f, BDD_ID x) with x as f's top variable
    *
    *  @param f ID of node representing function f
    * @return BDD_ID ID of the node representing the negative cofactor
    *
    * @author Kamel Fakih
    */
    BDD_ID Manager::coFactorFalse(BDD_ID f){
        return coFactorFalse(f, topVar(f));
    }

    /**
     * @brief returns the set of all nodes that are reachable from root node
     * including itself
     *
     *
     * uses DFS algorithm
     *
     * @param root
     * @param nodes_of_root
     * @author Kamel Fakih
     */
    void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root){
        std::unordered_set<BDD_ID> discovered = std::unordered_set<BDD_ID>();
        findNodesDFS(root, nodes_of_root, discovered);
    }

    void Manager::findNodesDFS(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root, std::unordered_set<BDD_ID> &discovered){
        if(discovered.count(root) != 0)
            return;
        
        discovered.insert(root);
        nodes_of_root.insert(root);

        findNodesDFS(low(root), nodes_of_root, discovered);
        findNodesDFS(high(root), nodes_of_root, discovered);
    }

    /**
     * @brief returns the set of all variables that are reachable from root node
     * including itself
     *
     *
     * uses DFS algorithm
     *
     * @param root
     * @param vars_of_root
     * @author Kamel Fakih
     */
    void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root){
        std::unordered_set<BDD_ID> discovered = std::unordered_set<BDD_ID>();
        findVarsDFS(root, vars_of_root, discovered);
    }

    void Manager::findVarsDFS(const BDD_ID &root, std::set<BDD_ID> &vars_of_root, std::unordered_set<BDD_ID> &discovered){

        if((discovered.count(root) != 0) || isConstant(root)){
            return;
        }

        vars_of_root.insert(topVar(root));
        discovered.insert(root);

        findVarsDFS(low(root), vars_of_root, discovered);
        findVarsDFS(high(root), vars_of_root, discovered);
    }
}