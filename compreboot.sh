#!/bin/sh

prefix="/usr/local/etc"
conf=`IFS="."; basename ${0}`.conf

if [ -n "${prefix}" ]; then
    conf="${prefix}/${conf}"
fi

if [ -r "$conf" ]; then
    . ${conf}
fi

ppiswitch_cmd=${ppiswitch_cmd:-"/usr/local/sbin/ppiswitch"}
ppiswitch_flags=${ppiswitch_flags:-""}
channels="0 1 2 3 4 5 6 7"

usage () {
    echo "Usage: `basename ${0}` channel 
    where channel:
        0..7 or altenative name, defined at $conf"
        for _chnl in ${channels}
        do
           eval _label=\${label_${_chnl}:-"${_chnl}"}
           if [ ! "${_label}" = "${_chnl}" ]; then
               echo "        ${_label}"
           fi
        done
    exit
}


if [ "$#" -gt "0" ]; then
    for label in $*
    do
        for _chnl in ${channels}
        do
           eval _label=\${label_${_chnl}:-"${_chnl}"}
           case $label in
               ${_label}|${_chnl})
                   if [ -z "$_chnls" ]; then
                       _chnls="$_chnl"
                   else
                       _chnls="${_chnls},${_chnl}"
                   fi
                   ;;
           esac
        done
    done
    if [ -n "${_chnls}" ]; then
        ${ppiswitch_cmd} ${ppiswitch_flags} --kick ${_chnls}
    fi
else
    usage
fi
