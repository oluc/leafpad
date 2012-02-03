#!/bin/bash

# script to create and populate 
# a git repository for leafpad, 
# from its tarballs

mkdir leafpad.git
cd leafpad.git
git init
cp ../create-leafpad-git-repository.sh .
git add .
git commit create-leafpad-git-repository.sh -m "Creating repository for leafpad clone. (history from tarballs)"

git branch -m master upstream/tarballs

TARBALLS_DIR=../tarballs/download.savannah.gnu.org/releases/leafpad
TARBALLS_NUM="\
    0.7.6  \
    0.7.7  \
    0.7.8  \
    0.7.9  \
    0.8.0  \
    0.8.1  \
    0.8.2  \
    0.8.3  \
    0.8.4  \
    0.8.5  \
    0.8.6  \
    0.8.7  \
    0.8.8  \
    0.8.9  \
    0.8.10 \
    0.8.11 \
    0.8.12 \
    0.8.13 \
    0.8.14 \
    0.8.15 \
    0.8.16 \
    0.8.17 \
    0.8.18 \
    0.8.18.1"


for ii in $TARBALLS_NUM; do
    rm -rf ./*
    git rm . -r

    tar zxf ./$TARBALLS_DIR/leafpad-$ii.tar.gz
    mv leafpad-$ii/* .
    rmdir leafpad-$ii
    git add . 

    GIT_AUTHOR_NAME="Tarot Osuji"                                               \
    GIT_AUTHOR_EMAIL="<unknown email>"                                          \
    GIT_AUTHOR_DATE="$(stat --format '%y' ./$TARBALLS_DIR/leafpad-$ii.tar.gz)"  \
      git commit -m "import tarball leafpad--$ii.tar.gz"

    git tag "release-$ii"
done


git branch master
git checkout master

exit




# tarballs are in
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.7.6.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.7.7.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.7.8.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.7.9.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.0.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.1.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.2.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.3.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.4.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.5.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.6.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.7.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.8.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.9.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.10.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.11.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.12.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.13.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.14.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.15.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.16.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.17.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.18.tar.gz
#../tarballs/download.savannah.gnu.org/releases/leafpad/leafpad-0.8.18.1.tar.gz
