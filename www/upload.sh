#!/bin/bash

export IMAGES="*.png *.ico"
export TEXT="*.html *.css ChangeLog"
export DOWNLOADS="downloads"

rsync -v -L -r --rsh="ssh" ${IMAGES} ${TEXT} ${DOWNLOADS} icculus.org:/webspace/projects/referencer/
