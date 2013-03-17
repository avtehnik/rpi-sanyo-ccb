#! /bin/sh
sudo killall fm_control
sudo update-rc.d -f fm_control remove
sudo rm /usr/local/bin/fm_control
sudo rm /etc/init.d/fm_control
