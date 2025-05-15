#ifndef SEARCH_ALGORITHMS_FRAMEWORK_H
#define SEARCH_ALGORITHMS_FRAMEWORK_H

#include "../open_list.h"
#include "../search_algorithm.h"
#include "../utils/closed.h"
#include <memory>
#include <vector>
#include <optional>

class Evaluator;
class PruningMethod;
class OpenListFactory;

namespace plugins {
class Feature;
}

namespace framework {
    // enumeration for the type of width used in the search
    enum class Widthtype {
        NONE,
        Hamming,
        Novelty
    };
class Framework : public SearchAlgorithm {
    const bool reopen_closed_nodes;
    Widthtype width_type;
    int width_k;
    std::unique_ptr<StateOpenList> open_list;
    std::unique_ptr<ClosedList> closed;
    std::optional<State> reference; // optional to delay initialization in constructor --> maybe a better way?

    


protected:
    virtual void initialize() override;
    virtual SearchStatus step() override;
    bool progressCheck(const State &candidate, const State &reference) const;
    bool expandCheck(const State &candidate, const ClosedList &closed, int k) const;
    void updateClosed(const State &s, ClosedList &closed, int k) const;
    // Hamming Width helper functions
    int hamming_distance(const State &a, const State &b);
    bool hamming_progress_check(const State &candidate, const State &reference);
    // reference? should be needed?
    bool hamming_expand_check(const State &candidate, const ClosedList &closed, int k);
    void hamming_update_closed(const State &s, ClosedList &closed, int k);

public:
    explicit Framework(
        const std::shared_ptr<OpenListFactory> &open,
        bool reopen_closed, 
        OperatorCost cost_type,
        int bound,
        double max_time,
        const std::string &description,
        utils::Verbosity verbosity,
        Widthtype width_type = Widthtype::NONE,
        int width_k = 1);


    virtual void print_statistics() const override;

    void dump_search_space() const;
};
//need it? (yes)
extern void add_eager_search_options_to_feature(
    plugins::Feature &feature, const std::string &description);
extern std::tuple<std::shared_ptr<PruningMethod>,
                  std::shared_ptr<Evaluator>, OperatorCost, int, double,
                  std::string, utils::Verbosity>
get_eager_search_arguments_from_options(const plugins::Options &opts);
}




#endif
