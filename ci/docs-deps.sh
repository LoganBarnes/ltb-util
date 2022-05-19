#!/usr/bin/env bash

# Install pipx for managing python dependencies and doxygen for C++ documentation
sudo apt-get install -y python3-pip python3-venv doxygen
python3 -m pip install --user pipx
export PATH="$PATH:$HOME/.local/bin"
# The main Markdown to HTML generation tool
pipx install sphinx
# Add extensions and themes to sphinx
pipx inject sphinx breathe exhale myst-parser sphinxcontrib-mermaid sphinx_rtd_theme

