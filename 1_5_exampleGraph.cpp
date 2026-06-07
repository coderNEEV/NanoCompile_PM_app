#include <vector>
#include <string>
#include <unordered_map>
#include <ranges>
#include <algorithm>
#include <iostream>

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

std::string nodeTypeToString(NodeType type)
{
    switch (type)
    {
    case NodeType::Conv:
        return "Conv";
    case NodeType::ReLU:
        return "ReLU";
    case NodeType::Add:
        return "Add";
    case NodeType::Mul:
        return "Mul";
    }
    return "Unknown";
}

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
        : name(name), shape(shape), dtype(dtype), producer(nullptr)
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
    float execTime; // Time taken to operate in ms
    std::unordered_map<std::string, std::string> attributes; // Further Attributes depending on specific type of operator
    int level;

    Node(std::string name, NodeType type) : name(name), type(type), level(-1) {}
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
    std::vector<Node *> getNodeDependencies(Node *op)
    {
        std::vector<Node *> dependencies;
        for (auto tensor : op->inputs)
        {
            if (tensor->producer)
            {
                dependencies.push_back(tensor->producer);
            }
        }
        return dependencies;
    }

    std::vector<Node *> getConsumers(Node &op)
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

    // Traversing graph by dependencies and building structure and timeline
    void generateGraphTimeline()
    {
        // Find level 0 nodes
        std::vector<Node *> level0;
        for (auto op : this->operators)
        {
            if (this->getNodeDependencies(op).empty())
            {
                op->level = 0;
                level0.push_back(op);
            }
        }

        int assignedNodes = level0.size();

        while (assignedNodes < this->operators.size())
        {

            for (auto *op : this->operators)
            {
                if (op->level != -1) // already assigned
                    continue;

                auto deps = getNodeDependencies(op);

                bool allKnown = true;
                int maxLevel = -1;

                for (auto *dep : deps)
                {
                    if (dep->level == -1)
                    {
                        allKnown = false;
                        break;
                    }

                    maxLevel = std::max(maxLevel, dep->level);
                }

                if (allKnown)
                {
                    op->level = maxLevel + 1;
                    assignedNodes++;
                }
            }
        }

        for (auto *op : operators)
        {
            timeline.resize(
                std::max(
                    timeline.size(),
                    static_cast<size_t>(op->level + 1)));

            timeline[op->level].push_back(op);
        }
    }

    void graphTimelinePrint()
    {
        int index = 0;
        for (auto level : this->timeline)
        {
            std::cout << "Level " << index << " : ";
            for (auto *node : level)
            {
                std::cout << node->name << " ";
            }
            std::cout << std::endl;
            index++;
        }
    }

    void graphSchedulePrint()
    {
        std::cout << "=========================\n";
        std::cout << "Graph Schedule\n";
        std::cout << "=========================\n\n";

        for (size_t levelIdx = 0; levelIdx < timeline.size(); levelIdx++)
        {
            std::cout << "Level " << levelIdx << "\n";

            for (Node *node : timeline[levelIdx])
            {
                std::cout << "  " << node->name << "(" << nodeTypeToString(node->type) << ")\n";
                std::cout << "    Inputs:\n";

                for (Tensor *input : node->inputs)
                {
                    std::cout << "      "
                              << input->name;

                    if (input->producer)
                    {
                        std::cout << " <- "
                                  << input->producer->name;
                    }
                    else
                    {
                        std::cout << " <- External";
                    }

                    std::cout << "\n";
                }

                std::cout << "    Outputs:\n";

                for (Tensor *output : node->outputs)
                {
                    std::cout << "      "
                              << output->name
                              << "\n";
                }

                std::cout << "\n";
            }
        }
    }

    void graphPrint()
    {
        std::cout << "Graph Structure\n";
        std::cout << "===============\n\n";

        for (Node *node : operators)
        {
            auto consumers = getConsumers(*node);

            for (Node *consumer : consumers)
            {
                std::cout
                    << node->name
                    << " -> "
                    << consumer->name
                    << '\n';
            }
        }
    }

    Graph() {}
};

