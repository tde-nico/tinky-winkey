SERVICE			= svc.exe
KEYLOGGER		= keylogger.exe

CC				= cl
CFLAGS			= /Wall /WX
LINK			= link
RM				= del

SRC_SERVICE		=
SRC_KEYLOGGER	=

OBJ_SERVICE		= $(SRC_SERVICE:.c=.obj)
OBJ_KEYLOGGER	= $(SRC_KEYLOGGER:.c=.obj)


all: $(SERVICE) $(KEYLOGGER)

$(SERVICE): $(OBJ_SERVICE)
	$(LINK) /nologo $< /OUT:$@

$(KEYLOGGER): $(OBJ_KEYLOGGER)
	$(LINK) /nologo $< /OUT:$@

%.c: %.obj:
	$(CC) $(CFLAGS) /nologo /I./keylogger/include /I./service/include /c $< /Fo: $@

clean:
	$(RM) $(OBJ_SERVICE) $(OBJ_KEYLOGGER)

fclean: clean
	$(RM) $(SERVICE) $(KEYLOGGER)

re: fclean all

.PHONY: all clean fclean re 
