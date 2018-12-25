#include "jutil.h"


// Test Main
DECL_ARG_CALLBACK(help_and_exit);
DECL_ARG_CALLBACK(test_logs);
DECL_ARG_CALLBACK(test_death);

const argument_bundle argb[] = {
    /* {short_form, long_form, description, exp_val, call_back} */
    {"h", "help", "Print this help text and exit", false, help_and_exit},
    {"l", "log", "Test logs", false, test_logs},
    {"d", "die", "Test death", false, test_death}
    /* More arguments... */
};

DECL_ARG_CALLBACK(help_and_exit) {
    print_arg_bundles(argb);
    exit(1);
}
DECL_ARG_CALLBACK(test_logs) {
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
}

DECL_ARG_CALLBACK(test_death) {
    die("Exit successful\n");
}

int main(int argc, const char ** argv) {
    parse_args(argc, argv, argb, help_and_exit);
    return 0;
}