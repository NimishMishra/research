/*
    Copyright (C) 2010 William Hart
    Copyright (C) 2010 Sebastian Pancratz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include <gmp.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpq_poly.h"

void _fmpq_poly_set_length(fmpq_poly_t poly, slong len)
{
    slong i;
    for (i = len; i < poly->length; i++)
        _fmpz_demote(poly->coeffs + i);
    poly->length = len;
}
