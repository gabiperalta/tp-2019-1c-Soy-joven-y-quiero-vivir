cd
mkdir workspace
cd
mv tp-2019-1c-Soy-joven-y-quiero-vivir/ workspace/
cd

git clone https://github.com/sisoputnfrba/so-commons-library.git
cd so-commons-library/
sudo make install
cd

curl -L faq.utnso.com/script-ntp.sh | bash

cd workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/biblioteca/Debug/
make clean
make
cd

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/utnso/workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/biblioteca/Debug

cd workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/kernel/Debug/
make clean
make
cd

cd workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/memoria/Debug/
make clean
make
cd

cd workspace/tp-2019-1c-Soy-joven-y-quiero-vivir/filesystem/Debug/
make clean
make
cd

