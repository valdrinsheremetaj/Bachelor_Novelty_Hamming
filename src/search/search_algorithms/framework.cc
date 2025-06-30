#include "framework.h"

#include "../evaluation_context.h"
#include "../evaluator.h"
#include "../open_list_factory.h"
#include "../pruning_method.h"

#include "../algorithms/ordered_set.h"
#include "../plugins/options.h"
#include "../task_utils/successor_generator.h"
#include "../utils/logging.h"

#include <cassert>
#include <cstdlib>
#include <memory>
#include <optional>
#include <set>

using namespace std;

namespace framework {
    Framework::Framework(
        const shared_ptr<OpenListFactory> &open,  bool reopen_closed,
        OperatorCost cost_type,
        int bound,
        double max_time,
        const std::string &description,
        utils::Verbosity verbosity,
        int width_type,
        int width_k)
    : SearchAlgorithm(cost_type, bound, max_time, description, verbosity),
        reopen_closed_nodes(reopen_closed),
      open_list(open->create_state_open_list()),
      width_k(width_k){
        if (width_type == 0) {this->width_type = Widthtype::Hamming;}
        else if (width_type == 1) {this->width_type = Widthtype::Novelty;}
        else if (width_type == 2) {this->width_type = Widthtype::Hybrid;}
        else if (width_type == 3) {this->width_type = Widthtype::OR;}
        else {throw runtime_error("Invalid width type specified. Use 0 for Hamming, 1 for Novelty , 2 for Hybrid or 3 for OR.");}}

void Framework::initialize() {
    log << "Conducting width-based search"
        << (reopen_closed_nodes ? " with" : " without")
        << " reopening closed nodes, (real) bound = " << bound
        << endl;
    assert(open_list);

    reference = state_registry.get_initial_state();
    State initial_state = *reference;
    closed = std::make_unique<Closed>();
    updateClosed(initial_state, *closed, width_k);

    EvaluationContext eval_context(initial_state, 0, true, &statistics);
    statistics.inc_evaluated_states();

    SearchNode initial_node = search_space.get_node(initial_state);
    initial_node.open_initial();

    open_list->insert(eval_context, initial_state.get_id());

    print_initial_evaluator_values(eval_context);
}
    

void Framework::print_statistics() const {
    statistics.print_detailed_statistics();
    search_space.print_statistics();
}

SearchStatus Framework::step() {
    std::optional<SearchNode> node;
    // while open is not empty -> maybe while true around it
    if (open_list->empty()) {
        log << "Couldn't find a solution with width_k = " << width_k << endl;
        width_k++; // increase width_k if no solution found
        log << "Restarting search with increased width_k = " << width_k << endl;
        initialize(); // reinitialize search with new width_k
        
        return IN_PROGRESS;
    }

    // current = pop first element from open
    StateID sid = open_list->remove_min();
    State current = state_registry.lookup_state(sid);
    node.emplace(search_space.get_node(current));


    if (check_goal_and_set_plan(current))
        return SOLVED;


    // create successors after current state
    vector<OperatorID> applicable_ops;
    successor_generator.generate_applicable_ops(current, applicable_ops);

    // iterate through all successors
    for (OperatorID op_id : applicable_ops) {
        OperatorProxy op = task_proxy.get_operators()[op_id];
        // creation of candidate
        State candidate = state_registry.get_successor_state(current, op);
        statistics.inc_generated();

        SearchNode candidate_node = search_space.get_node(candidate);
        if (candidate_node.is_new()) {
            candidate_node.open_new_node(search_space.get_node(current), op, op.get_cost());
        }



        // check if candidate is a goal state
        if (check_goal_and_set_plan(candidate)) {
            log << "Found a solution with width_k = " << width_k << endl;
            return SOLVED;
        }

        // else check if there is progress made, according to width definition
        if (progressCheck(candidate, *reference)) {
            // insert candidate into open list
            EvaluationContext eval_context(candidate, 0, false, &statistics);
            statistics.inc_evaluated_states();
            open_list->insert(eval_context, candidate.get_id());

            closed->clear(); // clear closed list
            // update closed list with new candidate
            updateClosed(candidate, *closed, width_k);            
            // reference becomes candidate
            reference = candidate;
        }

        // should we expand the candidate?
        if (!expand_check(candidate, *closed, width_k, *reference))
            continue;
        
        // if expandCheck is true, we update the closed list
        updateClosed(candidate, *closed, width_k);

        // append candidate to open list
        EvaluationContext eval_context(candidate, 0, false, &statistics);
        statistics.inc_evaluated_states();
        open_list->insert(eval_context, candidate.get_id());
    }
    return IN_PROGRESS;
}

// computation of hamming distance: number of different variables between two states
int Framework::hamming_distance(const State &a, const State &b) const {
    assert(a.size() == b.size());
    int dist = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) {
            ++dist;
        }
    }
    return dist;
}

