/*
    Copyright (C) 2011, 2012 Sebastian Pancratz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "padic.h"

void padic_set_si(padic_t rop, slong op, const padic_ctx_t ctx)
{
    fmpz_set_si(padic_unit(rop), op);
    padic_val(rop) = 0;
    padic_reduce(rop, ctx);
}
