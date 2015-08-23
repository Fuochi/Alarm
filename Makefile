CONTIKI_PROJECT = alarm
all: $(CONTIKI_PROJECT)

#UIP_CONF_IPV6=1
PROJECT_SOURCEFILES += distanceSensors.c

CONTIKI = $(HOME)/contiki
include $(CONTIKI)/Makefile.include