bool Framework::progressCheck(const State &candidate, const State &reference) {
    switch (width_type) {
        case Widthtype::Hamming:
            return hamming_progress_check(candidate, reference);

        case Widthtype::Novelty:
            return novelty_progress_check(candidate, reference);

        case Widthtype::Hybrid:
            return hamming_progress_check(candidate, reference) &&
                   novelty_progress_check(candidate, reference);

        case Widthtype::OR:
            return hamming_progress_check(candidate, reference) ||
                   novelty_progress_check(candidate, reference);

    }
}

void Framework::updateClosed(const State &candidate, Closed &closed, int k) {
    if (width_type == Widthtype::Hamming)
        hamming_update_closed(candidate, closed, k);
    else if (width_type == Widthtype::Novelty)
        novelty_update_closed(candidate, closed, k);
    else if (width_type == Widthtype::Hybrid || width_type == Widthtype::OR) {
        hamming_update_closed(candidate, closed, k);
        novelty_update_closed(candidate, closed, k);
    }
}



// if we expand a state, depends of if the state is already closed -> not needed and if candidates hamming distane is <= k, with respect to reference
bool Framework::expand_check(const State &candidate, Closed &closed, int k, const State &reference) {
    switch (width_type) {
        case Widthtype::Hamming:
            return hamming_expand_check(candidate, closed, k, reference);

        case Widthtype::Novelty:
            return novelty_expand_check(candidate, closed, k);

        case Widthtype::Hybrid:
            return hamming_expand_check(candidate, closed, k, reference) &&
                   novelty_expand_check(candidate, closed, k);

        case Widthtype::OR:
            return hamming_expand_check(candidate, closed, k, reference) ||
                   novelty_expand_check(candidate, closed, k);
    }
}


bool Framework::hamming_progress_check(const State &candidate, const State &reference) const {
    int wrong_candidate = 0, wrong_reference = 0;
    for (const FactProxy &goal_fact : task_proxy.get_goals()) {
        int var_id = goal_fact.get_variable().get_id();
        int goal_val = goal_fact.get_value();
        if (candidate[var_id].get_value() != goal_val) ++wrong_candidate;
        if (reference[var_id].get_value() != goal_val) ++wrong_reference;
    }
    return wrong_candidate < wrong_reference;
}

bool Framework::novelty_progress_check(const State &candidate, const State & reference) const {
    return false;
}

bool Framework::hamming_expand_check(const State &candidate, Closed &closed, int k, const State &reference) const {
    if (closed.is_closed(candidate))
        return false;
    return hamming_distance(candidate, reference) <= k;
}

bool Framework::novelty_expand_check(const State &candidate, Closed &closed, int k) const {
    bool found_novel = false;
    for (const PartialState &ps : generate_partial_states(candidate, k)) {
        if (ps.size() <= k && !closed.is_closed(ps)) {
            found_novel = true;
            closed.mark_closed(ps);
        }
    }
    return found_novel;
}

void Framework::hamming_update_closed(const State &candidate, Closed &closed, int /*k*/) const {
    closed.mark_closed(candidate);
}

void Framework::novelty_update_closed(const State &candidate, Closed &closed, int k) const {
    for (const PartialState &ps : generate_partial_states(candidate, k)) {
        if (ps.size() == k) {
            closed.mark_closed(ps);
        }
    }
}





void add_framework_options_to_feature(
    plugins::Feature &feature, const string &description) {
    add_search_pruning_options_to_feature(feature);
    // We do not add a lazy_evaluator options here
    // because it is only used for astar but not the other plugins.
    add_search_algorithm_options_to_feature(feature, description);
}

tuple<OperatorCost, int, double, string, utils::Verbosity, int, int>
get_framework_arguments_from_options(const plugins::Options &opts) {
    return tuple_cat(
        get_search_algorithm_arguments_from_options(opts),
        make_tuple(opts.get<int>("widthtype"),
        opts.get<int>("width_k"))
        );
}
}