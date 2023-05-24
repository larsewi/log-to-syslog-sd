#define _GNU_SOURCE

#include <stdio.h>
#include <systemd/sd-journal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>

void LogSystemLogSD(int priority, ...)
{
    va_list args;
    va_start(args, priority);
    char *key, *value;

    int i = 0, n = 2;
    struct iovec *iov = malloc(sizeof(struct iovec) * n);
    assert(iov != NULL);

    for (key = va_arg(args, char *);
         strcmp(key, "MESSAGE") != 0;
         key = va_arg(args, char *), i++, n++)
    {
        value = va_arg(args, char *);

        char *variable;
        int ret = asprintf(&variable, "%s=%s", key, value);
        assert(ret > 0);

        iov[i].iov_base = variable;
        iov[i].iov_len = ret;

        iov = realloc(iov, sizeof(struct iovec) * (n + 1));
        assert(iov != NULL);
    }

    assert(strcmp(key, "MESSAGE") == 0);

    value = va_arg(args, char *);
    char *msg;
    int ret = vasprintf(&msg, value, args);
    assert(ret > 0);

    char *variable;
    ret = asprintf(&variable, "%s=%s", key, msg);
    assert(ret > 0);

    iov[i].iov_base = variable;
    iov[i++].iov_len = ret;

    value = va_arg(args, char *);
    ret = asprintf(&variable, "PRIORITY=%d", priority);
    assert(ret > 0);
    iov[i].iov_base = variable;
    iov[i++].iov_len = ret;

    ret = sd_journal_sendv(iov, i);
    assert(ret == 0);
}

int main(void)
{
    LogSystemLogSD(LOG_ERR, "FOO", "bogus", "BAR", "doofus", "MESSAGE", "Vra%s", "ta");
    return 0;
}

