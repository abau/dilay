TEMPLATE     = subdirs
SUBDIRS      = lib app test

app.depends  = lib
test.depends = lib
