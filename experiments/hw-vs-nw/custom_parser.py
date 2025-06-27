import logging
import re

from lab.parser import Parser


class CommonParser(Parser):
    def add_repeated_pattern(
        self, name, regex, file="run.log", required=False, type=int
    ):
        def find_all_occurences(content, props):
            matches = re.findall(regex, content)
            if required and not matches:
                logging.error(f"Pattern {regex} not found in file {file}")
            props[name] = [type(m) for m in matches]

        self.add_function(find_all_occurences, file=file)

    def add_bottom_up_pattern(
        self, name, regex, file="run.log", required=False, type=int
    ):
        def search_from_bottom(content, props):
            reversed_content = "\n".join(reversed(content.splitlines()))
            match = re.search(regex, reversed_content)
            if required and not match:
                logging.error(f"Pattern {regex} not found in file {file}")
            if match:
                props[name] = type(match.group(1))

        self.add_function(search_from_bottom, file=file)


def get_parser():
    parser = CommonParser()
    parser.add_bottom_up_pattern(
        "plan_length",
        r"Plan length: (\d+) step\(s\)\.",
        type=int,
    )
    parser.add_bottom_up_pattern(
        "plan_cost",
        r"Plan cost: (\d+)",
        type=int,
    )
    parser.add_bottom_up_pattern(
        "expanded_states",
        r"Expanded (\d+) state\(s\)\.",
        type=int,
    )
    parser.add_bottom_up_pattern(
        "evaluated_states",
        r"Evaluated (\d+) state\(s\)\.",
        type=int,
    )
    parser.add_bottom_up_pattern(
        "generated_states",
        r"Generated (\d+) state\(s\)\.",
        type=int,
    )
    parser.add_bottom_up_pattern(
        "search_time",
        r"Search time: ([\d\.]+)s",
        type=float,
    )
    parser.add_bottom_up_pattern(
        "total_time",
        r"Total time: ([\d\.]+)s",
        type=float,
    )


    parser.add_bottom_up_pattern(
        "width_k",
        r"Found a solution with width_k = (\d+)",
        type=int,
    )



    return parser
