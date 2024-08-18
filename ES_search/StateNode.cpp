#include <list>
#include <cinttypes>
#include <optional>
#include <cmath>
#include <queue>
#include <map>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <set>

const static int GOAL_NUM = 20; 
const static int EXPECTED_NODE_DISTANCE = 1400;

class StateNode {
public:
    int64_t x_coord;
    int64_t z_coord;
    std::map<StateNode*, double> connections;
    std::optional<StateNode*> parent;
    int num_visited;
    double path_cost;
    
public:
    StateNode(int64_t x_coord, int64_t z_coord, std::map<StateNode*, double> connections, std::optional<StateNode*> parent, int num_visited, double path_cost) :
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
        repr += " Parent coords: ";
        return repr;
    }

    int64_t get_x_coord() {
        return x_coord;
    }

    int64_t get_z_coord() {
        return z_coord;
    }  

    std::pair<int64_t, int64_t> get_position() {
        std::pair<int64_t, int64_t> pos {get_x_coord(), get_z_coord()};
        return pos;
    } 

    void add(StateNode* node) {
        connections.insert({node, this->distance(node)});
    }

    std::map<StateNode*, double> get_connections() {
        return connections;
    }

    double distance(StateNode* Parent) {
        if (Parent != nullptr) {
            return pow(pow(x_coord - Parent->x_coord , 2) + pow(z_coord - Parent->z_coord , 2) , 0.5);
        } else {
            return pow(pow(x_coord, 2) + pow(z_coord, 2) , 0.5);
        }
    };

    std::optional<StateNode*> get_parent() {
        return parent;
    }

    int g() {
    return num_visited;
    }

    // Heuristic

    // double h() {
    //     return GOAL_NUM - path_cost/EXPECTED_NODE_DISTANCE;
    // }

    // double f() {
    //     return g() + h();
    // }

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

// class ConnectionsMatrix {
//     double** adjMatrix;
//     int numVertices;

//     // ConnectionsMatrix() {
//     //     adjMatrix = new double*[rows];

//     // }
// };

class CompareNode {
public:
    bool operator()(StateNode* node1, StateNode* node2) {
        return node1->path_cost < node2->path_cost;
    }
};

std::vector<StateNode*> get_successors(StateNode* start) {
        // std::vector<StateNode*> successors;
        // CompareNode cmp;
        // auto firstElement = start->get_connections().begin();
        // for (; firstElement != start->get_connections().end(); firstElement++) {
        //     StateNode* child = firstElement->first;

        //     start->get_connections().erase(firstElement->first);

        //     child->parent = start;
        //     child->num_visited = start->num_visited + 1;
        //     child->path_cost = child->distance(start);

        //     if (std::find(successors.begin(), successors.end(), child) == successors.end()) {
        //         successors.push_back(child);
        //     }
        // }
        // return successors;
        std::vector<StateNode*> successors;
        for (const auto& [key, value] : start->get_connections()) {
            StateNode* child = key;
            child->parent = start;
            child->num_visited = start->num_visited + 1;
            child->path_cost = child->distance(start);
            successors.push_back(child);   
        }
        std::map<StateNode*, double> empty {{}};
        start->connections.clear();

        if (!successors.empty()){
            std::set<StateNode*> s {successors.begin(), successors.end()};
            successors.assign(s.begin(), s.end());
            successors.resize(s.size());
        }
        return successors;
}; // no dupes

std::unordered_set<std::string> get_ancestors_states(StateNode* node) {
    std::list<StateNode*> ancestors = node->get_path();
    ancestors.pop_front();
    std::unordered_set<std::string> ancestors_states;
    for (StateNode* node : ancestors) {
        ancestors_states.insert("" + std::to_string(node->get_x_coord()) + ", " + std::to_string(node->get_z_coord()));
    }
    return ancestors_states;
};

bool max_distance_exceeded(StateNode* start, int max_distance) {
    int distance_travelled = 0;
    for (StateNode* node : start->get_path()) {
        distance_travelled += node->path_cost;
    }
    return (distance_travelled > max_distance);
}


// std::list<StateNode*> a_star(StateNode* start, int max_distance) {
//     std::priority_queue<StateNode*, std::vector<StateNode*>, CompareNode> frontier;
//     start->path_cost = max_distance;
//     frontier.push(start);

//     std::map<std::string, std::pair<double, std::unordered_set<std::string>>> visited {{start->convert_to_string(), {start->g(), std::unordered_set<std::string>()}}};
//     std::map<std::list<StateNode*>, int> solution_set;
    

//     while (!frontier.empty()) {
//         StateNode* node = frontier.top();
//         frontier.pop();

//         if (node->g() >= GOAL_NUM) {
//             return node->get_path();
//         }

//         for (StateNode* child : get_successors(node)) {
//             if (child->path_cost < 0) {
//                 if (solution_set.size() < 6) {
//                     solution_set.insert({node->get_path(), node->num_visited});
//                 } else {
//                     break;
//                 }
//             }

