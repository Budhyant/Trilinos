#!/bin/bash
if [ "${Trilinos_TRACK}" == "" ] ; then
  export Trilinos_TRACK=Experimental
fi
$WORKSPACE/Trilinos/cmake/ctest/drivers/atdm/ats1/local-driver.sh
