#!/usr/bin/bash

set -x

python3 -m venv venv

bash -cx 'source venv/bin/activate; pip3 install flask'



