#ifndef TRIE_HPP
#define TRIE_HPP

#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "planner.hpp"

struct TrieNode {
  std::unordered_map<size_t, TrieNode *> children;
  std::vector<VecOperator> applicable_actions;

  TrieNode() : children(std::unordered_map<size_t, TrieNode *>()) {}
  ~TrieNode() {
    for (auto &kv : children)
      delete kv.second;
  }
};

class Trie {
public:
  Trie() { root = new TrieNode(); }
  ~Trie() { delete root; }

  void initialize(const Task &task) {
    for (auto op : task.operators) {
      TrieNode *curr = root;

      for (auto precond : op.preconditions) {
        if (curr->children.find(precond) == curr->children.end()) {
          curr->children[precond] = new TrieNode();
        }
        curr = curr->children[precond];
      }
      curr->applicable_actions.push_back(op);
    }
  }

  std::vector<std::pair<VecOperator, std::vector<bool>>>
  get_successor_states(const std::vector<bool> &state) const {
    std::vector<std::pair<VecOperator, std::vector<bool>>> ret;
    std::vector<TrieNode *> reachable_nodes;
    std::vector<VecOperator> applicables;

    reachable_nodes.push_back(root);

    for (size_t i = 0; i < state.size(); ++i) {
      if (!state[i])
        continue;
      std::vector<TrieNode *> add_nodes;
      for (auto reachable_node : reachable_nodes) {
        if (reachable_node->children.find(i) != reachable_node->children.end())
          add_nodes.push_back(reachable_node->children[i]);
      }
      reachable_nodes.insert(reachable_nodes.end(), add_nodes.begin(),
                             add_nodes.end());
    }

    for (auto reachable_node : reachable_nodes) {
      for (auto op : reachable_node->applicable_actions) {
        ret.push_back(std::make_pair(op, op.apply(state)));
      }
    }

    return ret;
  }

private:
  TrieNode *root;
};

#endif
