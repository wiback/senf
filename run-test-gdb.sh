#!/bin/sh

# This script helps finding the origin of unhandled exceptions in the
# unit tests. The unit test framework will tell, that an exception has
# been caught by the test driver but will not tell, where in the code
# it has occured.
#
# This script will run the .test.bin test driver within gdb and will
# create a backtrace for every exception caught by the test driver.
#
# NOTE: If your unit test (excplicitly) writes output to stderr, this
# output will be lost
#
# NOTE: This works by setting a breakpoint in the std::exception
# constructor. This is, where the backtrace is created from. If you do
# some funky manipulations with your exceptions, the backtrace might
# not point to the throw statement, however this is very unusual. Of
# course, this only works, if all your exceptions are derived from
# std::exception but that should always be the case.

trap 'rm -f .run-test-gdb.cmd' 0 1 2 15

# This gdb script will set the breakpoint and will then run the test
# driver. Whenever the execution stops, it will print a backtrace and
# will then continue execution. This will produce superflous
# backtraces for exceptions which are handled correctly. These will be
# filtered out later.
cat >.run-test-gdb.cmd <<EOF
break std::exception::exception()
run --build_info=yes --log_level=test_suite
while 1
  bt
  c
end
EOF

# The perl script will filter out exceptions which are handled
# correctly (cought before the reach the unit test driver). It will
# also truncate the backtrace at the first stackframe within the unit
# test subsystem since we are only interested in the user code.
gdb -batch -x .run-test-gdb.cmd ./.test.bin 2>/dev/null | perl -e '
  $mode=0;
  while (<STDIN>) {
    if ($mode==0) {
      if (/^$/) {
        $mode=1;
      } else {
        print;
      }
    }
    elsif ($mode==1) {
      if (/^Breakpoint 1, exception/) {
        $mode=2;
        @l=();
      } else {
        print "\n";
        print;
        $mode=0;
      }
    }
    elsif ($mode==2) {
      if (/^(#?[0-9]| )/) {
        push @l,$_ if /^#/;
        $l[$#l] .= $_ if /^ /;
      } else {
        $mode=0;
        if (/: fatal error in /) {
          for (@l[1..$#l]) {
             last if /^#[0-9]+ +0x[0-9a-f]+ in boost::unit_test::ut_detail::invoker/;
             print;
          }
          print;
        } else {
          redo;
        }
      }
    }
  }
'
