/**      
 * @file sample_app.c
 * @author Ravi Tandon
 * brief Sample app to register with sysrepo datastore based on Netconf/YANG model
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <signal.h>

#include "sysrepo.h"
#include "sysrepo/values.h"
#include "sysrepo/xpath.h"

/*Don't change below line manually, generator will replace xxyyzz
* with actual name of generated hdr file*/
#include "XXYYZZ"

#define MAX_LEN 512
volatile int stop_app = 0;


static void
sigint_handler(int signum)
{
    stop_app = 1;
}

static void
dump_config(sr_session_ctx_t *sess, const char *module_name)
{
    sr_val_t *values = NULL;
    size_t count = 0;
    int rc = SR_ERR_OK;
    char xpath[MAX_LEN] = {0};
    snprintf(xpath, MAX_LEN, "/%s:*//.", module_name);

    rc = sr_get_items(sess, xpath, &values, &count);
    if (SR_ERR_OK != rc) {
        printf("Error by sr_get_items: %s", sr_strerror(rc));
        return;
    }
    for (size_t i = 0; i < count; i++){
        if(!((values[i].type == SR_CONTAINER_T)
                    ||(values[i].type == SR_CONTAINER_PRESENCE_T)))
        {
            sr_print_val(&values[i]);
        }
    }
    sr_free_values(values, count);
}


int main(int argc, char **argv)
{
    sr_conn_ctx_t *conn = NULL;
    sr_session_ctx_t *sess = NULL;
    sr_subscription_ctx_t *subscptn = NULL;
    int i =0;
    int rc = SR_ERR_OK;
    uint32_t priority = 1;
    sr_subscr_options_t opts = SR_SUBSCR_APPLY_ONLY;

    if (argc == 1) {
        fprintf(stderr, "No modules specified.\n");
        return 1;
    }

    /* connect to sysrepo */
    rc = sr_connect(argv[1], SR_CONN_DEFAULT, &conn);
    if (SR_ERR_OK != rc) {
        fprintf(stderr, "Error by sr_connect: %s\n", sr_strerror(rc));
        goto exitapp;
    }

    /* start session */
    rc = sr_session_start(conn, SR_DS_STARTUP, SR_SESS_DEFAULT, &sess);
    if (SR_ERR_OK != rc) {
        fprintf(stderr, "Error by sr_session_start: %s\n", sr_strerror(rc));
        goto exitapp;
    }

    setvbuf(stdout, NULL, _IOLBF, MAX_LEN);
    setvbuf(stderr, NULL, _IOLBF, MAX_LEN);
    printf("\n\n ========== READING STARTUP CONFIG %s: ==========\n\n", argv[i]);
    dump_config(sess, argv[1]);

    rc = subscribe_leaf_callbacks(sess, priority, opts, subscptn);
    if (SR_ERR_OK != rc) {
        goto exitapp;
    }
    /* loop until ctrl-c is pressed / SIGINT is received */
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, SIG_IGN);
    while (!stop_app) {
        sleep(500);
    }

    printf("Exiting App!.\n");

exitapp:
    if (NULL != subscptn) {
        sr_unsubscribe(sess, subscptn);
    }
    if (NULL != sess) {
        sr_session_stop(sess);
    }
    if (NULL != conn) {
        sr_disconnect(conn);
    }
    return rc;
}
