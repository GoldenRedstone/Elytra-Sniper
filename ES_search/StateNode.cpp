#include <list>
#include <cinttypes>
#include <optional>
#include <cmath>
#include <queue>
#include <map>
#include <iostream>
#include <algorithm>

const static int GOAL_NUM = 20; 
const static int EXPECTED_NODE_DISTANCE = 1400;

class StateNode {
public:
    int64_t x_coord;
    int64_t z_coord;
    std::vector<StateNode*> connections;
    std::optional<StateNode*> parent;
    int num_visited;
    double path_cost;
    
public:
    StateNode(int64_t x_coord, int64_t z_coord, std::vector<StateNode*> connections, std::optional<StateNode*> parent, int num_visited, double path_cost) :
    x_coord(x_coord), z_coord(z_coord), connections(connections), parent(parent), num_visited(num_visited), path_cost(path_cost)
    {    }

    StateNode(int64_t x_coord, int64_t z_coord) :
    x_coord(x_coord), z_coord(z_coord), connections(), parent(nullptr), num_visited(0), path_cost(0) 
    {    }

    std::string convert_to_string() {
        std::string repr = "";
        repr += std::to_string(x_coord);
        repr += ", ";
        repr += std::to_string(z_coord);
        return repr;
    }

    int64_t get_x_coord() {
        return x_coord;
    }

    int64_t get_z_coord() {
        return z_coord;
    }

    void add(StateNode* node) {
        connections.push_back(node);
    }

    std::vector<StateNode*> get_connections() {
        return connections;
    }

    double distance(StateNode* next) {
        return pow(pow(x_coord - next->x_coord , 2) + pow(z_coord - next->z_coord , 2) , 0.5);
    }

    std::optional<StateNode*> get_parent() {
        return parent;
    }

    int g() {
        return num_visited;
    }

    double h() {
        return GOAL_NUM - path_cost/EXPECTED_NODE_DISTANCE - num_visited;
    }

    double f() {
        return g() + h();
    }

    std::list<StateNode*> get_path() {
        std::list<StateNode*> path {};
        StateNode* cur = this;

        while (cur != nullptr) {
            path.push_back(cur);
            if (cur->get_parent().has_value()) {
                cur = cur->get_parent().value();
            } else {
                break;
            }
        }

        return path;
    }
};

class ConnectionsMatrix {
    double** adjMatrix;
    int numVertices;

    // ConnectionsMatrix() {
    //     adjMatrix = new double*[rows];

    // }
};

class CompareNode {
public:
    bool operator()(StateNode* node1, StateNode* node2) {
        return node1->f() < node2->f();
    }
};

std::vector<StateNode*> get_successors(StateNode* start) {
        std::vector<StateNode*> successors;
        CompareNode cmp;
        // std::cout << std::ceil(start->get_connections().size() / (static_cast<double>(2))) << std::endl;
        // std::cout << start->get_connections().size() << std::endl;

        for (int i = 0; i < std::ceil(start->get_connections().size() / (static_cast<double>(2))); i++) {
            StateNode* child = start->get_connections().at(i);
            child->parent = start;
            child->num_visited = start->num_visited + 1;
            child->path_cost = start->distance(child);
            successors.push_back(child);
        }
        // successors.shrink_to_fit();
        return successors;
};



std::list<StateNode*> a_star(StateNode* start, int max_distance) {
    std::priority_queue<StateNode*, std::vector<StateNode*>, CompareNode> frontier;
    frontier.push(start);
    std::map<std::string, double> visited {{start->convert_to_string(), start->f()}};

    while (!frontier.empty()) {
        StateNode* node = frontier.top();
        frontier.pop();

        if (node->g() >= GOAL_NUM) {
            return node->get_path();
        }

        // visited[node] =  

    };

    std::list<StateNode*> path{};
    return path;
};

int main() {

    StateNode a {-256, -4112};
    StateNode b {-4464, -3824};
    StateNode c {-304, -3776};
    StateNode d {-3152, -2864};

    a.add(&b);
    a.add(&c);
    a.add(&d);
    b.add(&a);
    b.add(&c);
    b.add(&d);
    c.add(&a);
    c.add(&b);
    c.add(&d);
    d.add(&b);
    d.add(&c);
    d.add(&a);

    // for (StateNode* node : a.get_connections()) {
    //     if (node->get_x_coord() == -3152) {
    //         std::list<StateNode*> path = node->get_path();
    //         // std::cout << path.front()->get_x_coord();
    //         for (StateNode* node : path) {
    //             std::cout << node->get_x_coord();
    //         }
    //         // std::cout << static_cast<int>(node->get_parent().has_value()) << std::endl;
    //         // std::cout << static_cast<int>(!(node->get_parent().has_value())) << std::endl;
    //     }
    // }

    for (StateNode* node : get_successors(&a)) {
        // std::cout << node->convert_to_string() << std::endl;
        std::cout << node->x_coord << std::endl << node->z_coord << std::endl << node->get_connections().front() << std::endl 
                  << node->get_parent().value()->get_x_coord() << std::endl << node->num_visited << std::endl << node->path_cost << std::endl;
    }
    std::cout << get_successors(&a).size();
    // std::cout << get_successors(&a).front()->convert_to_string();

    return 0;
};