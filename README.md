## Install Zeromq
Each machine that you plan on using for the application would require following steps to install the pre-req:

```wget https://github.com/zeromq/libzmq/releases/download/v4.2.1/zeromq-4.2.1.tar.gz

tar -xf zeromq-4.2.1.tar.gz

cd zeromq-4.2.1/

./configure
make
sudo make install
sudo ldconfig

```

## Install MsgPack
```
git clone https://github.com/msgpack-c.git
cd msgpack-c
cmake .
make
sudo make install
```

## Or run the setup script
./setup.sh

**Note:** See the README.md in the Minotaur-SGX directory for information on how to run the example applications. 
