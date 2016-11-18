#include "util.h"

void test_logs();

int main(void)
{

    test_logs();
    return 0;
}

void test_logs()
{
    prf("Proof\n");

    dbg("Debug\n");

    wrn("Warning\n");

    msg("Message\n");

    raw("Raw\n");

    set_log_level(LOG_DEBUG);
    raw_at(LOG_DEBUG, "Raw at Debug (should show)\n");

    set_log_level(LOG_WARN);
    raw_at(LOG_DEBUG, "Raw at Debug (should not show)\n");

    raw_at(LOG_WARN, "Raw at Warning (should show)\n");

    set_log_level(LOG_MESSAGE);
    raw_at(LOG_WARN, "Raw at Warning (should NOT show)\n");

    err("Error\n");

    msg("Test done - now exit with die()...\n");
    die("Exit successful\n");
}