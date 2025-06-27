#! /usr/bin/env python

from pathlib import Path

import project

from lab.experiment import Experiment

ATTRIBUTES = [
    "error",
    "run_dir",
    "planner_time",
    "initial_h_value",
    "coverage",
    "cost",
    "evaluations",
    "memory",
    "width_k",      
    project.EVALUATIONS_PER_TIME,
]

exp = Experiment()
exp.add_step(
    "remove-combined-properties", project.remove_file, Path(exp.eval_dir) / "properties"
)

project.fetch_algorithms(exp, "hwvsnw.py", "hw-vs-nw", "fw-h1" ,"fw-hw2")

filters = [project.add_evaluations_per_time]

project.add_absolute_report(
    exp, attributes=ATTRIBUTES, filter=filters, name=f"{exp.name}"
)


exp.run_steps()
