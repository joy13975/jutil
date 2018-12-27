#include "jutil.h"

#include <stdlib.h>

JUTIL_ARG_CALLBACK_DECL(help_and_exit);
JUTIL_ARG_CALLBACK_DECL(test_logs);
JUTIL_ARG_CALLBACK_DECL(test_death);

JUtilArgBundle const arg_bundles[] = {
    /* {short_form, long_form, description, exp_val, call_back} */
    {"h", "help", "Print this help text and exit", false, help_and_exit},
    {"l", "log", "Test logs", false, test_logs},
    {"p", "panic", "Test panic", false, test_death}
    /* More arguments... */
};

JUTIL_ARG_CALLBACK_DECL(help_and_exit) {
    JUtil.print_arg_bundles(arg_bundles, JUTIL_COUNT_ARG_BUNDLES(arg_bundles));
    exit(1);
}
JUTIL_ARG_CALLBACK_DECL(test_logs) {
    JUtil.error("Error\n");
    JUtil.warn("Warning\n");
    JUtil.info("Message\n");
    JUtil.debug("Debug\n");

    JUtil.set_log_lvl(LOGLVL_DEBUG);
    JUtil.error("Should %s\n", "show");
    JUtil.warn("Should %s\n", "show");
    JUtil.info("Should %s\n", "show");
    JUtil.debug("Should %s\n", "show");

    JUtil.set_log_lvl(LOGLVL_INFO);
    JUtil.error("Should %s\n", "show");
    JUtil.warn("Should %s\n", "show");
    JUtil.info("Should %s\n", "show");
    JUtil.debug("Should %s\n", "NOT show");

    JUtil.set_log_lvl(LOGLVL_WARNING);
    JUtil.error("Should %s\n", "show");
    JUtil.warn("Should %s\n", "show");
    JUtil.info("Should %s\n", "NOT show");
    JUtil.debug("Should %s\n", "NOT show");

    JUtil.info("Logging tests done\n");
}

JUTIL_ARG_CALLBACK_DECL(test_death) {
    JUtil.panic("Exit via panic()\n");
}

int main(int argc, const char ** argv) {
    JUtil.parse_args(
        argc,
        argv,
        JUTIL_COUNT_ARG_BUNDLES(arg_bundles),
        arg_bundles,
        help_and_exit);
    return 0;
}