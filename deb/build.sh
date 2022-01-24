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
# "$1" is a 'client' or 'server'

set -e
set -u

# set variables
script_location="$(pwd)"
script_filename="$(basename $0)"
project_location="./.."
client_filename='dxc'
server_filename='dxs'
deskx_version='1.3'
package_version='1'
fhs="usr/bin"
package_filename="deskx-${1:-}_${deskx_version}-${package_version}_amd64"
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
dpkg -s g++ 1>/dev/null
dpkg -s make 1>/dev/null

# stop if the first argument is wrong
[[ "${1:-}" = 'client' ]] || [[ "${1:-}" = 'server' ]] ||
    (echo "You need to provide client or server as the first argument"; exit 1)

function clean_build_directory()
{
    rm -rf "${script_location}/"deskx-{client,server}_*
}

# create build folder and compile required binary
clean_build_directory
cd "$project_location"
make "$1"
mkdir -p "${build_directory}/"{DEBIAN,$fhs}
cd "$script_location"

# manifest declaration
cat << EOF > ${build_directory}/DEBIAN/control
Package: deskx-${1}
Version: ${deskx_version}
Homepage: https://github.com/DeskX11/DeskX
Architecture: amd64
Maintainer: mrrva <no email>
Description: performance-oriented remote ${1} for a local network
 The project was created for its own use within the home local network
 (it is not recommended to use it over
 the Internet without an encrypted tunnel).
Depends: libx11-dev,
         libxtst-dev,
         libxext-dev

EOF

# compile package
[[ "$1" = 'client' ]] &&
    mv "${project_location}/${client_filename}" "${build_directory}/${fhs}/" &&
    dpkg-deb --build --root-owner-group ${package_filename} &&
    mv "${package_filename}.deb" "$project_location"

[[ "$1" = 'server' ]] &&
    mv "${project_location}/${server_filename}" "${build_directory}/${fhs}/" &&
    dpkg-deb --build --root-owner-group ${package_filename} &&
    mv "${package_filename}.deb" "$project_location"


clean_build_directory
exit 0

