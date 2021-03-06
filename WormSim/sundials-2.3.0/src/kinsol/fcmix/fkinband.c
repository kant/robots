/*
 * -----------------------------------------------------------------
 * $Revision: 1.2 $
 * $Date: 2006/07/22 01:58:06 $
 * ----------------------------------------------------------------- 
 * Programmer(s): Radu Serban @ LLNL
 * -----------------------------------------------------------------
 * Copyright (c) 2005, The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * -----------------------------------------------------------------
 * Fortran/C interface routines for KINSOL/KINBAND, for the case
 * of a user-supplied Jacobian approximation routine.
 * -----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include "fkinsol.h"     /* standard interfaces and global vars.*/
#include "kinsol_impl.h" /* definition of KINMem type           */

#include <kinsol/kinsol_band.h>

/*
 * ----------------------------------------------------------------
 * prototypes of the user-supplied fortran routines
 * ----------------------------------------------------------------
 */

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

extern void FK_BJAC(long int*, long int*, long int*, long int*,
                    realtype*, realtype*,
                    realtype*,
                    realtype*, realtype*, int*);

#ifdef __cplusplus
}
#endif

/*
 * ----------------------------------------------------------------
 * Function : FKIN_BANDSETJAC
 * ----------------------------------------------------------------
 */

void FKIN_BANDSETJAC(int *flag, int *ier)
{
  if (*flag == 0) {
    *ier = KINBandSetJacFn(KIN_kinmem, NULL, NULL);
  }
  else {
    *ier = KINBandSetJacFn(KIN_kinmem, (KINBandJacFn) FKINBandJac, NULL);
  }

  return;
}

/*
 * ----------------------------------------------------------------
 * Function : FKINBandJac
 * ----------------------------------------------------------------
 * C function FKINBandJac interfaces between KINSOL and a Fortran
 * subroutine FKBJAC for solution of a linear system with band
 * Jacobian approximation. Addresses are passed to FKBJAC for
 * the banded Jacobian and vector data.
 * Auxiliary data is assumed to be communicated by common blocks.
 * ----------------------------------------------------------------
 */

int FKINBandJac(long int N, long int mupper, long int mlower,
                BandMat J, N_Vector uu, N_Vector fval, void *jac_data,
                N_Vector vtemp1, N_Vector vtemp2)
{
  realtype *uu_data, *fval_data, *jacdata, *v1_data, *v2_data;
  long int eband;
  int ier;

  /* Initialize all pointers to NULL */
  uu_data = fval_data = jacdata = v1_data = v2_data = NULL;

  /* NOTE: The user-supplied routine should set ier to an
     appropriate value, but we preset the value to zero
     (meaning SUCCESS) so the user need only reset the
     value if an error occurred */
  ier = 0;

  /* Get pointers to vector data */
  uu_data   = N_VGetArrayPointer(uu);
  fval_data = N_VGetArrayPointer(fval);
  v1_data   = N_VGetArrayPointer(vtemp1);
  v2_data   = N_VGetArrayPointer(vtemp2);

  eband = (J->smu) + mlower + 1;
  jacdata = BAND_COL(J,0) - mupper;

  /* Call user-supplied routine */
  FK_BJAC(&N, &mupper, &mlower, &eband,
          uu_data, fval_data, 
          jacdata,
          v1_data, v2_data, &ier);

  return(ier);
}


