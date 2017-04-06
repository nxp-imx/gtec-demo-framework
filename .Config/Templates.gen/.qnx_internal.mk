#VERSION 4.7.0
cpu_variants:=$(if $(filter arm,$(CPU)),v7,$(if $(filter ppc,$(CPU)),spe))

ifeq ($(filter g, $(VARIANT_LIST)),g)
DEBUG_SUFFIX=_g
LIB_SUFFIX=_g
else
DEBUG_SUFFIX=$(filter-out $(VARIANT_BUILD_TYPE) le be $(cpu_variants),$(VARIANT_LIST))
ifeq ($(DEBUG_SUFFIX),)
DEBUG_SUFFIX=_r
else
DEBUG_SUFFIX:=_$(DEBUG_SUFFIX)
endif
endif

CPU_VARIANT:=$(CPUDIR)$(subst $(space),,$(foreach v,$(filter $(cpu_variants),$(VARIANT_LIST)),_$(v)))

EXPRESSION = $(firstword $(foreach a, $(1)_$(CPU_VARIANT)$(DEBUG_SUFFIX)  $(1)$(DEBUG_SUFFIX) \
			$(1)_$(CPU_VARIANT) $(1), $(if $($(a)),$(a),)))
MERGE_EXPRESSION= $(foreach a, $(1)_$(CPU_VARIANT)$(2)$(DEBUG_SUFFIX) $(1)$(2)$(DEBUG_SUFFIX) \
		$(1)_$(CPU_VARIANT)$(2) $(1)$(2) , $($(a)))

FIX_LIB_SUFFIXES=  \
 $(if $(1),  \
    $(if $(filter $(1), -Bstatic -Bdynamic),\
      $(1) \
      $(if $(2),\
        $(call FIX_LIB_SUFFIXES,\
            $(firstword $(2)),$(wordlist 2,$(words $(2)), $(2)),$(1))),\
      $(if $(filter -Bstatic,$(3) ),\
        $($(1):%.so,%.a),$($(1):%.a,%.so)) \
      $(if $(2),\
   	    $(call FIX_LIB_SUFFIXES,\
           $(firstword $(2)), $(wordlist 2, $(words $(2)), $(2)), $(3))))) 

GCC_VERSION:=$($(call EXPRESSION,GCC_VERSION))
DEFCOMPILER_TYPE:= $($(call EXPRESSION, DEFCOMPILER_TYPE))

EXTRA_LIBVPATH := $(call MERGE_EXPRESSION, EXTRA_LIBVPATH)
extra_incvpath_tmp:=$(call MERGE_EXPRESSION,EXTRA_INCVPATH,)
EXTRA_INCVPATH = $(call MERGE_EXPRESSION,EXTRA_INCVPATH,_@$(basename $@)) \
	$(extra_incvpath_tmp)
LATE_SRCVPATH := $(call MERGE_EXPRESSION, EXTRA_SRCVPATH)
EXTRA_OBJS := $($(call EXPRESSION,EXTRA_OBJS))

CCFLAGS_D = $(CCFLAGS$(DEBUG_SUFFIX)) $(CCFLAGS_$(CPU_VARIANT)$(DEBUG_SUFFIX)) \
			$(CCFLAGS_@$(basename $@)$(DEBUG_SUFFIX)) 					  \
			$(CCFLAGS_$(CPU_VARIANT)_@$(basename $@)$(DEBUG_SUFFIX))
LDFLAGS_D = $(LDFLAGS$(DEBUG_SUFFIX)) $(LDFLAGS_$(CPU_VARIANT)$(DEBUG_SUFFIX))

CCFLAGS += $(CCFLAGS_$(CPU_VARIANT))  $(CCFLAGS_@$(basename $@)) 				  \
		   $(CCFLAGS_$(CPU_VARIANT)_@$(basename $@))  $(CCFLAGS_D)
LDFLAGS += $(LDFLAGS_$(CPU_VARIANT)) $(LDFLAGS_D)

LIBS:= $(LIBSOPT) $(patsubst %S_g, %_gS, $(foreach token, $($(call EXPRESSION,LIBS)),$(if $(findstring ^, $(token)), $(subst ^,,$(token))$(LIB_SUFFIX), $(token))))
ifdef LIBNAMES 
LIBNAMES:= $(subst lib-Bdynamic.a, ,$(subst lib-Bstatic.a, , $(LIBNAMES)))
LIBNAMES := $(call FIX_LIB_SUFFIXES,$(firstword $(LIBNAMES)),$(wordslist 2, $(words $(LIBNAMES))),-Bdynamic)
endif 
libopts := $(subst -l-B,-B, $(libopts))
ifneq ($(LIBS),)
EXTRA_DEPS += $(wildcard $(foreach a,$(EXTRA_LIBVPATH),$(a)/*.a))
endif

BUILDNAME:=$($(call EXPRESSION,BUILDNAME))$(if $(suffix $(BUILDNAME)),,$(IMAGE_SUFF_$(BUILD_TYPE)))
BUILDNAME_SAR:= $(patsubst %$(IMAGE_SUFF_$(BUILD_TYPE)),%S.a,$(BUILDNAME))

POST_BUILD:=$($(call EXPRESSION,POST_BUILD))
