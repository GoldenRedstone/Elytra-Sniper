#include <iostream>
#include <cinttypes>
#include <list>
#include <optional>
#include <cmath>
// #include "node.h"
// #include "edge.h"

class Node {
private:
    int64_t x_coord;
    int64_t z_coord;
    std::list<Node*> connections;
    bool isStart;
    bool hasShip;

public:
    Node(int64_t x_coord, int64_t z_coord, std::list<Node*>& connections, bool isStart, bool hasShip) :
    x_coord{x_coord}, z_coord{z_coord}, connections{connections}, isStart{isStart}, hasShip{hasShip}
    {    }

    Node() :
    x_coord{0}, z_coord{0}, connections{std::list<Node*> {}}, isStart{false}, hasShip{false}
    {    }

    int64_t get_x_coord() {
        return x_coord;
    }

    int64_t get_z_coord() {
        return z_coord;
    }

    bool get_isStart() {
        return isStart;
    }

    bool get_hasShip() {
        return hasShip;
    }

    double distance(Node* nextnode) {
        return pow(pow(get_x_coord() - nextnode->get_x_coord(), 2) + pow(get_z_coord() - nextnode->get_z_coord(), 2), 0.5);
    }
};

// class EdgeCompare {
// public:
//     bool operator() (Node* edge1, Node* edge2) {
//         return (edge1->weight < edge2->weight);
//     }
// };

class Frontier {
public:
    std::list<Node*> frontier;
    // EdgeCompare cmp;

    Frontier() :
    frontier{} //, cmp{}
    {    }

    bool isEmpty() {
        return frontier.empty();
    }

    std::optional<Node*> pop() {
        if (!isEmpty()) {
            Node* node = frontier.front();
            frontier.pop_front();
            return node;
        } else {
            std::nullopt;
        }
    }

    // void priosort() {
    //     frontier.sort(cmp);
    // };

    void add(Node* edge) {
        frontier.push_front(edge);
        // priosort();
    };

    
};

double heuristic() {
    return 0.0;
};

int main() {
    return 0;
};