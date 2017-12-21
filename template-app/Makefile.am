# Makefile.am - libtemplate.a

common_sources:= \
	TemplateApplication.cpp

# Target name DSO
lib_LTLIBRARIES := libtemplate.la

libtemplate_la_CPPFLAGS := \
	-I$(SDKTARGETSYSROOT)/usr/include/servicelayer \
	-D_MORE_MARSHM
	-O2 \
	-g \
	-Wall \
	-Wno-unused-parameter \
	-DHAVE_SYS_UIO_H \
	-rdynamic \
	-fpermissive

libtemplate_la_LDFLAGS = \
	-lpthread \
	-lapplication \
	-liapplication \
	-lisystem \
	-shared \
	-module \
	-export-dynamic \
	-export-symbols-regex 'createApplication'

requiredlibs := -lpthread

libtemplate_la_SOURCES := \
	$(common_sources)
