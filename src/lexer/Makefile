LOCAL_FLAG = $(DIRNAME_LEXER)$(DELIMITER)
LOCAL_FILES = $(wildcard *.c)
LOCAL_PREFIX = $(OBJDIR)/$(LOCAL_FLAG)

LOCAL_OBJ_PREFIX=$(OBJDIR)/$(LOCAL_FLAG)
LOCAL_DEP_PREFIX=$(DEPDIR)/$(LOCAL_FLAG)
LOCAL_OBJFILES =$(LOCAL_FILES:%.c=$(LOCAL_OBJ_PREFIX)%.o)
LOCAL_DEPFILES =$(LOCAL_FILES:%.c=$(LOCAL_DEP_PREFIX)%.d)

.PHONY: all local 

-include $(LOCAL_DEPFILES)

$(LOCAL_OBJ_PREFIX)%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

$(LOCAL_DEP_PREFIX)%.d:%.c
	$(CC) $(DEPFLAG) $< | sed '1s|^|'"$(LOCAL_OBJ_PREFIX)"'|' > $@

all: local

local: $(LOCAL_OBJFILES)
