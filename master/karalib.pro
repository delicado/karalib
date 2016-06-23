TEMPLATE = subdirs

CONFIG += C++14

SUBDIRS += \
    karalib \
    karalib_console_karaoke

karalib.subdir = karalib
karalib_console_karaoke.subdir = karalib_console_karaoke
karalib_console_karaoke.depends = karalib
