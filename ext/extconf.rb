# CentOS:
# cd /usr/local/src
# wget http://download.icu-project.org/files/icu4c/3.8/icu4c-3_8-src.tgz
# tar -xzvf icu4c-3_8-src.tgz
# cd icu/source
# ./runConfigureICU Linux
# make
# make install
# cd /web/bandaancha.eu/www/vendor/plugins/g11n/lib
# ruby extconf.rb --with-icu-lib=/usr/local/lib --with-icu-include=/usr/local/src/icu/source/i18n
# make
# ldd g11n.so y comprobar que no faltan librerias, si faltan:
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/bin
# vi /etc/profile y añado la anterior linea al final


#$LDFLAGS = "icuuc.lib icuin.lib"
#$LDFLAGS = "-licuuc -licui18n -licudata"
#$LDFLAGS = "-licuuc -licui18n -licudata"
#$CFLAGS = "-rpath=/usr/local/lib -L/usr/local/lib"

require 'mkmf'

dir_config('icu')

if have_library('icui18n')
  create_makefile('icu')
else
  puts "ICU v3.x required -- not found."
  exit 1
end