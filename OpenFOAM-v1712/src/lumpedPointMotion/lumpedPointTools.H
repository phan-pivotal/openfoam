/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2016 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Namespace
    Foam::lumpedPointTools

Description
    A collection of utility functions for handling IO related to the
    lumped-mass movement.

SourceFiles
    lumpedPointTools.C

\*---------------------------------------------------------------------------*/

#ifndef lumpedPointTools_H
#define lumpedPointTools_H

#include "labelList.H"
#include "polyMesh.H"
#include "pointMesh.H"
#include "pointFields.H"
#include "Tuple2.H"

#include "lumpedPointState.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

typedef Tuple2<scalar, lumpedPointState> lumpedPointStateTuple;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace lumpedPointTools
{

//- Load a list of states from an Istream
List<lumpedPointStateTuple> lumpedPointStates(Istream& is);

//- Load a list of states from a file
List<lumpedPointStateTuple> lumpedPointStates(const fileName& file);


//- Return the 0 or constant points field
pointIOField points0Field(const polyMesh& mesh);

//- Return the patch-ids associated a "lumpedPointDisplacement" type
labelList lumpedPointPatchList(const pointVectorField& pvf);

//- Get the "pointDisplacement" at time 0 and use that to determine which
//  patches have a "lumpedPointDisplacement" type
labelList lumpedPointPatchList(const polyMesh& mesh);

} // End namespace lumpedPointTools

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
