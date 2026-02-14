#include <cpl_port.h>
#include <Rcpp.h>
#include <cli/progress.h>

static SEXP global_pb = R_NilValue;

int CPL_STDCALL GDALTermProgressR(double dfComplete,
                                  CPL_UNUSED const char *pszMessage,
                                  CPL_UNUSED void *pProgressArg)
{
    if (dfComplete == 0 || Rf_isNull(global_pb)) {
        if (!Rf_isNull(global_pb))
            cli_progress_done(global_pb);

        R_ReleaseObject(global_pb);
        global_pb = cli_progress_bar(1, NULL);
        R_PreserveObject(global_pb);
    }
    else if (dfComplete < 1) {
        cli_progress_set(global_pb, dfComplete);
    }
    else {
        cli_progress_done(global_pb);
        R_ReleaseObject(global_pb);
        global_pb = R_NilValue;
        R_PreserveObject(global_pb);
    }

    return TRUE;
}
