#!/usr/bin/env perl
#
# Copyright (c) 2013 Cisco Systems, Inc.  All rights reserved.
#
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

#
# Generate Fortran interface strings
#

package OMPI::Interfaces;

use strict;

use FortranMPI::Emit::Interfaces;

use OMPI::Utils;
use OMPI::Constants;
use OMPI::Handles;

my $types_module_name = "mpi_f08_types";

sub EmitMPI {
    my $args = shift;

    my $output = ' -*- f90 -*-
! THIS FILE IS AUTOMATICALLY GENERATED!
! Manual edits will be lost
! ' . $args->{gen_str} . '
!

';
    $output .= "!
! This file is not compiled by itself.
! It is intended to be included in a modulefile.
!

#ifndef OMPI_MPI_MODULE_INTERFACES
#define OMPI_MPI_MODULE_INTERFACES

";

    $args->{types_module_name} = $types_module_name;
    $output .= FortranMPI::Emit::Interfaces::EmitMPI($args);

    $output .= "\n#endif\n";

    safe_write_file("ompi/mpi/fortran/use-mpi-ignore-tkr/mpi-module-interfaces.h",
                    $output);
}

sub EmitMPI_F08 {
    my $args = shift;

    my $output = '! -*- f90 -*-
! THIS FILE IS AUTOMATICALLY GENERATED!
! Manual edits will be lost
! ' . $args->{gen_str} . '
!

';
    $output .= "!
! This file is not compiled by itself.
! It is intended to be included in a modulefile.
!

#ifndef OMPI_MPI_F08_MODULE_INTERFACES
#define OMPI_MPI_F08_MODULE_INTERFACES

";

    $args->{types_module_name} = $types_module_name;
    $output .= FortranMPI::Emit::Interfaces::EmitMPI_F08($args);

    $output .= "\n#endif\n";

    safe_write_file("ompi/mpi/fortran/use-mpi-f08/mpi-f08-module-interfaces.h",
                    $output);
}
