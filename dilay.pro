CONFIG      += debug_and_release
TEMPLATE     = subdirs
SUBDIRS      = lib app test

app.depends  = lib
test.depends = lib

unix {
  gdb.commands = gdb -ex run ./dilay_debug
  valgrind.commands = valgrind ./dilay_debug &> valgrind.log
  leak.commands = valgrind --leak-check=yes ./dilay_debug &> valgrind.log
  format.CONFIG = recursive
  copyright.commands = find $$SUBDIRS \\( -name "*.cpp" -o -name "*.hpp" -o -name "*.hs" \\) -print0 | \
                       xargs -0 sed -i 's/Copyright\ ©\ 2015-2017/Copyright\ ©\ 2015-2018/'

  docker.commands = docker build -t dilay-$(IMAGE) -f unix/docker/$(IMAGE)/Dockerfile unix/docker && \
                    docker run --rm --mount type=bind,src=$$(PWD),dst=/dilay dilay-$(IMAGE)

  QMAKE_EXTRA_TARGETS += gdb valgrind leak format copyright icon docker
}
