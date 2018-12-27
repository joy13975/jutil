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
    dbg("Debug\n");
    info("Message\n");
    warn("Warning\n");
    err("Error\n");
    raw("Raw\n");

    set_log_level(LOG_DEBUG);
    gated_raw(LOG_DEBUG, "Should show\n");
    gated_raw(LOG_INFO, "Should show\n");
    gated_raw(LOG_WARNING, "Should show\n");
    gated_raw(LOG_ERROR, "Should show\n");

    set_log_level(LOG_INFO);
    gated_raw(LOG_DEBUG, "Should NOT show\n");
    gated_raw(LOG_INFO, "Should show\n");
    gated_raw(LOG_WARNING, "Should show\n");
    gated_raw(LOG_ERROR, "Should show\n");

    set_log_level(LOG_WARNING);
    gated_raw(LOG_DEBUG, "Should NOT show\n");
    gated_raw(LOG_INFO, "Should NOT show\n");
    gated_raw(LOG_WARNING, "Should show\n");
    gated_raw(LOG_ERROR, "Should show\n");

    info("Logging tests done\n");
}

DECL_ARG_CALLBACK(test_death) {
    die("Exit successful\n");
}

int main(int argc, const char ** argv) {
    parse_args(argc, argv, argb, help_and_exit);
    return 0;
}