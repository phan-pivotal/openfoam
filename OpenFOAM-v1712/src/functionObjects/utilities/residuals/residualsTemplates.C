/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2015-2016 OpenFOAM Foundation
     \\/     M anipulation  | Copyright (C) 2015-2016 OpenCFD Ltd.
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

\*---------------------------------------------------------------------------*/

#include "residuals.H"
#include "volFields.H"
#include "ListOps.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
void Foam::functionObjects::residuals::writeFileHeader
(
    Ostream& os,
    const word& fieldName
) const
{
    typedef GeometricField<Type, fvPatchField, volMesh> fieldType;

    if (foundObject<fieldType>(fieldName))
    {
        typename pTraits<Type>::labelType validComponents
        (
            mesh_.validComponents<Type>()
        );

        for (direction cmpt=0; cmpt<pTraits<Type>::nComponents; cmpt++)
        {
            if (component(validComponents, cmpt) != -1)
            {
                writeTabbed
                (
                    os,
                    fieldName + word(pTraits<Type>::componentNames[cmpt])
                );
            }
        }
    }
}


template<class Type>
void Foam::functionObjects::residuals::writeResidual(const word& fieldName)
{
    typedef GeometricField<Type, fvPatchField, volMesh> fieldType;

    if (foundObject<fieldType>(fieldName))
    {
        const Foam::dictionary& solverDict = mesh_.solverPerformanceDict();

        if (solverDict.found(fieldName))
        {
            const List<SolverPerformance<Type>> sp
            (
                solverDict.lookup(fieldName)
            );

            const Type& residual = sp.first().initialResidual();

            typename pTraits<Type>::labelType validComponents
            (
                mesh_.validComponents<Type>()
            );

            for (direction cmpt=0; cmpt<pTraits<Type>::nComponents; cmpt++)
            {
                if (component(validComponents, cmpt) != -1)
                {
                    const scalar r = component(residual, cmpt);

                    file() << token::TAB << r;

                    const word resultName =
                        fieldName + word(pTraits<Type>::componentNames[cmpt]);
                    setResult(resultName, r);
                }
            }
        }
    }
}


// ************************************************************************* //
