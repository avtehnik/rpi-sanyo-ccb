#! /bin/sh
sudo cp fm_control /usr/local/bin/fm_control
sudo cp service /etc/init.d/fm_control
sudo chmod 755 /etc/init.d/fm_control
sudo update-rc.d fm_control defaults
sudo /etc/init.d/fm_control start
