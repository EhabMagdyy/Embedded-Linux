#!/bin/bash

if [ -f ~/.bashrc ]; then
    echo "You have a .bashrc file."

    grep -qxF "export HELLO=${HOSTNAME}" ~/.bashrc || echo "export HELLO=$HOSTNAME" >> ~/.bashrc
    LOCAL=$(whoami)
    grep -qxF "LOCAL=$LOCAL" ~/.bashrc || echo "LOCAL=$LOCAL" >> ~/.bashrc
    gnome-terminal -- bash -ic "echo -e 'HELLO = $HELLO\nLOCAL = $LOCAL'; exec bash"

else
    echo "You do not have a .bashrc file."
fi
