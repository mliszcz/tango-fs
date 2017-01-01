
#define FUSE_USE_VERSION 30

#include <TangoFuseOperations.hpp>

#include <cstdlib>
#include <string>

#include <filesystem.hpp>

struct TangoFsConfig {
    char* tangoHost;
};

enum ConfigKey {
    CONFIG_KEY_HELP,
    CONFIG_KEY_VERSION
};

#define TANGOFS_OPT(t, p, v) { t, offsetof(struct TangoFsConfig, p), v }

static struct fuse_opt myfs_opts[] = {
    TANGOFS_OPT("tango-host=%s", tangoHost, 0),
    TANGOFS_OPT("-H %s", tangoHost, 0),
    TANGOFS_OPT("--tango-host=%s", tangoHost, 0),
    FUSE_OPT_KEY("-V",        CONFIG_KEY_VERSION),
    FUSE_OPT_KEY("--version", CONFIG_KEY_VERSION),
    FUSE_OPT_KEY("-h",        CONFIG_KEY_HELP),
    FUSE_OPT_KEY("--help",    CONFIG_KEY_HELP),
    FUSE_OPT_END
};

static int myfs_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs)
{
    struct fuse_operations fuseDefaultOps;
     switch (key) {
     case CONFIG_KEY_HELP:
             fprintf(stderr,
               "usage: %s mountpoint [options]\n"
                    "\n"
                    "general options:\n"
                    "    -o opt,[opt...]        mount options\n"
                    "    -h   --help            print help\n"
                    "    -V   --version         print version\n"
                     "\n"
                     "Myfs options:\n"
                     "    -o mynum=NUM\n"
                     "    -o mystring=STRING\n"
                     "    -o mybool\n"
                     "    -o nomybool\n"
                     "    -n NUM           same as '-omynum=NUM'\n"
                     "    --mybool=BOOL    same as 'mybool' or 'nomybool'\n"
                     "\n"
                     , outargs->argv[0]);
             fuse_opt_add_arg(outargs, "-ho");
             fuse_main(outargs->argc, outargs->argv, &fuseDefaultOps, NULL);
             exit(1);

     case CONFIG_KEY_VERSION:
             fprintf(stderr, "Myfs version %s\n", "0.1.0");
             fuse_opt_add_arg(outargs, "--version");
             fuse_main(outargs->argc, outargs->argv, &fuseDefaultOps, NULL);
             exit(0);
     }
     return 1;
}

int main(int argc, char *argv[])
{
    // const std::string tango_host = std::string("TANGO_HOST=") + "172.17.0.1" + ":" + "10000";
    // putenv(const_cast<char*>(tango_host.c_str()));

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct TangoFsConfig conf;
    memset(&conf, 0, sizeof(conf));
    fuse_opt_parse(&args, &conf, myfs_opts, myfs_opt_proc);

    if (conf.tangoHost)
    printf("passed host is: %s", conf.tangoHost);

    TangoFuseOperations ops {};
    struct fuse_operations fuseOps = static_cast<fuse_operations>(ops);
    fuseOps.getattr = &filesystem::getattr;
    return fuse_main(args.argc, args.argv, &fuseOps, nullptr);
}
