#include "uni-dbgen.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <pthread.h>

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

static void make_part(void);
static void make_customer(void);
static void make_orders_lineitem(void);

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

static double
rand_within_double(double from, double to)
{
    return from + ((to - from) * drand48());
}


static const char *p_name_bag[] = {
    "almond____", "antique___", "aquamarine", "azure_____", "beige_____",
    "bisque____", "black_____", "blanched__", "blue______", "blush_____",
    "brown_____", "burlywood_", "burnished_", "chartreuse", "chiffon___",
    "chocolate_", "coral_____", "cornflower", "cornsilk__", "cream_____",
    "cyan______", "dark______", "deep______", "dim_______", "dodger____",
    "drab______", "firebrick_", "floral____", "forest____", "frosted___",
    "gainsboro_", "ghost_____", "goldenrod_", "green_____", "grey______",
    "honeydew__", "hot_______", "indian____", "ivory_____", "khaki_____",
    "lace______", "lavender__", "lawn______", "lemon_____", "light_____",
    "lime______", "linen_____", "magenta___", "maroon____", "medium____",
    "metallic__", "midnight__", "mint______", "misty_____", "moccasin__",
    "navajo____", "navy______", "olive_____", "orange____", "orchid____",
    "pale______", "papaya____", "peach_____", "peru______", "pink______",
    "plum______", "powder____", "puff______", "purple____", "red_______",
    "rose______", "rosy______", "royal_____", "saddle____", "salmon____",
    "sandy_____", "seashell__", "sienna____", "sky_______", "slate_____",
    "smoke_____", "snow______", "spring____", "steel_____", "tan_______",
    "thistle___", "tomato____", "turquoise_", "violet____", "wheat_____",
    "white_____", "yellow____"};
static const int p_name_bag_size = 92;

static const char *p_type_syll1[] = {
    "STANDARD", "SMALL___", "MEDIUM__", "LARGE___", "ECONOMY_", "PROMO___"
};
static const char *p_type_syll2[] = {
    "ANODIZED_", "BURNISHED", "PLATED___", "POLISHED_", "BRUSHED__"
};
static const char *p_type_syll3[] = {
    "TIN___", "NICKEL", "BRASS_", "STEEL_", "COPPER"
};

static const char *p_container_syll1[] = {
    "SM___", "LG___", "MED__", "JUMBO", "WRAP_"
};
static const char *p_container_syll2[] = {
    "CASE", "BOX_", "BAG_", "JAR_", "PKG_", "PACK", "CAN_", "DRUM"
};

static void
make_part(void)
{
    long partkey;
    char name[128];
    char mfgr[64];
    char brand[64];
    char type[64];
    int size;
    char container[64];
    double retailprice;
    const char *comment = "CONSTANT LENGTH";

    srand48(0);

    for (partkey = 1; partkey <= option.scalefactor * 200000; partkey ++)
    {
        int M;

        /* Generate P_NAME */
        /* TODO: uniquely random generation */
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
        retailprice = (90000 + ((partkey / 10) % 20001) + 100 * (partkey % 1000)) / 100.0;

        fprintf(part_file,
                "%ld|%s|%s|%s|%s|%d|%s|%lf|%s\n",
                partkey, name, mfgr, brand, type, size, container, retailprice, comment);
    }
}


static char *c_mktsegment_bag[] = {
    "AUTOMOBILE", "BUILDING__", "FURNITURE_", "MACHINERY_", "HOUSHOLD__"
};

static void
make_customer(void)
{
    long custkey;
    long nr_customer = 150000 * option.scalefactor;

    char name[64];
    const char *address = "CONSTANT ADDRESS _____________";
    int nationkey;
    char phone[64];
    double acctbal;
    const char *mktsegment;
    const char *comment = "NO COMMENT! NO COMMENT! NO COMMENT! NO COMMENT! NO COMMENT! NO COMMENT! ";

    for (custkey = 1; custkey <= nr_customer; custkey ++)
    {
        /* Generate C_NAME */
        sprintf(name, "Customer#%ld", custkey);

        /* Generate C_NATIONKEY */
        nationkey = rand_within_int(0, 24);

        /* Generate C_PHONE */
        {
            int country_code = 10 + rand_within_int(0, 24);
            int local1 = rand_within_int(100, 999);
            int local2 = rand_within_int(100, 999);
            int local3 = rand_within_int(1000, 9999);
            sprintf(phone, "%d-%d-%d-%d", country_code, local1, local2, local3);
        }

        /* Generate C_ACCTBAL */
        acctbal = rand_within_double(-999.99, 9999.99);

        /* Generate C_MKTSEGMENT */
        mktsegment = c_mktsegment_bag[rand_within_int(0, 4)];

        fprintf(customer_file, "%ld|%s|%s|%d|%s|%.2lf|%s|%s\n",
                custkey, name, address, nationkey, phone, acctbal, mktsegment, comment);
    }
}

static inline void
swap(long *val1, long *val2)
{
    long tmp;
    tmp = *val1;
    *val1 = *val2;
    *val2 = tmp;
}


static char *o_orderpriority_bag[] = {
    "1-URGENT", "2-HIGH__", "3-MEDIUM", "4-NOT___", "5-LOW___"
};
static char *l_shipinstruct_bag[] = {
    "DELIVER IN PERSON", "COLLECT COD______", "NONE_____________", "TAKE BACK RETURN_"
};
static char *l_shipmode_bag[] = {
    "REG AIR", "AIR____", "RAIL___", "SHIP___", "TRUCK__", "MAIL___", "FOB____"
};

