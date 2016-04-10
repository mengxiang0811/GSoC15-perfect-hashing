cp xia_vxidty.h /home/developer/XIA-for-Linux/include/net/
cp vxidty.c /home/developer/XIA-for-Linux/net/xia/

sudo rmmod xia_ppal_ad
sudo rmmod xia_ppal_hid
sudo rmmod xia

cd ~/XIA-for-Linux
make M=net/xia

sudo install -oroot -groot -m644 net/xia/xia.ko /lib/modules/`uname -r`/kernel/net/xia/


sudo modprobe xia
sudo modprobe xia_ppal_ad
sudo modprobe xia_ppal_hid

#sudo install -oroot -groot -m644 net/xia/xia.ko /lib/modules/4.0.0-rc1-xia/kernel/net/xia/