//             std::string state = child->convert_to_string();
//             std::unordered_set<std::string> prevstates = get_ancestors_states(child);
//             int costval = child->g();
//             std::pair<double, std::unordered_set<std::string>> pairvalue {costval, prevstates};
//             bool inVisited = false;

//             for (const auto &myPair : visited) {

//                 if ((state.compare(myPair.first) == 0 && pairvalue.second == myPair.second.second && pairvalue.first > myPair.second.first )) { 
//                     visited[state].first = costval;
//                     // visited[state].second = pairvalue;
//                     inVisited = true;
//                 }
//             }
//             if (!inVisited) {
//                 visited.insert({state, pairvalue});
//                 frontier.push(child);
//             }
//         }
//     }


//         double max = 0;
//         std::cout << "sizeof sol " << solution_set.size() << std::endl;
//         std::list<StateNode*> max_path;
//         for (auto& [path, value] : solution_set) {
//             std::cout << "value: " << value << std::endl; 
//             if (max < solution_set[path]) {
//                 max = solution_set[path];
//                 max_path = path; // come back
//                 std::cout << "got here" << std::endl;
//                 for (const auto& i : max_path)
//                     std::cout << i << std::endl;
//             }
//         }

//         return max_path;
//         };


std::list<StateNode*> ucs(StateNode* start, int max_distance) {
    std::priority_queue<StateNode*, std::vector<StateNode*>, CompareNode> frontier;
    start->path_cost = 0;
    frontier.push(start);
    std::pair<std::unordered_set<std::string>, double> mapvalue = {get_ancestors_states(start), start->g()};
    std::map<std::string, std::pair<std::unordered_set<std::string>, double>> visited {{start->convert_to_string(), mapvalue}};
    std::map<std::list<StateNode*>, int> solution_set;

    int max_depth = 2;

    while (!frontier.empty()) {
        StateNode* node = frontier.top();
        frontier.pop();
        if (node->g() <= max_depth) {

            if (node->g() >= GOAL_NUM) {
                return node->get_path();
            } 

            for (StateNode* child : get_successors(node)) {
                std::string state = child->convert_to_string();
                bool inVisited = false;
                if (max_distance_exceeded(child, max_distance)) {
                    solution_set.insert({node->get_path(), node->g()});
                } else {
                    auto search = visited.find(state);
                    if (search != visited.end()) {
                        inVisited = true;
                        std::unordered_set<std::string> prevstates = get_ancestors_states(child);
                        if ((search->second.first == prevstates) && (search->second.second > child->path_cost)) {
                            std::pair<std::unordered_set<std::string>, double> mapvalue = {prevstates, child->path_cost};
                            visited[state] = mapvalue;
                        } else if (search->second.first != prevstates && search->second.first.size() < prevstates.size()) {
                            std::pair<std::unordered_set<std::string>, double> mapvalue = {prevstates, child->path_cost};
                            visited[state] = mapvalue;
                        } 
                    } else if (!inVisited) {
                        std::unordered_set<std::string> prevstates = get_ancestors_states(child);
                        std::pair<std::unordered_set<std::string>, double> mapvalue = {prevstates, child->path_cost};
                        visited.insert({child->convert_to_string(), mapvalue});
                    }
                    frontier.push(child);
                    std::unordered_set<StateNode*> dupeprevent;
                    while(!frontier.empty()) {
                        StateNode* duper = frontier.top();
                        dupeprevent.insert(duper);
                        frontier.pop();
                    }
                    for (StateNode* node : dupeprevent) {
                        frontier.push(node);
                    }
                    // std::cout << frontier.size() << std::endl;
                }
            }
        } else {
            return node->get_path();
            break;
        }
    }
    std::cout << solution_set.size() << std::endl;
    int max_num_visited;
    auto solpair = solution_set.begin();
    auto max_path = std::list<StateNode*>();

    for (;solpair != solution_set.end(); ++solpair) {
        if (solpair->second > max_num_visited) {
            max_num_visited = solpair->second;
            max_path = solpair->first;
            // std::cout << max_path.size() << std::endl;
        }
    }

    return max_path;
}

int main() {

    StateNode a {-256, -4112};
    StateNode b {-4464, -3824};
    StateNode c {-304, -3776};
    StateNode d {-3152, -2864};

    a.add(&b);
    a.add(&c);
    a.add(&d);
    b.add(&c);
    b.add(&d);
    c.add(&b);
    c.add(&d);
    d.add(&b);
    d.add(&c);

    // d.path_cost;

    // std::vector<StateNode*> vec = get_successors(&c);
    // std::cout << vec.size() << std::endl;


    // d.parent = &c;
    // c.parent = &b;
    // b.parent = &a;

    std::list<StateNode*> path = ucs(&a, 6000);
    std::cout << path.size() << std::endl;
    for (StateNode* node : path) {
        // std::cout << "Here!" << std::endl;
        std::cout << node->get_x_coord() << std::endl;
    }

    // for (StateNode* node : start.get_connections()) {

    // }

    // std::list<StateNode*> path = c.get_path();
    // for (StateNode* node : path) {
    //     std::cout << node->get_x_coord() << std::endl;
    // }    

    return 0;
};