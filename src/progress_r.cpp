#include <cpl_port.h>
#include <Rcpp.h>
#include <cli/progress.h>
using namespace Rcpp;

static SEXP global_pb = R_NilValue;

int CPL_STDCALL GDALTermProgressR(double dfComplete,
                                 const char *pszMessage,
                                 CPL_UNUSED void *pProgressArg)
{
  if (dfComplete == 0 || Rf_isNull(global_pb)) {
    std::string msg(pszMessage);
    if (msg.length() > 23) {
      msg = msg.substr(0, 10) + "..." +
        msg.substr(msg.length() - 10, msg.length());
    }
    List args =
      List::create(Named("name", msg.c_str()),
                   Named("clear", false),
                   Named("format_done", "Done."),
                   Named("format_failed", "Failed."));
    R_ReleaseObject(global_pb);
    global_pb = cli_progress_bar(1, args);
    R_PreserveObject(global_pb);
  } else if (dfComplete < 1 && CLI_SHOULD_TICK) {
    cli_progress_set(global_pb, dfComplete);
  } else if (dfComplete >= 1) {
    cli_progress_done(global_pb);
    R_ReleaseObject(global_pb);
    global_pb = R_NilValue;
    R_PreserveObject(global_pb);
  }

  return TRUE;
}
