#ifndef PARTIAL_STATE_H
#define PARTIAL_STATE_H

#include "task_proxy.h"
#include <vector>
#include <utility>
#include <algorithm>

class PartialState {
private:
    std::vector<std::pair<int, int>> var_values;

public:

    PartialState(const State &s, const std::vector<int> &var_subset) {
        for (size_t i = 0; i < var_subset.size(); ++i) {
            int var_id = var_subset[i]; // loop over all variable IDs we want to include
            int value = s[var_id].get_value(); // get their value in the state
            var_values.push_back(std::make_pair(var_id, value)); // store the variable ID and its value

        }
        std::sort(var_values.begin(), var_values.end()); // sort the vector by variable ID --> nedeed to ctually compare partial states equality
    }

    // Equality operator for comparing two partial states. Don't know if i'll actually need it.
    bool operator==(const PartialState &other) const {
        return var_values == other.var_values;
    }
    
    int size() const {
        return var_values.size();
    }
    

};
std::vector<PartialState> generate_partial_states(const State &s);


#endif
