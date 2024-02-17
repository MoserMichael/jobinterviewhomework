#!/bin/bash

if [[ ! -d venv ]]; then
  ./setup.sh
fi 

bash -cx "source venv/bin/activate; python ./svc.py $*"


