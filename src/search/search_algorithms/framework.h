#ifndef SEARCH_ALGORITHMS_FRAMEWORK_H
#define SEARCH_ALGORITHMS_FRAMEWORK_H

#include "../open_list.h"
#include "../search_algorithm.h"
#include "../utils/closed.h"
#include <memory>
#include <vector>
#include <optional>
#include "../utils/width_type.h" 

class Evaluator;
class PruningMethod;
class OpenListFactory;

namespace plugins {
class Feature;
}

namespace framework {

class Framework : public SearchAlgorithm {
    // rausnehmen
    const bool reopen_closed_nodes;
    Widthtype width_type;
    int width_k;
    std::unique_ptr<StateOpenList> open_list;
    std::unique_ptr<Closed> closed;
    std::optional<State> reference; // optional to delay initialization in constructor --> maybe a better way?

protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;
    bool hamming_progress_check(const State &candidate, const State &reference) const;
    bool novelty_progress_check(const State &candidate, const State &reference) const;
    
    bool hamming_expand_check(const State &candidate, Closed &closed, int k, const State &reference) const;
    bool novelty_expand_check(const State &candidate, Closed &closed, int k) const;
    
    void hamming_update_closed(const State &candidate, Closed &closed, int k) const;
    void novelty_update_closed(const State &candidate, Closed &closed, int k) const;

    bool progressCheck(const State &candidate, const State &reference);
    bool expand_check(const State &candidate, Closed &closed, int k, const State &reference);
    void updateClosed(const State &candidate, Closed &closed, int k);
    // Hamming Width helper functions
    int hamming_distance(const State &a, const State &b) const;



public:
    explicit Framework(
        const std::shared_ptr<OpenListFactory> &open,
        bool reopen_closed, 
        OperatorCost cost_type,
        int bound,
        double max_time,
        const std::string &description,
        utils::Verbosity verbosity,
        // interpretation des integers hinzufügen
        int width_type = 0,
        int width_k = 1);


    virtual void print_statistics() const override;

    void dump_ssrch_space() const;
};
//need it? (yes)
extern void add_framework_options_to_feature(
    plugins::Feature &feature, const std::string &description);
extern std::tuple<OperatorCost, int, double,
                  std::string, 
                  utils::Verbosity,
                  int, int>
get_framework_arguments_from_options(const plugins::Options &opts);
}




#endif
