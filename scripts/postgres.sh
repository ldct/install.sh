apt-get install bison flex libreadline-dev libz-dev --assume-yes
git clone git://git.postgresql.org/git/postgresql.git
cd postgresql
./configure
make
make install
cd contrib
make
make install
adduser --disabled-password --gecos "" postgres
mkdir /usr/local/pgsql/data
chown postgres -R /usr/local/pgsql/data
