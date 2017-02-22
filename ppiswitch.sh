#!/bin/sh
#
# PROVIDE: ppiswitch
# KEYWORD: shutdown
#
# Add the following to /etc/rc.conf[.local] to enable this service
#
# ppiswitch_enable="YES"
#
# ppiswitch_dev=auto
# ppiswitch_dev="ppi0 ppi1 ppi2"
# ppiswitch_value=255
# ppiswitch_value_ppi0=FILE
# ppiswitch_value_ppi2=FILE

. /etc/rc.subr

name=ppiswitch
rcvar=`set_rcvar`

command=/usr/local/sbin/ppiswitch
extra_commands="restore save"
start_cmd=ppiswitch_start
stop_cmd=ppiswitch_stop
restore_cmd=ppiswitch_restore
save_cmd=ppiswitch_save
required_modules="ppi"

load_rc_config ${name}

: ${ppiswitch_enable=NO}
: ${ppiswitch_dev=AUTO}		# "ppi0 ppi1" or "AUTO"
: ${ppiswitch_value=0}		# "0..255" or "FILE"

ppiswitch_start () {
    local _dev _ppiswitch_dev
    _ppiswitch_dev=$*
    if [ -z "${_ppiswitch_dev}" ]; then
        _ppiswitch_dev=$ppiswitch_dev
    fi 
    for _dev in ${_ppiswitch_dev}
    do
        ppiswitch_start_dev $_dev
    done
}

ppiswitch_stop () {
    local _dev 
    _ppiswitch_dev=$*
    if [ -z "${_ppiswitch_dev}" ]; then
        _ppiswitch_dev=$ppiswitch_dev
    fi 
    for _dev in ${_ppiswitch_dev}
    do
        ppiswitch_stop_dev $_dev
    done
}

ppiswitch_save () {
    local _dev 
    _ppiswitch_dev=$*
    if [ -z "${_ppiswitch_dev}" ]; then
        _ppiswitch_dev=$ppiswitch_dev
    fi 
    for _dev in ${_ppiswitch_dev}
    do
        ppiswitch_save_dev $_dev
    done
}

ppiswitch_restore () {
    local _dev 
    _ppiswitch_dev=$*
    if [ -z "${_ppiswitch_dev}" ]; then
        _ppiswitch_dev=$ppiswitch_dev
    fi 
    for _dev in ${_ppiswitch_dev}
    do
        ppiswitch_restore_dev $_dev
    done
}

ppiswitch_start_dev () {
    local _dev _device _value _ppiswitch_value
    _dev=$1
    _device="/dev/${_dev}"
    if [ -c "${_device}" -a -r "${_device}" -a -w "${_device}" ]; then
        _value=0
        eval _ppiswitch_value=\$ppiswitch_value_${_dev}
        if [ -z $_ppiswitch_value ]; then
            _ppiswitch_value=${ppiswitch_value}
        fi
        case $_ppiswitch_value in
            [Ff][Ii][Ll][Ee])
                if [ -r /var/db/ppiswitch.${_dev} ]; then
                    _value=`head -n 1 /var/db/ppiswitch.${_dev}` 
                fi
                ;;
            *)
                _value=${_ppiswitch_value}
                ;;
        esac
        $command --quiet --device ${_device} --value ${_value}
    fi
}

ppiswitch_stop_dev () {
    local _dev _device _ppiswitch_value
    _dev=$1
    _device="/dev/${_dev}"
    if [ -c "${_device}" -a -r "${_device}" -a -w "${_device}" ]; then
        eval _ppiswitch_value=\$ppiswitch_value_${_dev}
        if [ -z $_ppiswitch_value ]; then
            _ppiswitch_value=${ppiswitch_value}
        fi
        case $_ppiswitch_value in
            [Ff][Ii][Ll][Ee])
                $command --quiet --device ${_device} --status > /var/db/ppiswitch.${_dev} 
                ;;
        esac
    fi
}

ppiswitch_save_dev () {
    local _dev _device _ppiswitch_value
    _dev=$1
    _device="/dev/${_dev}"
    if [ -c "${_device}" -a -r "${_device}" -a -w "${_device}" ]; then
        eval _ppiswitch_value=\$ppiswitch_value_${_dev}
        if [ -z $_ppiswitch_value ]; then
            _ppiswitch_value=${ppiswitch_value}
        fi
        $command --quiet --device ${_device} --status > /var/db/ppiswitch.${_dev} 
    fi
}

ppiswitch_restore_dev () {
    local _dev _device _value _ppiswitch_value
    _dev=$1
    _device="/dev/${_dev}"
    if [ -c "${_device}" -a -r "${_device}" -a -w "${_device}" ]; then
        _value=0
        eval _ppiswitch_value=\$ppiswitch_value_${_dev}
        if [ -z $_ppiswitch_value ]; then
            _ppiswitch_value=${ppiswitch_value}
        fi
        if [ -r /var/db/ppiswitch.${_dev} ]; then
            _value=`head -n 1 /var/db/ppiswitch.${_dev}`
        else 
            _value=${_ppiswitch_value}
        fi
        $command --quiet --device ${_device} --value ${_value} 
    fi
}

case $ppiswitch_dev in
    [Aa][Uu][Tt][Oo])
        cd /dev
        ppiswitch_dev=`ls -1 ppi*`
        ;;
esac

run_rc_command $*
