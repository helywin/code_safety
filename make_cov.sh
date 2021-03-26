#!/usr/bin/env bash

COVERAGE_FILE=coverage.info
REPORT_FOLDER=coverage_report
lcov --rc lcov_branch_coverage=1 -c -d cmake-build-debug/CMakeFiles/test_gcov.dir -o ${COVERAGE_FILE}
genhtml --rc genhtml_branch_coverage=1 ${COVERAGE_FILE} -o ${REPORT_FOLDER}
