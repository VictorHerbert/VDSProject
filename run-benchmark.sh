#!/bin/bash

PART2_DIR_DEFAULT="part2"

show_help() {
    echo "Usage: $0 [circuit]"
    echo ""
    echo "set the following variables :"    
    echo ""
    echo "  PART2_DIR   : path to folder provided by instructor with benchmarks and results (default can be set in script)"
    echo "  PROJECT_DIR : VDS project path (default is current working directory)"    
    echo ""
}

# Check for help option
if [ "$1" == "-h" ] || [ "$1" == "-help" ]; then
    show_help
    exit 0
fi

# set project directory
if [ -z "$PROJECT_DIR" ]; then    
    PROJECT_DIR=$(pwd)
fi

# set results/benchmarks directory
if [ -z "$PART2_DIR" ]; then    
    PART2_DIR=$PART2_DIR_DEFAULT
fi

# set default test circuit
if [ -z $1 ]; then    
    circuit="c3540"
else
    circuit=$1
fi

echo "running benchmark (circuit $circuit):"

eval "$PROJECT_DIR/build/src/bench/VDSProject_bench" "$PART2_DIR/benchmarks/iscas85/$circuit.bench"

echo "running verify (circuit $circuit):"
echo ""

FAIL_LIST=()

echo -n "Verifying "
for file in "$PROJECT_DIR/results_$circuit"/txt/*; do
    if [ -f "$file" ]; then        
        #echo "file : $(basename $file)" 
        #eval "$PROJECT_DIR/build/src/verify/VDSProject_verify" "$PROJECT_DIR/results_$circuit/txt/$(basename $file)" "$PART2_DIR/results/results_$circuit/txt/$(basename $file)"
        
        if "$PROJECT_DIR/build/src/verify/VDSProject_verify" "$PROJECT_DIR/results_$circuit/txt/$(basename $file)" "$PART2_DIR/results/results_$circuit/txt/$(basename $file)"; then
            echo -n "."
        else
            echo -n "x"
            FAIL_LIST+=("$(basename $file)")
        fi
    fi
done    

echo " Done!"
echo ""
echo "Fail at files:"
echo "${FAIL_LIST[*]}" | tr ' ' '\n'