static void
make_orders_lineitem(void)
{
    long custkey;
    long custidx;
    long nr_customer = option.scalefactor * 150000;
    long nr_orders = nr_customer * 10;
    long *custkeys;

    long orderkey;
    char orderstatus = 'O';     /* FIXME */
    double totalprice = 0.0;    /* FIXME */
    time_t orderdate_epoch;
    struct tm orderdate_tm;
    char orderdate[64];
    const char *orderpriority;
    char clerk[20];
    int shippriority = 0;
    const char *comment = "NO COMMENT! NO COMMENT! NO COMMENT! NO COMMENT!";

    custkeys = malloc(sizeof(long) * nr_orders);
    for (custidx = 0; custidx <= nr_orders; custidx ++)
        custkeys[custidx] = custidx % nr_customer + 1;

    /* Shuffling custkeys */
    for (custidx = 0; custidx < nr_orders - 1; custidx++)
    {
        long a = rand_within_long(0, nr_orders - custidx - 1);
        swap(&custkeys[custidx], &custkeys[custidx + a]);
    }

    for (custidx = 0; custidx < nr_orders; custidx++)
    {
        int i;

        /* O_ORDERKEY */
        orderkey = custidx + 1;

        /* O_CUSTKEY */
        custkey = custkeys[custidx];

        /* O_ORDERDATE */
        orderdate_epoch = rand_within_int(694191600, 902156399);
        localtime_r(&orderdate_epoch, &orderdate_tm);
        strftime(orderdate, 63, "%Y-%m-%d", &orderdate_tm);

        /* O_ORDERPRIORITY */
        orderpriority = o_orderpriority_bag[rand_within_int(0, 4)];

        /* O_CLERK */
        sprintf(clerk, "Clerk#%09d", rand_within_int(1, option.scalefactor * 1000));

        /* O_SHIPPRIORITY */
        shippriority = 0;

        for (i = 0; i < 4; i++)
        {
            long partkey;
            long suppkey;
            int linenumber;
            int quantity;
            double extendedprice;
            double discount;
            double tax;
            char returnflag = 'N'; /* FIXME */
            char linestatus = 'O'; /* FIXME */
            time_t shipdate_epoch;
            struct tm shipdate_tm;
            char shipdate[64];
            time_t commitdate_epoch;
            struct tm commitdate_tm;
            char commitdate[64];
            time_t receiptdate_epoch;
            struct tm receiptdate_tm;
            char receiptdate[64];
            const char *shipinstruct;
            const char *shipmode;
            const char *comment = "NO COMMENT! NO COMMENT! ";

            /* L_PARTKEY */
            partkey = rand_within_long(1, option.scalefactor * 200000);

            /* L_SUPPKEY */
            suppkey = 1;        /* FIXME */

            /* L_LINENUMBER */
            linenumber = i;

            /* L_QUANTITY */
            quantity = rand_within_int(1, 50);

            /* L_EXTENDEDPRICE */
            extendedprice = ((90000 + ((partkey / 10) % 20001) + 100 * (partkey % 1000)) / 100.0) * quantity;

            /* L_DISCOUNT */
            discount = rand_within_double(0.0, 0.1);

            /* L_TAX */
            tax = rand_within_double(0.0, 0.08);

            /* L_SHIPDATE */
            shipdate_epoch = orderdate_epoch + rand_within_int(1, 121) * 24 * 3600;
            localtime_r(&shipdate_epoch, &shipdate_tm);
            strftime(shipdate, 63, "%Y-%m-%d", &shipdate_tm);

            /* L_COMMITDATE */
            commitdate_epoch = orderdate_epoch + rand_within_int(30, 90) * 24 * 3600;
            localtime_r(&commitdate_epoch, &commitdate_tm);
            strftime(commitdate, 63, "%Y-%m-%d", &commitdate_tm);

            /* L_RECEIPTDATE */
            receiptdate_epoch = orderdate_epoch + rand_within_int(1, 121) * 24 * 3600;
            localtime_r(&receiptdate_epoch, &receiptdate_tm);
            strftime(receiptdate, 63, "%Y-%m-%d", &receiptdate_tm);

            /* L_SHIPINSTRUCT */
            shipinstruct = l_shipinstruct_bag[rand_within_int(0, 3)];

            /* L_SHIPMODE */
            shipmode = l_shipmode_bag[rand_within_int(0, 6)];

            fprintf(lineitem_file, "%ld|%ld|%ld|%d|%d|%.2lf|%.2lf|%.2lf|%c|%c|%s|%s|%s|%s|%s|%s\n",
                    orderkey, partkey, suppkey, linenumber, quantity, extendedprice, discount,
                    tax, returnflag, linestatus, shipdate, commitdate, receiptdate,
                    shipinstruct, shipmode, comment);
        }

        fprintf(orders_file, "%ld|%ld|%c|%lf|%s|%s|%s|%d|%s\n",
                orderkey, custkey, orderstatus, totalprice, orderdate,
                orderpriority, clerk, shippriority, comment);
    }
}

/* global function bodies */

void generate(gint argc, gchar **argv)
{
    parse_args(argc, argv);

    g_print("== settings ==\n");
    g_print(" scale factor : %d\n", option.scalefactor);
    g_print(" parallelism  : %d\n", option.parallel);
    g_print(" verbosity    : %s\n", (option.verbose ? "true" : "false"));

    customer_file = fopen("customer.csv", "w");
    orders_file = fopen("orders.csv", "w");
    lineitem_file = fopen("lineitem.csv", "w");
    part_file = fopen("part.csv", "w");

    make_part();
    make_customer();
    make_orders_lineitem();

    fclose(customer_file);
    fclose(orders_file);
    fclose(lineitem_file);
    fclose(part_file);
}
