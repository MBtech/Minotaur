gcc -c aesgcm.c 
gcc -o aesgcm aesgcm.o -I /usr/include/openssl/  -lssl -lcrypto
