#include "state_partial.h"
#include <numeric>

#include "state_partial.h"
#include <numeric>

/*Function that generates all subsets of a State, by taking a State, the Variable IDs as an 
Input and indexes through the state recursively. Creating every possible non-empty subset,
of a State.*/
void generate_subsets(
    const State &s,
    const std::vector<int> &vars,
    int index,
    std::vector<int> &current,
    std::vector<PartialState> &result) {
    
    // if current not empty --> we have a valid subset, save it in result
    if (!current.empty()) {
        result.emplace_back(s, current); 
    }
    // if we reached the end, stop the recursion
    if (index >= vars.size())
        return;

    // Include current index
    current.push_back(vars[index]);
    // Recur with the next index --> we get every subset that contain current variable
    generate_subsets(s, vars, index + 1, current, result);

    // Exclude it, after saved and continue
    current.pop_back();
    generate_subsets(s, vars, index + 1, current, result);
}


std::vector<PartialState> generate_partial_states(const State &s) {
    std::vector<PartialState> result;
    std::vector<int> all_vars(s.size());
    std::iota(all_vars.begin(), all_vars.end(), 0);
    std::vector<int> current;
    generate_subsets(s, all_vars, 0, current, result);
    return result;
}
