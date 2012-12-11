#include "uni-dbgen.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <pthread.h>


typedef struct {
    pthread_t pth;
    int offset;
} cust_thread_t;

/* static variables */
static option_t option;

static GOptionEntry entries[] =
{
    { "scalefactor", 's', 0, G_OPTION_ARG_INT, &option.scalefactor, "TPC-H scale factor", "N" },
    { "parallel", 'p', 0, G_OPTION_ARG_INT, &option.parallel, "Parallelism", "N" },
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &option.verbose, "Be verbose", NULL },
    { NULL }
};

static FILE *customer_file;
static FILE *orders_file;
static FILE *lineitem_file;
static FILE *part_file;


/* static function prototypes */

static void parse_args(gint argc, gchar **argv);

static int rand_within_int(int from, int to);
static long rand_within_long(long from, long to);

static void *part_thread_handler(void *arg);
static void *cust_thread_handler(void *arg);

static void make_orders(long custkey);
static void make_lineitem(long orderkey);


/* static function bodies */
static void
parse_args(gint argc, gchar **argv)
{
    GError *error = NULL;
    GOptionContext *context;

    option.scalefactor = 0;
    option.parallel = 1;
    option.verbose = false;

    context = g_option_context_new ("");
    g_option_context_add_main_entries (context, entries, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &error))
    {
        g_print ("option parsing failed: %s\n", error->message);
        exit(EXIT_FAILURE);
    }

    if (option.scalefactor <= 0)
    {
        g_print ("scale factor must be greater than 0: %d\n", option.scalefactor);
        exit(EXIT_FAILURE);
    }
}

static int
rand_within_int(int from, int to)
{
    return from + (int)((to - from + 1) * drand48());
}

static long
rand_within_long(long from, long to)
{
    return from + (long)((to - from + 1) * drand48());
}


static const char *p_name_bag[] = {
    "almond", "antique", "aquamarine", "azure",
    "beige", "bisque", "black", "blanched", "blue", "blush", "brown", "burlywood", "burnished",
    "chartreuse", "chiffon", "chocolate", "coral", "cornflower", "cornsilk", "cream", "cyan",
    "dark", "deep", "dim", "dodger", "drab", "firebrick", "floral", "forest", "frosted",
    "gainsboro", "ghost", "goldenrod", "green", "grey", "honeydew", "hot", "indian", "ivory", "khaki",
    "lace", "lavender", "lawn", "lemon", "light", "lime", "linen",
    "magenta", "maroon", "medium", "metallic", "midnight", "mint", "misty", "moccasin",
    "navajo", "navy", "olive", "orange", "orchid",
    "pale", "papaya", "peach", "peru", "pink", "plum", "powder", "puff", "purple",
    "red", "rose", "rosy", "royal",
    "saddle", "salmon", "sandy", "seashell", "sienna", "sky", "slate", "smoke", "snow", "spring", "steel",
    "tan", "thistle", "tomato", "turquoise", "violet", "wheat", "white", "yellow"};
static const int p_name_bag_size = 92;

static const char *p_type_syll1[] = {
    "STANDARD", "SMALL", "MEDIUM", "LARGE", "ECONOMY", "PROMO"
};
static const char *p_type_syll2[] = {
    "ANODIZED", "BURNISHED", "PLATED", "POLISHED", "BRUSHED"
};
static const char *p_type_syll3[] = {
    "TIN", "NICKEL", "BRASS", "STEEL", "COPPER"
};

static const char *p_container_syll1[] = {
    "SM", "LG", "MED", "JUMBO", "WRAP"
};
static const char *p_container_syll2[] = {
    "CASE", "BOX", "BAG", "JAR", "PKG", "PACK", "CAN", "DRUM"
};

static void *
part_thread_handler(void *arg)
{
    long partkey;
    char name[128];
    char mfgr[64];
    char brand[64];
    char type[64];
    int size;
    char container[64];
    long retailprice;
    char comment[23];

    srand48(0);

    for (partkey = 1; partkey <= option.scalefactor * 200000; partkey ++)
    {
        int M;

        /* Generate P_NAME */
        sprintf(name, "%s %s %s %s %s",
                p_name_bag[rand_within_int(0, p_name_bag_size - 1)],
                p_name_bag[rand_within_int(0, p_name_bag_size - 1)],
                p_name_bag[rand_within_int(0, p_name_bag_size - 1)],
                p_name_bag[rand_within_int(0, p_name_bag_size - 1)],
                p_name_bag[rand_within_int(0, p_name_bag_size - 1)]);

        /* Generate P_MFGR */
        sprintf(mfgr, "Manufacture#%d", (M = rand_within_int(1, 5)));

        /* Generate P_BRAND */
        sprintf(brand, "Brand#%d%d", M, rand_within_int(1, 5));

        /* Generate P_TYPE */
        sprintf(type, "%s %s %s",
                p_type_syll1[rand_within_int(0, 5)],
                p_type_syll2[rand_within_int(0, 4)],
                p_type_syll3[rand_within_int(0, 4)]);

        /* Generate P_SIZE */
        size = rand_within_int(1, 50);

        /* Generate P_CONTAINER */
        sprintf(container, "%s %s",
                p_container_syll1[rand_within_int(0, 4)],
                p_container_syll2[rand_within_int(0, 7)]);

        /* Generate P_RETAILPRICE */
        retailprice = (90000 + ((partkey / 10) % 20001) + 100 * (partkey % 1000)) / 100;

        /* Generate P_COMMENT */
        bzero(comment, sizeof(char) * 23);
        {
            int i;
            int comment_len = rand_within_int(5, 22);
            for (i = 0; i < comment_len; i++) comment[i] = 'X';
        }

        fprintf(part_file,
                "%ld|%s|%s|%s|%s|%d|%s|%ld|%s\n",
                partkey, name, mfgr, brand, type, size, container, retailprice, comment);
    }

    return NULL;
}

static void *
cust_thread_handler(void *arg)
{

    return NULL;
}


/* global function bodies */

void generate(gint argc, gchar **argv)
{
    int i;
    pthread_t part_thread;
    cust_thread_t *cust_threads;

    parse_args(argc, argv);

    g_print("== settings ==\n");
    g_print(" scale factor : %d\n", option.scalefactor);
    g_print(" parallelism  : %d\n", option.parallel);
    g_print(" verbosity    : %s\n", (option.verbose ? "true" : "false"));

    cust_threads = malloc(sizeof(cust_thread_t) * option.parallel);

    customer_file = fopen("customer.csv", "w");
    orders_file = fopen("orders.csv", "w");
    lineitem_file = fopen("lineitem.csv", "w");
    part_file = fopen("part.csv", "w");

    pthread_create(&part_thread, NULL, part_thread_handler, NULL);

    for (i = 0; i < option.parallel; i++)
    {
        cust_threads[i].offset = i;
        pthread_create(&cust_threads[i].pth, NULL, cust_thread_handler, &cust_threads[i]);
    }

    pthread_join(part_thread, NULL);
    for (i = 0; i < option.parallel; i++)
    {
        pthread_join(cust_threads[i].pth, NULL);
    }

    free(cust_threads);

    fclose(customer_file);
    fclose(orders_file);
    fclose(lineitem_file);
    fclose(part_file);
}
