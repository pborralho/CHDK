# Build rules for platform independant code (except modules) - code built in THUMB mode

include $(topdir)makefile_base.inc
include $(topdir)arm_rules.inc

CFLAGS+=-mthumb -mthumb-interwork -DCHDK_MODULE_CODE

include $(topdir)build_rules.inc

# Define empty recipes for source files (including the makefiles)
# to prevent make from trying implicit rules to create them. Speeds up build process
$(topdir)makefile_core.inc: ;
