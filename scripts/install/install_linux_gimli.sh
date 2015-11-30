#!/usr/bin/env bash

ROOT=$PWD
PARALELL=2

CMAKE_GENERATOR='Unix Makefiles'

buildGIMLI(){
        if [ -d "gimli" ]; then
            pushd gimli
                git pull   
            popd  
        else
            git clone https://github.com/gimli-org/gimli.git
        fi
          
        [ -n "$BRANCH" ] && git checkout $BRANCH  
        
        chmod +x gimli/python/apps/*
        
        rm -rf build/
        mkdir -p build
    pushd build
            cmake -G "$CMAKE_GENERATOR" ../gimli 

            make -j$PARALELL && make pygimli J=$PARALELL 
            echo ""
            echo ""
            echo "============================================================================"
            echo "---  try some basic test: calling pygimli once------------------------------"
            export PYTHONPATH=$PYTHONPATH:$ROOT/gimli/python
            python -c 'import pygimli as pg; print("pygimli version:", pg.__version__)'
            echo "--- ------------------------------------------------------------------------"
            echo "export PYTHONPATH=\$PYTHONPATH:$ROOT/gimli/python" > $ROOT/.bash_hint_pygimli
            echo "export PATH=\$PATH:$ROOT/gimli/python/apps" >> $ROOT/.bash_hint_pygimli
           
    popd
}


buildGIMLI