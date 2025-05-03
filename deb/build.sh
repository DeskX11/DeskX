#!/usr/bin/env bash
#
# simple deb builder
# this is a stupid and straightforward way to get a working
# deb packages for the deskx program automatically. if you are know how to
# build deb packages you are welcome to wipe this
# script and do it in a way like pro maintainers doing it
#
# how to use:
# it is supposed to run from the makefile, but
# nothing prevents you from running it manually with
# script.sh "$1"
# where
# "$1" is deskx version

set -e
set -u

# set variables
script_location="$(pwd)"
script_filename="$(basename $0)"
project_location="./.."
deskx_version="${1}"
package_version='1'
fhs="usr/bin"
package_filename="deskx_${deskx_version}-${package_version}_amd64"
build_directory="${script_location}/${package_filename}"

# stop if the script is run from the wrong directory
[[ ! -f "${script_location}/${script_filename}" ]] &&
    (echo "Script needs to be run from its own directory"; exit 1)

# stop if the deskx project's location is wrong
[[ ! -f "${project_location}/Makefile" ]] &&
    (echo "Can't find the original Makefile"; exit 1)

# stop if the dependency check will fail
command -v dpkg-deb 1>/dev/null ||
    (echo "Can't find dpkg-deb"; exit 1)
dpkg -s libx11-dev 1>/dev/null
dpkg -s libxtst-dev 1>/dev/null
dpkg -s libxext-dev 1>/dev/null
dpkg -s libsdl2-dev 1>/dev/null
dpkg -s libportal-dev 1>/dev/null
dpkg -s libpipewire-0.3-dev 1>/dev/null
dpkg -s g++ 1>/dev/null
dpkg -s make 1>/dev/null

function clean_build_directory() {
    rm -rf "${script_location}/"deskx_*
}

# create build folder and compile required binary
clean_build_directory
cd "$project_location"
make
mkdir -p "${build_directory}/"{DEBIAN,$fhs}
cd "$build_directory"

# manifest declaration
cat << EOF > ./DEBIAN/control
Package: deskx
Version: ${deskx_version}
Homepage: https://github.com/DeskX11/DeskX
Architecture: amd64
Maintainer: mrrva <no email>
Description: performance-oriented remote control of a computer
 (it is not recommended to use it over the Internet without an encrypted
 tunnel, for example can use: SSH with port forwarding).
Depends: libx11-dev,
         libxtst-dev,
         libxext-dev,
         libsdl2-dev,
         libportal-dev,
         libpipewire-0.3-dev

EOF

# compile package
cd ..
mv "${project_location}/deskx" "${build_directory}/${fhs}/"
dpkg-deb --build --root-owner-group ${package_filename}
mv "${package_filename}.deb" "$project_location"

clean_build_directory
exit 0