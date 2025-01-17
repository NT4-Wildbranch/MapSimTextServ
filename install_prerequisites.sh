#!/bin/bash

echo "Installing Xcode Command Line Tools..."
xcode-select --install

echo "Installing Homebrew..."
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

echo "Installing Python 3..."
brew install python

echo "Verifying Python installation..."
python3 --version

echo "Installing Git (if not installed)..."
brew install git

echo "Prerequisites installation complete!"
