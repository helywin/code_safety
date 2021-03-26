#!/usr/bin/env bash

BINARY_DIR=$1
COVERAGE_FILE=coverage.info
REPORT_FOLDER=coverage_report
echo "二进制目录""${BINARY_DIR}"
lcov --rc lcov_branch_coverage=1 -c -d "${BINARY_DIR}" -o "${BINARY_DIR}/${COVERAGE_FILE}"
genhtml --rc genhtml_branch_coverage=1 "${BINARY_DIR}/${COVERAGE_FILE}" -o ${REPORT_FOLDER}
