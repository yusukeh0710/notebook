### Setup iSCSI target @ CentOS7

Install package and service on

    # yum -y install targetcli
    # systemctl enable target.service
    # systemctl start target.service

Configure storage volume structure

    # targetcli ls /
    o- / ........................................................... [...]
      o- backstores ................................................ [...]
      | o- block .................................... [Storage Objects: 0]
      | o- fileio ................................... [Storage Objects: 0]
      | o- pscsi .................................... [Storage Objects: 0]
      | o- ramdisk .................................. [Storage Objects: 0]
      o- iscsi .............................................. [Targets: 0]
      o- loopback ........................................... [Targets: 0]

Define backend blocke device

     # targetcli /backstores/block create name=lun0 dev=/dev/sdb
       | o- block .................................... [Storage Objects: 0]
       | | o- lun0  [/dev/sdb (20.0GiB) write-thru deactivated]

Define IQN

    # targetcli /iscsi create iqn.2017-11.com.example:host
    o- iscsi .............................................. [Targets: 1]
    | o- iqn.2017-11.com.example:host......................... [TPGs: 1]
    |   o- tpg1 ................................. [no-gen-acls, no-auth]
    |     o- acls ............................................ [ACLs: 0]
    |     o- luns ............................................ [LUNs: 0]
    |     o- portals ...................................... [Portals: 1]
    |       o- 0.0.0.0:3260 ....................................... [OK]
      
Associate the backend device to the IQN

    # targetcli /iscsi/iqn.2015-12.com.example:myhost01/tpg1/luns create /backstores/block/lun0
    o- iscsi .............................................. [Targets: 1]
    | o- iqn.2017-11.com.example:host......................... [TPGs: 1]
    |   o- tpg1 ................................. [no-gen-acls, no-auth]
    |     o- acls ............................................ [ACLs: 0]
    |     o- luns ............................................ [LUNs: 1]
    |     | o- lun0iqn.2017-11.com.example:host .............. [block/lun0 (/dev/sdb)]

Configure ACL for iSCSI initiator

    #targetcli /iscsi/iqn.2017-11.com.example:host/tpg1/acls create iqn.1994-05.com.redhat:xxxxxxxxxx
    
Save configuration

    # targetcli saveconfig
    
### Appendix
#### The type of backend device
1. BlOCKIO
 Specify the block device. The way provide best performance.

1. FILEIO (VFS devices)
 Specify the file. The file is used as virtual block device.

1. PSCSI(Pass-through SCSI devices)
  Spcify SCSI device. It provides the device in /proc/scsi/scsi.

1. RAMDISK
　 Provide memory as RAM disk.
