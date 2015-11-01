Connect via telnet
=====================

After you flashed the image to the router and booted it, connect your computer
and the router with a LAN cable. The router has a fixed address of
**192.168.1.20/24**, so be sure to give your computer's NIC an address in the same
subnet.
Use _telnet(1)_ to login as user _user_:

    $ telnet 192.168.1.20
    Trying 192.168.1.20...
    Connected to 192.168.1.20.
    Escape character is '^]'.
    Trying SRA secure login:
    User (lars): user
    Password: 
    [ SRA accepts you ]
    $ 

Set passwords
=================

Now that you are logged in, set a password for the user and root:

    $ passwd user
    Changing local password for user
    New Password:
    Retype New Password:
    $

    $ passwd root
    Changing local password for user
    New Password:
    Retype New Password:
    $

Configure Networking
======================

TDB, see <https://github.com/freebsd/freebsd-wifi-build/wiki/Config-Overview>

Store run-time configuration
==============================

Every change to files in /etc is temporary and you need to store the config to
the router's NVRAM. To do so, run:

    cfg_save


Backup configuration via network
================================

If you re-flash your router, all settings you made are lost. Be sure to backup
the configuration to your computer first, so you can restore it after the
re-flash. Connect your computer with the router via LAN do the following.

On your host:

    $ nc -l 8888 > $HOME/routerconfig.tar.gz

On the router:

    # tar cvzf - --exclude .snap  -C /etc . | nc -N  192.168.1.100 8888

Restore configuration via network
=================================

To restore the configuration run this:

On the router:

    # nc -l 8888 | tar xzf - -C /etc

On your host:

    $ nc 192.168.1.20 < $HOME/routerconfig.tar.gz

Then run again on the router:

    # cfg_save
