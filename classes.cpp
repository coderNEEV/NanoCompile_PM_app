#include <vector>
#include <string>
#include <unordered_map>

enum class DType
{
    Float16,
    Float32,
    Float64,
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    Bool
};

enum class NodeType
{
    Conv,
    ReLU,
    Add,
    Mul
};

class Tensor;
class Node;

class Tensor
{
private:
public:
    std::string name;
    std::vector<int> shape;
    Node *producer;
    std::vector<Node *> consumers;
    DType dtype;

    Tensor(std::string name,
           std::vector<int> shape,
           DType dtype)
        : name(name), shape(shape), dtype(dtype),  producer(nullptr)
    {
    }
};

class Node
{
private:
public:
    std::string name;
    NodeType type;
    std::vector<Tensor *> inputs;
    std::vector<Tensor *> outputs;
    // Attributes
    float execTime; // Time taken to operate in ms
    std::unordered_map<std::string, std::string> attributes; // Further Attributes depending on specific type of operator

    Node(std::string name, NodeType type) : name(name), type(type) {}
};

class Graph
{
private:
    /* data */
public:
    std::vector<Node *> operators;
    std::vector<Tensor *> tensors;
    std::vector<Tensor *> primary_inputs; // Tensors given to Graph from outside
    std::vector<Tensor *> final_outputs;  // Tensors given out by Graph with no consumers

    std::vector<std::vector<Node *>> timeline; // Used for paralleization representation and scheduling

    // Methods of node producers and consumers for dependency traversal
    std::vector<Node *> getNodeDependencies(Node &op)
    {
        std::vector<Node *> dependencies;
        for (auto tensor : op.inputs)
        {
            if (tensor->producer)
            {
                dependencies.push_back(tensor->producer);
            }
        }
        return dependencies;
    }

    std::vector<Node*> getConsumers(Node& op)
    {
        std::vector<Node *> consumers;
        for (auto tensor : op.outputs)
        {
            for (auto consumerNode : tensor->consumers)
            {
                consumers.push_back(consumerNode);
            }
        }
        return consumers;
    }

    void printGraph(){} // Implementing later

    Graph() {}
};

int main(void)
{

}