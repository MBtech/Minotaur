sudo apt-get install cmake libboost-all-dev python-pip gdb -y 
curl -s https://packagecloud.io/install/repositories/github/git-lfs/script.deb.sh | sudo bash
sudo apt-get install git-lfs
git lfs install

sudo pip install paramiko
wget https://github.com/zeromq/libzmq/releases/download/v4.2.2/zeromq-4.2.2.tar.gz
tar -xf zeromq-4.2.2.tar.gz
cd zeromq-4.2.2/
./configure
make
sudo make install
sudo ldconfig

git clone https://github.com/msgpack/msgpack-c.git
cd msgpack-c
cmake .
make
sudo make install
