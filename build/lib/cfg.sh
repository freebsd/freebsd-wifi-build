#!/bin/sh

# This library takes a set of configuration parameters
# and builds the configuration variables used elsewhere
# in the build system.

# Configuration variables which need to be set

CFGVARS="CFGNAME BUILDNAME KERNCONF TARGET TARGET_ARCH LOCAL_DIRS"
UBNT_VARS="UBNT_ARCH UBNT_VERSION"

# TARGET_CPUTYPE doesn't have to be set for i386, amd64, etc.

DOEXIT="NO"

for i in ${CFGVARS}; do
	eval _value=\$${i}
	if [ "x${_value}" = "x" ]; then
		echo "ERROR: ${i} needs to be set in the configuration file."
		DOEXIT="YES"
	fi
done

if [ "x${DOEXIT}" = "xYES" ]; then
	echo "ERROR: fatal errors found; exiting."
	exit 127
fi

# BUILD_FLAGS is set by the environment
BUILD_FLAGS=${BUILD_FLAGS:="NO_CLEAN=1 -j2"}

# CFGNAME
# BUILDNAME
# UBNT_ARCH
# UBNT_VERSION
# KERNCONF
# TARGET
# TARGET_ARCH
# TARGET_CPUTYPE
# BUILD_FLAGS
# LOCAL_DIRS

# X_MAKEFS_ENDIAN - what endian-ness should the makefs FFS be?
X_MAKEFS_ENDIAN="be"

# X_MAKEFS_FLAGS - what flags to pass to makefs when building the MFS image
X_MAKEFS_FLAGS="version=1,bsize=4096,fsize=512"

# X_MAKEFS_FULL_FLAGS - what flags to pass to makefs when building the full image
X_MAKEFS_FULL_FLAGS="version=2"

# X_FSSIZE - how big to make the makefs?
X_FSSIZE=${X_FSSIZE:="20971520"}

# X_FULL_FSSIZE - how big to make the full image?
X_FULL_FSSIZE=${X_FULL_FSSIZE:="1073741824"}

# Variables defined by this script

# X_MAKEOBJDIRPREFIX - where to place object files
X_MAKEOBJDIRPREFIX="${CUR_DIR}/../obj/${BUILDNAME}/" ; export X_MAKEOBJDIRPREFIX

# X_DESTDIR - where to place the install root 
X_DESTDIR="${CUR_DIR}/../root/${BUILDNAME}"

# Variables that need to be defined by one of the build scripts
# but I haven't decided whether they're configured by the user
# or automatically generated.

# X_FSIMAGE
X_FSIMAGE="${CUR_DIR}/../mfsroot-${CFGNAME}.img"

# X_FULL_FSIMAGE
X_FULL_FSIMAGE="${CUR_DIR}/../fullroot-${CFGNAME}.img"

# X_FSIMAGE_SUFFIX
X_FSIMAGE_SUFFIX=${X_FSIMAGE_SUFFIX:=".uzip"}

# X_FSIMAGE_CMD
X_FSIMAGE_CMD=${X_FSIMAGE_CMD:="mkuzip"}

# X_FSIMAGE_ARGS
X_FSIMAGE_ARGS=${X_FSIMAGE_ARGS:="-s 16384"}

# X_STAGING_FSROOT
X_STAGING_FSROOT="${CUR_DIR}/../mfsroot/${CFGNAME}"

# X_STAGING_TMPDIR
X_STAGING_TMPDIR="${CUR_DIR}/../tmp/${CFGNAME}"

# X_KERNEL
X_TFTPBOOT_KERNEL="/tftpboot/kernel.${KERNCONF}"
X_KERNEL="${X_DESTDIR}/boot/kernel.${KERNCONF}/kernel"

# Configuration filesystem image
X_CFGFS="${CUR_DIR}/../cfgfs-${CFGNAME}.img"

# Defaults!
X_FSIMAGE_CMD=${X_FSIMAGE_CMD:="mkuzip"}
X_FSIMAGE_ARGS=${X_FSIMAGE_ARGS:="-s 16384"}
X_FSIMAGE_SUFFIX=${X_FSIMAGE_SUFFIX:=".uzip"}

