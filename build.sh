#!/bin/bash
set -e
# We are in micropolis-rs, so go up one level
cd ..
python3 -m venv .venv
source .venv/bin/activate
pip install arcade maturin
cd micropolis-rs
cargo test