int main(void)
{
    // Manual hardcoded graph population

    Node n1("n1", NodeType::Conv);
    Node n2("n2", NodeType::ReLU);
    Node n3("n3", NodeType::Conv);
    Node n4("n4", NodeType::Conv);
    Node n5("n5", NodeType::Add);
    Node n6("n6", NodeType::ReLU);
    Node n7("n7", NodeType::Mul);
    Node n8("n8", NodeType::Add);

    std::vector<int> dummyShape{1, 1};

    Tensor x("x", dummyShape, DType::Float32);
    Tensor w1("w1", dummyShape, DType::Float32);
    Tensor w2("w2", dummyShape, DType::Float32);
    Tensor w_skip("w_skip", dummyShape, DType::Float32);
    Tensor scale("scale", dummyShape, DType::Float32);
    Tensor bias("bias", dummyShape, DType::Float32);

    Graph graph1;
    graph1.primary_inputs.push_back(&x);
    graph1.primary_inputs.push_back(&w1);
    graph1.primary_inputs.push_back(&w2);
    graph1.primary_inputs.push_back(&w_skip);
    graph1.primary_inputs.push_back(&scale);
    graph1.primary_inputs.push_back(&bias);

    x.consumers.push_back(&n1);
    x.consumers.push_back(&n4);

    w1.consumers.push_back(&n1);
    w2.consumers.push_back(&n3);

    w_skip.consumers.push_back(&n4);

    scale.consumers.push_back(&n7);
    bias.consumers.push_back(&n8);

    for (auto tensor : graph1.primary_inputs)
    {
        graph1.tensors.push_back(tensor);
    }

    graph1.operators.push_back(&n1);
    graph1.operators.push_back(&n2);
    graph1.operators.push_back(&n3);
    graph1.operators.push_back(&n4);
    graph1.operators.push_back(&n5);
    graph1.operators.push_back(&n6);
    graph1.operators.push_back(&n7);
    graph1.operators.push_back(&n8);

    // Step wise manual population instead of using connection
    n1.inputs.push_back(&x);
    n1.inputs.push_back(&w1);
    Tensor a("a", dummyShape, DType::Float32);
    a.producer = &n1;
    a.consumers.push_back(&n2);
    graph1.tensors.push_back(&a);
    n1.outputs.push_back(&a);

    n2.inputs.push_back(&a);
    Tensor b("b", dummyShape, DType::Float32);
    b.producer = &n2;
    b.consumers.push_back(&n3);
    graph1.tensors.push_back(&b);
    n2.outputs.push_back(&b);

    n3.inputs.push_back(&b);
    Tensor c("c", dummyShape, DType::Float32);
    c.producer = &n3;
    c.consumers.push_back(&n5);
    graph1.tensors.push_back(&c);
    n3.outputs.push_back(&c);

    n4.inputs.push_back(&x);
    n4.inputs.push_back(&w_skip);
    Tensor s("s", dummyShape, DType::Float32);
    s.producer = &n4;
    s.consumers.push_back(&n5);
    graph1.tensors.push_back(&s);
    n4.outputs.push_back(&s);

    n5.inputs.push_back(&c);
    n5.inputs.push_back(&s);
    Tensor d("d", dummyShape, DType::Float32);
    d.producer = &n5;
    d.consumers.push_back(&n6);
    graph1.tensors.push_back(&d);
    n5.outputs.push_back(&d);

    n6.inputs.push_back(&d);
    Tensor e("e", dummyShape, DType::Float32);
    e.producer = &n6;
    e.consumers.push_back(&n7);
    graph1.tensors.push_back(&e);
    n6.outputs.push_back(&e);

    n7.inputs.push_back(&e);
    n7.inputs.push_back(&scale);
    Tensor f("f", dummyShape, DType::Float32);
    f.producer = &n7;
    f.consumers.push_back(&n8);
    graph1.tensors.push_back(&f);
    n7.outputs.push_back(&f);

    n8.inputs.push_back(&f);
    n8.inputs.push_back(&bias);
    Tensor y("y", dummyShape, DType::Float32);
    y.producer = &n8;
    graph1.tensors.push_back(&y);
    n8.outputs.push_back(&y);
    graph1.final_outputs.push_back(&y);

    // Different analysis methods explained in app
    graph1.generateGraphTimeline();
    // graph1.graphTimelinePrint();
    // graph1.graphSchedulePrint();
    graph1.graphPrint();
}
