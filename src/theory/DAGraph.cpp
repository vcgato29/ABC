/*
 * DAGraph.cpp
 *
 *  Created on: Sep 16, 2015
 *      Author: baki
 */

#include <stack>
#include "DAGraph.h"

namespace Vlab {
namespace Theory {

int DAGraph::name_counter = 0;

DAGraph::DAGraph() :
      graph (nullptr), startNode (nullptr), sinkNode (nullptr) {
}

DAGraph::DAGraph(Graph_ptr graph) {
  this->graph = graph;
  startNode = sinkNode = nullptr;

  int num_of_vertices = graph->getNumOfNodes();
  int *disc = new int[num_of_vertices];
  int *low = new int[num_of_vertices];
  bool *is_stack_member = new bool[num_of_vertices];
  std::stack<int> *st = new std::stack<int>();
  int time = 0;

  for (int i = 0; i < num_of_vertices; i++) {
    disc[i] = -1;
    low[i] = -1;
    is_stack_member[i] = false;
  }

  for (int i = 0; i < num_of_vertices; i++) {
    if (disc[i] == -1){
      findSCCs(i, disc, low, st, is_stack_member, time);
    }
  }

  for (auto& entry : nodes) {
    DAGraphNode_ptr da_node = entry.second;
    for (GraphNode_ptr sub_node: da_node->getNextSubNodes()) {
      da_node->addNextNode(subNodes[sub_node]);
    }
    for (GraphNode_ptr sub_node: da_node->getPrevSubNodes()) {
      da_node->addPrevNode(subNodes[sub_node]);
    }
  }

  delete disc;
  delete low;
  delete is_stack_member;
  delete st;
}

DAGraph::~DAGraph() {

}

void DAGraph::setStartNode(DAGraphNode_ptr scc_node) {
  startNode = scc_node;
}

DAGraphNode_ptr DAGraph::getStartNode() {
  return startNode;
}

void DAGraph::setSinkNode(DAGraphNode_ptr scc_node) {
  sinkNode = scc_node;
}

DAGraphNode_ptr DAGraph::getSinkNode() {
  return sinkNode;
}

DAGraphNode_ptr DAGraph::getNode(int id) {
  DAGraphNode_ptr node = nullptr;
  auto it = nodes.find(id);
  if (it != nodes.end()) {
    node = it->second;
  }
  return node;
}

bool DAGraph::addNode(DAGraphNode_ptr scc_node) {
  auto result = nodes.insert(std::make_pair(scc_node->getID(), scc_node));
  return result.second;
}

void DAGraph::addFinalNode(DAGraphNode_ptr scc_node) {
  finalNodes.insert(scc_node);
  addNode(scc_node);
}

DAGraphNodeSet& DAGraph::getFinalNodes() {
  return finalNodes;
}

DAGraphNodeMap& DAGraph::getNodeMap() {
  return nodes;
}

void DAGraph::removeNode(DAGraphNode_ptr scc_node) {
  nodes.erase(scc_node->getID());
  finalNodes.erase(scc_node);
  for (auto prev_node : scc_node->getPrevNodes()) {
    prev_node->getNextNodes().erase(scc_node);
  }
}

Graph_ptr DAGraph::getRawGraph() {
  return graph;
}

void DAGraph::resetFinalNodes(DAGraphNodeSet& nodes) {
  finalNodes.clear();
  finalNodes.insert(nodes.begin(), nodes.end());
  for (auto scc_node : finalNodes) {

  }
}

void DAGraph::toDot(bool print_sink, std::ostream& out) {
  print_sink = print_sink || (nodes.size() == 1 and finalNodes.size() == 0);
  out << "digraph MONA_DFA {\n"
      " rankdir = LR;\n "
      " center = true;\n"
      " size = \"700.5,1000.5\";\n"
      " edge [fontname = Courier];\n"
      " node [height = .5, width = .5];\n"
      " node [shape = doublecircle];";

  for (auto& node : finalNodes) {
      out << " " << node->getID() << ";";
  }

  out << "\n node [shape = circle];";

  for (auto& entry : nodes) {
    if ((not print_sink) && sinkNode == entry.second) {
      continue;
    }
    out << " " << entry.first << ";";
  }

  out << "\n init [shape = plaintext, label = \"\"];\n" <<
      " init -> " << startNode->getID() << ";\n";

  for (auto& entry : nodes) {
    if ((not print_sink) && sinkNode == entry.second) {
      continue;
    }
    for (auto& next_node : entry.second->getNextNodes()) {
      if ((not print_sink) && sinkNode == next_node) {
        continue;
      }
      out << " " << entry.first << " -> " << next_node->getID();
      int node_flag = entry.second->getFlag();
      if ( node_flag != 0 && next_node != sinkNode) {
        out << "[label = \"" << node_flag << "\"]";
      }
      out << ";\n";
    }
  }
  out << "}" << std::endl;
}

void DAGraph::inspectGraph(bool print_sink) {
  std::stringstream file_name;
  file_name << "./output/inspect_dagraph_" << name_counter++ << ".dot";
  std::string file = file_name.str();
  std::ofstream outfile(file.c_str());
  if (!outfile.good()) {
    std::cout << "cannot open file: " << file_name << std::endl;
    exit(2);
  }
  toDot(print_sink, outfile);
  std::string dot_cmd("xdot " + file + " &");
  std::system(dot_cmd.c_str());
}

void DAGraph::findSCCs(int u, int disc[], int low[], std::stack<int>* st, bool is_stack_member[], int& time) {
  disc[u] = low[u] = ++time;
  st->push(u);
  is_stack_member[u] = true;

  GraphNode_ptr current_node = graph->getNode(u);
  for (GraphNode_ptr next_node : current_node->getNextNodes()) {
    int v = next_node->getID();
    if (disc[v] == -1) {
      findSCCs(v, disc, low, st, is_stack_member, time);
      low[u] = std::min(low[u], low[v]);
    } else if (is_stack_member[v] == true) {
      low[u] = std::min(low[u], disc[v]);
    }
  }

  int w = 0;
  if (low[u] == disc[u]) {
    DAGraphNode_ptr da_current_node = new DAGraphNode(u);
    GraphNode_ptr sub_node = nullptr;
    bool added = false;
    while (st->top() != u) {
      w = st->top();
      sub_node = graph->getNode(w);
      da_current_node->addSubNode(sub_node);
      subNodes[sub_node] = da_current_node;
      is_stack_member[w] = false;
      st->pop();

      if (graph->isStartNode(sub_node)) {
        startNode = da_current_node;
      }

      if (graph->isSinkNode(sub_node)) {
        sinkNode = da_current_node;
      }

      if (graph->isFinalNode(sub_node)) {
        addFinalNode(da_current_node);
        added = true;
      } else if(not added) {
        addNode(da_current_node);
        added = true;
      }
    }
    w = st->top();
    sub_node = graph->getNode(w);
    da_current_node->addSubNode(sub_node);
    subNodes[sub_node] = da_current_node;
    is_stack_member[w] = false;
    st->pop();
    if (graph->isStartNode(sub_node)) {
      startNode = da_current_node;
    }

    if (graph->isSinkNode(sub_node)) {
      sinkNode = da_current_node;
    }

    if (graph->isFinalNode(sub_node)) {
      addFinalNode(da_current_node);
      added = true;
    } else if(not added) {
      addNode(da_current_node);
      added = true;
    }
  }
}

} /* namespace Theory */
} /* namespace Vlab */